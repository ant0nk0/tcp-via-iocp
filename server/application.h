#pragma once

#include "tree_with_rms.h"
#include "server.h"

namespace Networking { class Connection; }

class App
{
public:
    App();

    void Run();

    void OnClientConnected(const Networking::Connection* conn);
    void OnClientDisconnected(const Networking::Connection* /*conn*/);
    void OnRead(const Networking::Connection* conn, void* data, std::size_t size);
    void OnWrite(const Networking::Connection* conn, std::size_t bytes_transferred);

private:
    Networking::Server server;
    TreeWithRms tree;
};