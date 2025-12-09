#pragma once

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Packet.hpp>
#include <vector>

#include "ClientSession.hpp"
#include "NewClientEvent.hpp"
#include "NewInputEvent.hpp"
#include "DeleteClientEvent.hpp"

class NetworkServer {
private:
    sf::TcpListener    listener;
    sf::UdpSocket      udp;
    sf::SocketSelector selector;

    std::vector<ClientSession> clients;

    int                         nextClientId = 0;
    std::vector<NewClientEvent> pendingNewClients;
    std::vector<NewInputEvent>  pendingInputs;
    std::vector<DeleteClientEvent> pendingDeleteClients;


public:
    NetworkServer() = default;
    ~NetworkServer();

    bool runTcp(unsigned short port);
    bool bindUdp(unsigned short port);
    bool start(unsigned short port);

    void poll();

    void sendToClient(ClientSession &client, sf::Packet &packet);

    std::vector<ClientSession> & getClients();

    std::vector<NewClientEvent> fetchNewClients();
    std::vector<NewInputEvent> fetchInputs();
    std::vector<DeleteClientEvent> fetchDeleteClients();

    void close();
};