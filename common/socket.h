#pragma once

#include <WinSock2.h>

namespace Networking
{

//! Socket helper class. Stores native socket and associated address
class Socket
{
public:
    Socket();
    ~Socket();

public:
    //! Returns native socket
    SOCKET& Native();

    //! Returns address associated with the socket
    sockaddr_in& GetAddress();

    //! Initialize the socket and address
    void Init(const char* address, unsigned port);

    //! Bind socket to address. User when the socket is listening (server side)
    void Bind();

    //! Bind socket to NULL address. User when the socket is connecting to the server (client side)
    void BindBeforeConnect();

private:
    //! Helper for resolve text address to ip
    unsigned long ResolveAddress(const char* address);

private:
    //! Native socket
    SOCKET _socket;

    //! Native address
    sockaddr_in _socket_address;

    //! Flag the socket was initialized
    bool _inited;
};

} // namespace Networking