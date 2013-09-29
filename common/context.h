#pragma once

#include "socket.h"
#include "exceptions.h"
#include "connection.h"
#include "winsock_initializer.h"
#include "overlapped.h"
#include "context_callbacks.h"

#include <functional>

namespace Networking
{

class Acceptor;
class Connector;

//! Main Network context.
//! Processes main async operations (AsyncRead, AsyncWrite) and handles all processed operations
class Context : public CallbacksHolder
{
    friend Acceptor;
    friend Connector;
    
public:
    Context();

    //! Initialize context with address and port
    void Init(const char* address, unsigned port);

    //! Start async read operation
    void AsyncRead(const Connection* conn);

    //! Start async write operation
    void AsyncWrite(const Connection* conn, void* data, std::size_t size);

    //! Main worker. Processes all completed async operations
    void MainLoop();

private:
    //! Get socket
    Socket& GetSocket();

    //! Get completion port handle
    HANDLE GetCompletionPort();

private:
    //! Helper for winsock initialization
    WinSockInitializer _winsock;

    //! Main socket. Used as listening socket for server and as working socket for client
    Socket _socket;

    //! Completion port handle
    HANDLE _completion_port;

    //! New connections acceptor
    Acceptor* _acceptor;
};

} // namespace Networking