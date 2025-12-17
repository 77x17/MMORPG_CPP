#pragma once

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

#include "Client/Snapshots/WorldSnapshot.hpp"
#include "Client/Snapshots/InventorySnapshot.hpp"
#include "Client/Snapshots/EquipmentSnapshot.hpp"
#include "Client/Snapshots/WorldCollisionSnapshot.hpp"

class NetworkClient {
private:
    sf::TcpSocket      tcp;
    sf::UdpSocket      udp;
    sf::SocketSelector selector;
    std::string        host;
    unsigned short     tcpPort;
    unsigned short     udpPort;

    WorldSnapshot          worldSnapshot;
    InventorySnapshot      inventorySnapshot;
    EquipmentSnapshot      equipmentSnapshot;
    WorldCollisionSnapshot worldCollisionSnapshot;
public:
    int assignedId = -1;

    NetworkClient(const std::string &_host, unsigned short _tcpPort, unsigned short _udpPort);
    ~NetworkClient();

    bool connectTcp(int clientId, const float &timeoutSeconds);
    bool bindUdp();
    bool connectAll(int clientId, const float &timeoutSeconds = 1.0f);

    // non-blocking: send input via TCP
    void sendInputPacket(int seq, const sf::Vector2f &moveDir, bool isShooting);
    void sendTcpPacket(sf::Packet &packet);

    // non-blocking: poll for incoming packets
    void pollReceive();

    WorldSnapshot & getWorldSnapshot();
    InventorySnapshot & getInventorySnapshot();
    EquipmentSnapshot & getEquipmentSnapshot();
    WorldCollisionSnapshot & getWorldCollisionSnapshot();

    void close();
};
