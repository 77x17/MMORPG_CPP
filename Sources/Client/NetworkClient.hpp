#pragma once

#include <SFML/Network.hpp>
#include <optional>
#include <vector>

#include "../Shared/InterfaceEntity.hpp"
#include "../Shared/NetworkingHelpers.hpp"

#include "WorldSnapshot.hpp"

class NetworkClient {
private:
    sf::TcpSocket      tcp;
    sf::UdpSocket      udp;
    sf::SocketSelector selector;
    std::string        host;
    unsigned short     port;

public:
    int assignedId = -1;

    NetworkClient(const std::string &_host, unsigned short _port);
    ~NetworkClient();

    bool connectTcp(const float &timeoutSeconds);
    bool bindUdp();
    bool connectAll(const float &timeoutSeconds = 1.0f);

    // non-blocking: send input via TCP
    void sendInputPacket(int seq, const sf::Vector2f &moveDir, bool isShooting);

    // non-blocking: poll for incoming packets
    std::optional<WorldSnapshot> pollReceive();

    void close();
};
