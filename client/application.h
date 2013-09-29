#pragma once

#include "client.h"
#include "types.h"

namespace Networking { class Connection; }

//! Client application
class App
{
public:

    App();

public:
    //! Start client
    void Run(const char* address, unsigned port);

    //! Send next random value to server
    void SendNextValue(const Networking::Connection* conn);

    //! Client-server async call handlers
    void OnConnected(const Networking::Connection* conn);
    void OnDisconnected(const Networking::Connection* conn);
    void OnRead(const Networking::Connection* conn, void* data, std::size_t size);
    void OnWrite(const Networking::Connection* conn, std::size_t bytes_transferred);

private:
    Networking::Context context;
    Networking::Client client;
};
