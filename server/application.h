#pragma once

#include "tree_with_rms.h"
#include "server.h"
#include "context.h"

namespace Networking { class Connection; }

class App
{
public:
    App();

    //! Start the server
    void Run(const char* address, unsigned port);

    //! Async call handlers
    void OnClientConnected(const Networking::Connection* conn);
    void OnClientDisconnected(const Networking::Connection* /*conn*/);
    void OnRead(const Networking::Connection* conn, void* data, std::size_t size);
    void OnWrite(const Networking::Connection* conn, std::size_t bytes_transferred);

private:
    //! Networking context
    Networking::Context context;

    //! Tcp server implementation
    Networking::Server server;

    //! Binary tree with rms
    TreeWithRms tree;
};