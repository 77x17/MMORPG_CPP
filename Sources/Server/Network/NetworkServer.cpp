#include "Server/Network/NetworkServer.hpp"

#include <SFML/Network/Packet.hpp>

#include <algorithm>

#include "Server/Systems/Log/LogSystem.hpp"

#include "Shared/TcpPacketType.hpp"
#include "Shared/UdpPacketType.hpp"

NetworkServer::~NetworkServer() {
    close();
}

bool NetworkServer::runTcp(unsigned short port) {
    if (listener.listen(port) != sf::Socket::Done) {
        LogSystem::addMessage("[Network] Cannot listen on port " + std::to_string(port));
        return false;
    }
    listener.setBlocking(false);
    selector.add(listener);

    return true;
}

bool NetworkServer::bindUdp(unsigned short port) {
    if (udp.bind(port) != sf::Socket::Done) {
        LogSystem::addMessage("[Network] Cannot bind UDP on port " + std::to_string(port));
        return false;
    }
    udp.setBlocking(false);
    selector.add(udp);

    return true;
}

bool NetworkServer::start(unsigned short tcpPort, unsigned short udpPort) {
    selector.clear();
    if (!(runTcp(tcpPort) && bindUdp(udpPort))) {
        return false;
    } 

    running = true;
    networkThread = std::thread(&NetworkServer::networkLoop, this);
    LogSystem::addMessage("[Network] Server running TCP on port " + std::to_string(tcpPort) + "...");  
    LogSystem::addMessage("[Network] Server running UDP on port " + std::to_string(udpPort) + "...");     

    return true;
}

void NetworkServer::stop() {
    running = false;
    if (networkThread.joinable()) {
        networkThread.join();
    }
}

void NetworkServer::networkLoop() {
    while (running) {
        bool didWork = false;

        didWork |= flushOutgoing();
        didWork |= pollTcp();
        didWork |= pollUdp();

        if (!didWork) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}

bool NetworkServer::flushOutgoing() {
    bool didWork = false;
    OutgoingPacket outgoingPacket;
    while (outgoingPackets.tryPop(outgoingPacket)) {
        ClientSession &client = getClient(outgoingPacket.clientId);
        if (outgoingPacket.useUdp) {
            auto status = udp.send(outgoingPacket.packet, client.udpId, client.udpPort);
            if (status != sf::Socket::Done) {
                LogSystem::addMessage("[Network] UDP failed on client " + std::to_string(client.id));
            }
        }
        else {
            auto status = client.tcp->send(outgoingPacket.packet);
            if (status != sf::Socket::Done) {
                LogSystem::addMessage("[Network] TCP failed on client " + std::to_string(client.id));
            }
        }
        didWork = true;
    }
    return didWork;
}

bool NetworkServer::isValidClientId(int id) const {
    if (id < 0) return false;

    for (const ClientSession &client : clients) {
        if (client.id == id) return false;
    }
    // for (const NewClientEvent & event : pendingNewClients) {
    //     if (event.clientId == id) return false;
    // }
    return true;
}

void NetworkServer::acceptClient() {
    sf::TcpSocket *newTcp = new sf::TcpSocket();
    if (listener.accept(*newTcp) == sf::Socket::Done) {
        newTcp->setBlocking(false);

        // === Pending create new player ===
        ClientSession newClient;
        newClient.id  = -1;
        newClient.tcp = newTcp;
        clients.push_back(newClient);
        selector.add(*newTcp);

        LogSystem::addMessage("[Network] Pending TCP from: " + newTcp->getRemoteAddress().toString() + ":" + std::to_string(newTcp->getRemotePort()));
    }
    else {
        delete newTcp;
    }
}

void NetworkServer::handleTcpPacket(ClientSession &client) {
    sf::Packet         packet;
    sf::Socket::Status status = client.tcp->receive(packet);
    if (status == sf::Socket::Done) {
        uint8_t type; packet >> type;
        if (static_cast<TcpPacketType>(type) == TcpPacketType::Login) {
            int requestedId; packet >> requestedId;

            if (isValidClientId(requestedId) == false) {
                sf::Packet packet; 
                packet << static_cast<uint8_t>(TcpPacketType::Login_Fail);
                client.tcp->send(packet);
            }
            else {   
                client.id = requestedId;

                NetworkEvent event;
                event.type = NetworkEventType::NewClient;
                event.clientId = requestedId;
                incomingEvents.push(event);

                sf::Packet packet; 
                packet << static_cast<uint8_t>(TcpPacketType::Assign_ID) << requestedId;
                sendAsync(requestedId, packet, false);
                
                LogSystem::addMessage("[Network] Register ID: " + std::to_string(requestedId) + " for: " + client.tcp->getRemoteAddress().toString() + ":" + std::to_string(client.tcp->getRemotePort()));
            }
        }
        else if (static_cast<TcpPacketType>(type) == TcpPacketType::MoveItem) {
            int from, to;
            packet >> from >> to;
            
            NetworkEvent event;
            event.type = NetworkEventType::MoveItem;
            event.clientId = client.id;
            event.from = from, event.to = to;
            incomingEvents.push(event);
        }
        else if (static_cast<TcpPacketType>(type) == TcpPacketType::EquipItem) {
            int fromInventory, toEquipment;
            packet >> fromInventory >> toEquipment;
            
            NetworkEvent event;
            event.type = NetworkEventType::EquipItem;
            event.clientId = client.id;
            event.from = fromInventory, event.to = toEquipment;
            incomingEvents.push(event);
        }
        else if (static_cast<TcpPacketType>(type) == TcpPacketType::TcpPing) {
            uint64_t clientTime; packet >> clientTime;
            
            sf::Packet replyPacket;
            replyPacket << static_cast<uint8_t>(TcpPacketType::TcpPing) << clientTime;

            client.tcp->send(replyPacket);
        }
        else if (static_cast<TcpPacketType>(type) == TcpPacketType::MouseSelect) {
            sf::Vector2f selectPosition;
            float x, y; packet >> x >> y;

            NetworkEvent event;
            event.type = NetworkEventType::MouseSelect;
            event.clientId = client.id;
            event.from = x, event.to = y;
            incomingEvents.push(event);
        }
        else {
            LogSystem::addMessage("[Network] TCP received undefine type " + type);
        }
    }
    else if (status == sf::Socket::Disconnected) {
        LogSystem::addMessage("[Network] Disconnected TCP from " + client.tcp->getRemoteAddress().toString() + ":" + std::to_string(client.tcp->getRemotePort()));
        selector.remove(*client.tcp);
        
        client.tcp->disconnect();
        
        client.state = ClientState::Disconnected;
        
        if (client.id != -1) {
            NetworkEvent event;
            event.type = NetworkEventType::Disconnect;
            event.clientId = client.id;
            incomingEvents.push(event);
        }
    }
}

bool NetworkServer::pollTcp() {
    bool didWork = false;
    // === Non-blocking: Accept new TCP clients / read TCP inputs ===
    // if (selector.wait(sf::microseconds(50))) {
    if (selector.wait(sf::milliseconds(0))) {
        if (selector.isReady(listener)) {
            acceptClient();
            didWork = true;
        }

        for (ClientSession &client : clients) {
            if (selector.isReady(*client.tcp)) {
                handleTcpPacket(client);
                didWork = true;
            }
        }
    }
    return didWork;
}

bool NetworkServer::pollUdp() {
    bool didWork = false;
    // === Receive UDP ===
    for (int i = 0; i < 16; ++i) {
        sf::Packet         packet;
        sf::IpAddress      sender;
        unsigned short     senderPort;
        sf::Socket::Status status = udp.receive(packet, sender, senderPort);
        if (status != sf::Socket::Done) break;

        didWork = true;

        std::uint8_t type; packet >> type;
        // === UDP Ping === 
        if (static_cast<UdpPacketType>(type) == UdpPacketType::UdpPing) {
            uint64_t clientTime; packet >> clientTime;
            
            sf::Packet replyPacket;
            replyPacket << static_cast<uint8_t>(UdpPacketType::UdpPing) << clientTime;

            udp.send(replyPacket, sender, senderPort);
            continue;
        }
        // === Gameplay ===
        if (static_cast<UdpPacketType>(type) == UdpPacketType::Assign_UDP) {
            int clientId; packet >> clientId;
            for (ClientSession &client : clients) {
                if (client.id == clientId) {
                    client.udpId   = sender; 
                    client.udpPort = senderPort;
                    LogSystem::addMessage("[Network] Register UDP for " + sender.toString() + ":" + std::to_string(senderPort));
                    break;
                }
            }
        }
        else if (static_cast<UdpPacketType>(type) == UdpPacketType::Input) {
            NetworkEvent event;
            event.type = NetworkEventType::Input;
            packet >> event.clientId
                   >> event.input.seq
                   >> event.input.movementDir.x
                   >> event.input.movementDir.y
                   >> event.input.isShooting;
            incomingEvents.push(event);
        }
        else {
            LogSystem::addMessage("[Network] UDP received undefine type: " + type);
        }
    }
    return didWork;
}

void NetworkServer::sendToClientUdp(ClientSession &client, sf::Packet &packet) {    
    auto status = udp.send(packet, client.udpId, client.udpPort);
    if (status != sf::Socket::Done) {
        LogSystem::addMessage("[UDP] Send failed to client " + std::to_string(client.id));
    }
}

void NetworkServer::sendToClientTcp(int clientId, sf::Packet &packet) {    
    for (ClientSession &client : clients) {
        if (client.id == clientId) {
            client.tcp->send(packet);
            break;
        }
    }
}

ClientSession & NetworkServer::getClient(int clientId) {
    for (ClientSession &client : clients) {
        if (client.id == clientId) {
            return client;
        }
    }
    throw std::out_of_range("[Network] getClient() out of range");
}

std::vector<ClientSession> & NetworkServer::getClients() {
    return clients;
}

const std::vector<ClientSession> & NetworkServer::getClients() const {
    return clients;
}

void NetworkServer::fetchEvents(std::vector<NetworkEvent> &outEvents) {
    incomingEvents.drain(outEvents);
}

void NetworkServer::sendAsync(int clientId, sf::Packet packet, bool udp) {
    outgoingPackets.push({ clientId, udp, packet });
}

void NetworkServer::cleanupDisconnectedClients() {
    clients.erase(
        std::remove_if(clients.begin(), clients.end(), 
            [] (ClientSession &client) {
                if (client.state == ClientState::Disconnected) {
                    delete client.tcp;
                    return true;
                }
                return false;
            }
        ),
        clients.end()
    );
}

void NetworkServer::close() {
    listener.close();
    udp.unbind();
}