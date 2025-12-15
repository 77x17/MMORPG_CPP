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
#include "MoveItemEvent.hpp"
#include "EquipItemEvent.hpp"

class NetworkServer {
private:
    sf::TcpListener    listener;
    sf::UdpSocket      udp;
    sf::SocketSelector selector;

    std::vector<ClientSession> clients;

    std::vector<NewClientEvent>    pendingNewClients;
    std::vector<NewInputEvent>     pendingInputs;
    std::vector<DeleteClientEvent> pendingDeleteClients;
    std::vector<MoveItemEvent>     pendingMoveItems;
    std::vector<EquipItemEvent>    pendingEquipItems;

public:
    NetworkServer() = default;
    ~NetworkServer();

    bool runTcp(unsigned short port);
    bool bindUdp(unsigned short port);
    bool start(unsigned short tcpPort, unsigned short udpPort);

    bool isValidClientId(int id) const;
    int generateNewClientId() const;

    void poll();

    void sendToClientUdp(ClientSession &client, sf::Packet &packet);
    void sendToClientTcp(int clientId, sf::Packet &packet);

    ClientSession & getClient(int clientId);
    std::vector<ClientSession> & getClients();

    std::vector<NewClientEvent>    & fetchNewClients();
    std::vector<NewInputEvent>     & fetchInputs();
    std::vector<DeleteClientEvent> & fetchDeleteClients();
    std::vector<MoveItemEvent>     & fetchMoveItems();
    std::vector<EquipItemEvent>    & fetchEquipItems();

    void close();
};