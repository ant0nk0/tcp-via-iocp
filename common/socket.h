#pragma once

#include <WinSock2.h>

namespace Networking
{

class Socket
{
    bool _inited;
    SOCKET _socket;

public:
    Socket();
    ~Socket();

    SOCKET& GetSocket();
    unsigned long ResolveAddress(const char* address);
    void Init(const char* address, unsigned port);
};

} // namespace Networking