#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <functional>
#include <sstream>

#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <mswsock.h>

#include "guard.h"
#include "exceptions.h"
#include "connection.h"


class Socket
{
    bool _inited;
    SOCKET _socket;

public:
    Socket();
    ~Socket();

    SOCKET& GetSocket();
    unsigned long ResolveAddress(const std::string& address);
    void Init(const std::string& address, unsigned port);
};
