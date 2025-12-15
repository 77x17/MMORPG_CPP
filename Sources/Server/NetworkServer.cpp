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

bool NetworkServer::start(unsigned short tcpPort, unsigned short udpPort) {
    selector.clear();
    if (!(runTcp(tcpPort) && bindUdp(udpPort))) {
        return false;
    } 
    std::cout << "[Network] - Server running TCP on port " << tcpPort << "..." << '\n';        
    std::cout << "[Network] - Server running UDP on port " << udpPort << "..." << '\n';        

    return true;
}

bool NetworkServer::isValidClientId(int id) const {
    for (const ClientSession &client : clients) {
        if (client.id == id) return false;
    }
    return true;
}

int NetworkServer::generateNewClientId() const {
    int *flag = new int[clients.size() + 1]();
    for (const ClientSession &client : clients) {
        if (client.id <= (int)clients.size()) {
            flag[client.id] = true;
        }
    }

    int newId = -1;
    for (size_t i = 0; i <= clients.size(); ++i) {
        if (flag[i] == false) {
            newId = i;
            break;
        }
    }
    delete[] flag;

    return newId;
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

                        int finalId;
                        if (requestedId != -1 && isValidClientId(requestedId)) {
                            finalId = requestedId;
                        }
                        else {
                            // finalId = generateNewClientId();
                            
                        }
                        
                        clients[i].id = finalId;

                        NewClientEvent newClientEvent;
                        newClientEvent.clientId = finalId;
                        pendingNewClients.push_back(newClientEvent);

                        sf::Packet packet; packet << std::string("Assign_ID") << finalId;
                        tcpSocket.send(packet);

                        std::cout << "[Network] - Client connected ID = " << finalId << '\n';
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
                        std::cout << "[Network] - Received undefine type: " << type << '\n';
                    }
                }
                else 
                if (status == sf::Socket::Disconnected) {
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
            std::cout << "[Network] - UDP Received undefine type: " << type << '\n';
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
    throw std::out_of_range("[Network] - getClient() out of range");
}

std::vector<ClientSession> & NetworkServer::getClients() {
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