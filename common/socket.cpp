#include "socket.h"
#include "guard.h"
#include "exceptions.h"
#include "connection.h"

#include <functional>

namespace Networking
{

Socket::Socket() 
    : _inited(false)
    , _socket()
{
}

Socket::~Socket()
{
    if (_inited)
        WSACleanup();
}

void Socket::Init(const char* address, unsigned port)
{
    if (_inited)
        return;

    _socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    WSA_CHECK(_socket != INVALID_SOCKET, "Failed to initialize socket");

    Utils::Guard socket_guard([&] { closesocket(_socket); });

    unsigned long resolved_address = ResolveAddress(address);
    WSA_CHECK(resolved_address != INADDR_NONE, "Failed to resolve address");

    memset(&_socket_address, 0, sizeof(_socket_address));
    _socket_address.sin_family = AF_INET;
    _socket_address.sin_addr.s_addr = resolved_address;
    _socket_address.sin_port = htons(port);

    socket_guard.Release();
    _inited = true;
}

void Socket::Bind()
{
    CHECK(_inited, "Socket must be initialized before Bind() call");

    WSA_CHECK
    (
        !bind(_socket, reinterpret_cast<SOCKADDR*>(&_socket_address), sizeof(_socket_address)),
        "Failed to bind socket to address/port"
    );
}

void Socket::BindBeforeConnect()
{
    // ConnectEx requires the socket to be initially bound
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = 0;

    WSA_CHECK
    (
        !bind(_socket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)),
        "Failed to bound socket"
    );
}

unsigned long Socket::ResolveAddress(const char* address)
{
    if (!strlen(address))
        return INADDR_ANY;

    hostent* host = gethostbyname(address);
    return inet_addr(inet_ntoa(*reinterpret_cast<struct in_addr *>(*host->h_addr_list)));
}

SOCKET& Socket::Native()
{
    return _socket;
}

sockaddr_in& Socket::GetAddress()
{
    return _socket_address;
}

} // namespace Networking