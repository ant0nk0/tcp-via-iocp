#pragma once

#include "socket.h"
#include "winsock_initializer.h"
#include "context.h"
#include "connector.h"

#include <cstddef>
#include <WinSock2.h>
#include <functional>
#include "context_callbacks.h"

namespace Networking
{

class Connection;

//! Asynchronous Tcp client based on IO Completion ports
class Client : public CallbacksExecutor
{
    Context& _context;
    Connector _connector;

public:
    Client(Context& context);
    ~Client();

    //! Initialization. Must be called before any other method
    void Init(const char* address, unsigned port);

    //! Start async write operation
    void AsyncWrite(const Connection* conn, void* data, std::size_t size);

    //! Start async read operation
    void AsyncRead(const Connection* conn);

    //! Run client
    void Run();
};

} // namespace Networking