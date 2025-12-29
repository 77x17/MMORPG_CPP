#pragma once

#include <thread>
#include <atomic>

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Packet.hpp>
#include <vector>

#include "Server/Network/ClientSession.hpp"

#include "Server/Utils/ThreadSafeQueue/ThreadSafeQueue.hpp"
#include "Server/Network/NetworkEvent.hpp"
#include "Server/Network/OutgoingPacket.hpp"

class NetworkServer {
private:
    // === THREAD ===
    std::thread networkThread;
    std::atomic<bool> running{ false };

    // === SOCKET ===
    sf::TcpListener    listener;
    sf::UdpSocket      udp;
    sf::SocketSelector selector;

    std::vector<ClientSession> clients;

    // === QUEUE ===
    ThreadSafeQueue<NetworkEvent>   incomingEvents;
    ThreadSafeQueue<OutgoingPacket> outgoingPackets;

private:
    // === THREAD ===
    void networkLoop();
    bool pollTcp();
    bool pollUdp();
    bool flushOutgoing();

    bool runTcp(unsigned short port);
    bool bindUdp(unsigned short port);

    void acceptClient();
    void handleTcpPacket(ClientSession &client);

public:
    NetworkServer() = default;
    ~NetworkServer();

    bool start(unsigned short tcpPort, unsigned short udpPort);
    void stop();

    bool isValidClientId(int id) const;

    void sendToClientUdp(ClientSession &client, sf::Packet &packet);
    void sendToClientTcp(int clientId, sf::Packet &packet);

    ClientSession & getClient(int clientId);
    std::vector<ClientSession> & getClients();
    const std::vector<ClientSession> & getClients() const;

    void fetchEvents(std::vector<NetworkEvent> &outEvents);
    void sendAsync(int clientId, sf::Packet packet, bool udp);

    void cleanupDisconnectedClients();

    void close();
};