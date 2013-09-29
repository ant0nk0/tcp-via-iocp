#pragma once

#include "context_callbacks.h"
#include "acceptor.h"

#include <cstddef>
#include <WinSock2.h>
#include <mswsock.h>
#include <functional>

namespace Networking
{

class Connection;
class Context;

//! Asynchronous Tcp server based on IO Completion ports
class Server : public CallbacksExecutor
{
public:
    Server(Context& context);
    ~Server();

public:
    //! Initialization. Must be called before any other method
    void Init(const char* address, unsigned port);

    //! Start async wait for new connections
    void StartAccept();

    //! Start async write operation
    void AsyncWrite(const Connection* conn, void* data, std::size_t size);

    //! Start async read operation
    void AsyncRead(const Connection* conn);

    //! Run the server
    void Run();

private:
    //! Networking context
    Context& _context;

    //! New connections acceptor
    Acceptor _acceptor;
};

} // namespace Networking