#pragma once

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>

// === Multithreading ===
#include <thread>
#include <atomic>
#include <mutex>

#include "Client/Network/LoginStatusType.hpp"

#include "Client/PingTracker/TcpPingTracker.hpp"
#include "Client/PingTracker/UdpPingTracker.hpp"

#include "Client/Snapshots/ChunkSnapshot.hpp"
#include "Client/Snapshots/EquipmentSnapshot.hpp"
#include "Client/Snapshots/InventorySnapshot.hpp"
#include "Client/Snapshots/WorldCollisionSnapshot.hpp"
#include "Client/Snapshots/WorldSnapshot.hpp"

class NetworkClient {
private:
    // === Multithreading ===
    std::thread networkThread;
    std::atomic<bool> running{ false };
    std::mutex snapshotMutex;           // to protect snapshot (race condition)
    sf::Clock tcpPingClock;
    sf::Clock udpPingClock;

    bool isConnected = false;
    LoginStatus loginStatus = LoginStatus::None;

    sf::TcpSocket      tcp;
    sf::UdpSocket      udp;
    sf::SocketSelector selector;
    std::string        host;
    unsigned short     tcpPort;
    unsigned short     udpPort;

    ChunkSnapshot          chunkSnapshot;
    EquipmentSnapshot      equipmentSnapshot;
    InventorySnapshot      inventorySnapshot;
    WorldCollisionSnapshot worldCollisionSnapshot;
    WorldSnapshot          worldSnapshot;

    TcpPingTracker tcpPingTracker;
    UdpPingTracker udpPingTracker;

    int clientId = -1;
    int entityId = -1;

private:
    void networkLoop();
    void sendPingIfNeeded();

public:
    NetworkClient(const std::string &_host, unsigned short _tcpPort, unsigned short _udpPort);
    ~NetworkClient();

    bool connectTcp(const float &timeoutSeconds);
    bool bindUdp();
    bool connectAll(const float &timeoutSeconds = 1.0f);

    void startNetworkThread();
    void stopNetworkThread();

    // non-blocking: send input via TCP
    void sendInputPacket(int seq, const sf::Vector2f &moveDir, bool isShooting);
    void sendTcpPacket(sf::Packet &packet);

    void resetLoginStatus();
    
    void sendLogin(int clientId);
    
    // non-blocking: poll for incoming packets
    void pollTCP();
    void pollUDP();
    void poll();

    int getClientId() const;
    int getEntityId() const;

    LoginStatus getLoginStatus() const;

    ChunkSnapshot getChunkSnapshot();
    EquipmentSnapshot getEquipmentSnapshot();
    InventorySnapshot getInventorySnapshot();
    WorldCollisionSnapshot getWorldCollisionSnapshot();
    WorldSnapshot getWorldSnapshot();

    int getTcpPing() const;
    int getUdpPing() const;

    void close();
};
