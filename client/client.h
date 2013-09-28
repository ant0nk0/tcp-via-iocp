#pragma once

#include "socket.h"
#include "winsock_initializer.h"

#include <cstddef>
#include <WinSock2.h>
#include <functional>
#include <memory>

namespace Networking
{

// forward declarations
class Connection;

//! Asynchronous Tcp client based on IO Completion ports
class Client
{
    //! Helper for winsock initialization
    WinSockInitializer _winsock;

    //! The Socket for outgoing connection
    Socket _socket;

    //! Completion port handle
    HANDLE _completion_port;
    
    //! Client's connection to server
    std::unique_ptr<Connection> _connection;

    //! OnWrite callback called when data was successfully transferred via socket
    typedef std::function<void(const Connection*, std::size_t)> OnWriteCallback;
    OnWriteCallback OnWrite;

    //! OnRead callback called when data was successfully read from socket
    typedef std::function<void(const Connection*, void*, std::size_t)> OnReadCallback;
    OnReadCallback OnRead;

    //! OnServerConnected callback called when new client connected to listening socket
    typedef std::function<void(const Connection*)> OnConnectedCallback;
    OnConnectedCallback OnConnected;

    //! OnServerDisconnected callback called when server closed connection
    typedef std::function<void(const Connection*)> OnDisconnectedCallback;
    OnDisconnectedCallback OnDisconnected;

public:
    Client();
    ~Client();

    //! Initialization. Must be called before any other method
    void Init(const char* address, unsigned port);

    //! Start async write operation
    void AsyncWrite(void* data, std::size_t size);

    //! Start async read operation
    void AsyncRead();

    //! Run the server
    void Run();

    //! Check server was initialized
    void CheckInited() const;

public:
    void SetOnWriteCallback(OnWriteCallback callback);
    void SetOnReadCallback(OnReadCallback callback);
    void SetOnConnectedCallback(OnConnectedCallback callback);
    void SetOnDisconnectedCallback(OnDisconnectedCallback callback);
};

} // namespace Networking