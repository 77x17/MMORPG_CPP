#include "Server/Network/NetworkServer.hpp"

#include <SFML/Network/Packet.hpp>

#include "Server/Systems/Log/LogSystem.hpp"

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
    LogSystem::addMessage("[Network] Server running TCP on port " + std::to_string(tcpPort) + "...");  
    LogSystem::addMessage("[Network] Server running UDP on port " + std::to_string(udpPort) + "...");     

    return true;
}

bool NetworkServer::isValidClientId(int id) const {
    if (id < 0) return false;

    for (const ClientSession &client : clients) {
        if (client.id == id) return false;
    }
    for (const NewClientEvent & event : pendingNewClients) {
        if (event.clientId == id) return false;
    }
    return true;
}

void NetworkServer::poll() {
    // === Non-blocking: Accept new TCP clients / read TCP inputs ===
    if (selector.wait(sf::milliseconds(1))) {
        // === Accept new client ===
        if (selector.isReady(listener)) {
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

        // === Handle client packets ===
        for (size_t i = 0; i < clients.size(); ++i) {
            sf::TcpSocket &tcpSocket = *clients[i].tcp;
            if (selector.isReady(tcpSocket)) {
                sf::Packet         packet;
                sf::Socket::Status status = tcpSocket.receive(packet);
                if (status == sf::Socket::Done) {
                    std::string type; packet >> type;
                    if (type == "Login") {
                        int requestedId; packet >> requestedId;

                        if (isValidClientId(requestedId) == false) {
                            sf::Packet packet; packet << std::string("Login_Fail");
                            tcpSocket.send(packet);
                        }
                        else {   
                            clients[i].id = requestedId;
    
                            NewClientEvent newClientEvent;
                            newClientEvent.clientId = requestedId;
                            pendingNewClients.push_back(newClientEvent);
    
                            sf::Packet packet; packet << std::string("Assign_ID") << requestedId;
                            tcpSocket.send(packet);
                            
                            LogSystem::addMessage("[Network] Register ID: " + std::to_string(requestedId) + " for: " + tcpSocket.getRemoteAddress().toString() + ":" + std::to_string(tcpSocket.getRemotePort()));
                        }
                    }
                    else if (type == "MoveItem") {
                        int from, to;
                        packet >> from >> to;
                        pendingMoveItems.push_back({ clients[i].id, from, to });
                    }
                    else if (type == "EquipItem") {
                        int fromInventory, toEquipment;
                        packet >> fromInventory >> toEquipment;
                        pendingEquipItems.push_back({ clients[i].id, fromInventory, toEquipment });
                    }
                    else {
                        LogSystem::addMessage("[Network] TCP received undefine type " + type);
                    }
                }
                else 
                if (status == sf::Socket::Disconnected) {
                    LogSystem::addMessage("[Network] Disconnected TCP from " + tcpSocket.getRemoteAddress().toString() + ":" + std::to_string(tcpSocket.getRemotePort()));
                    selector.remove(tcpSocket);
                    
                    tcpSocket.disconnect();
                    
                    delete clients[i].tcp;
                    
                    if (clients[i].id != -1) {
                        DeleteClientEvent deleteClientEvent;
                        deleteClientEvent.clientId = clients[i].id;
                        pendingDeleteClients.push_back(deleteClientEvent);
                    }

                    
                    clients.erase(clients.begin() + i);
                    --i;
                }
            }
        }
    }

    // === Receive UDP ===
    while (true) {
        sf::Packet         packet;
        sf::IpAddress      sender;
        unsigned short     senderPort;
        sf::Socket::Status status = udp.receive(packet, sender, senderPort);
        if (status != sf::Socket::Done) break;

        std::string type; packet >> type;
        if (type == "Assign_UDP") {
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
        else if (type == "Input") {
            int clientId;
            InputState input;

            packet >> clientId
                   >> input.seq
                   >> input.movementDir.x
                   >> input.movementDir.y
                   >> input.isShooting;

            NewInputEvent newInputEvent;
            newInputEvent.clientId = clientId;
            newInputEvent.input    = input;
            pendingInputs.push_back(newInputEvent);
        }
        else {
            LogSystem::addMessage("[Network] UDP received undefine type: " + type);
        }
    }
}

void NetworkServer::sendToClientUdp(ClientSession &client, sf::Packet &packet) {    
    udp.send(packet, client.udpId, client.udpPort);
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

std::vector<NewClientEvent> & NetworkServer::fetchNewClients() {
    return pendingNewClients;
}

std::vector<NewInputEvent> & NetworkServer::fetchInputs() {
    return pendingInputs;
}

std::vector<DeleteClientEvent> & NetworkServer::fetchDeleteClients() {
    return pendingDeleteClients;
}

std::vector<MoveItemEvent> & NetworkServer::fetchMoveItems() {
    return pendingMoveItems;
}

std::vector<EquipItemEvent> & NetworkServer::fetchEquipItems() {
    return pendingEquipItems;
}

void NetworkServer::close() {
    listener.close();
    udp.unbind();
}