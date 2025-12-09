#include "NetworkServer.hpp"

#include <SFML/Network/Packet.hpp>
#include <iostream>

NetworkServer::~NetworkServer() {
    close();
}

bool NetworkServer::runTcp(unsigned short port) {
    if (listener.listen(port) != sf::Socket::Done) {
        std::cout << "[Network] - Cannot listen on port " << port << '\n';
        return false;
    }
    listener.setBlocking(false);
    selector.add(listener);

    return true;
}

bool NetworkServer::bindUdp(unsigned short port) {
    if (udp.bind(port) != sf::Socket::Done) {
        std::cout << "[Network] - Cannot bind UDP on port " << port << '\n';
        return false;
    }
    udp.setBlocking(false);
    selector.add(udp);

    return true;
}

bool NetworkServer::start(unsigned short port) {
    selector.clear();
    if (!(runTcp(port) && bindUdp(port))) {
        return false;
    } 
    std::cout << "[Network] - Server running on port " << port << "..." << '\n';        

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
                int newId = ++nextClientId;
                ClientSession newClient;
                newClient.id  = newId;
                newClient.tcp = newTcp;
                clients.push_back(newClient);
                selector.add(*newTcp);

                NewClientEvent newClientEvent;
                newClientEvent.clientId = newId;
                pendingNewClients.push_back(newClientEvent);

                sf::Packet packet; packet << std::string("Assign_ID") << newId;
                newTcp->send(packet);

                std::cout << "[Network] - Client connected ID = " << newId << '\n';
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
                    if (type == "Input") {
                        // === Handle input ===
                        InputState input;
                        packet >> input.seq >> input.movementDir.x >> input.movementDir.y >> input.isShooting;
                        
                        NewInputEvent newInputEvent;
                        newInputEvent.clientId = clients[i].id;
                        newInputEvent.input    = input;
                        pendingInputs.push_back(newInputEvent);
                    }
                    else {
                        std::cout << "[Network] - Received undefine type: " << type << '\n';
                    }
                }
                else if (status == sf::Socket::Disconnected) {
                    std::cout << "[Network] - Client TCP disconnected ID = " << clients[i].id << '\n';
                    
                    selector.remove(tcpSocket);
                    tcpSocket.disconnect();

                    delete clients[i].tcp;
                    
                    DeleteClientEvent deleteClientEvent;
                    deleteClientEvent.clientId = clients[i].id;
                    pendingDeleteClients.push_back(deleteClientEvent);

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
                    std::cout << "[Network] - Register UDP endpoint for ID = " << client.id << " " << sender << ':' << senderPort << '\n';
                    break;
                }
            }
        }
    }
}

void NetworkServer::sendToClient(ClientSession &client, sf::Packet &packet) {    
    udp.send(packet, client.udpId, client.udpPort);
}

std::vector<ClientSession> & NetworkServer::getClients() {
    return clients;
}

std::vector<NewClientEvent> NetworkServer::fetchNewClients() {
    std::vector<NewClientEvent> temp = pendingNewClients;
    pendingNewClients.clear();
    return temp;
}

std::vector<NewInputEvent> NetworkServer::fetchInputs() {
    std::vector<NewInputEvent> temp = pendingInputs;
    pendingInputs.clear();
    return temp;
}

std::vector<DeleteClientEvent> NetworkServer::fetchDeleteClients() {
    std::vector<DeleteClientEvent> temp = pendingDeleteClients;
    pendingDeleteClients.clear();
    return temp;
}


void NetworkServer::close() {
    listener.close();
    udp.unbind();
}