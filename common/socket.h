#pragma once

#include <WinSock2.h>

namespace Networking
{

class Socket
{
    SOCKET _socket;
    sockaddr_in _socket_address;
    bool _inited;

public:
    Socket();
    ~Socket();

    SOCKET& GetSocket();
    sockaddr_in& GetAddress();

    void Init(const char* address, unsigned port);
    void Bind();

private:
    unsigned long ResolveAddress(const char* address);
};

} // namespace Networking