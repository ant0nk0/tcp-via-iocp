#include "server.h"
#include "exceptions.h"
#include "connection.h"
#include "btree.h"
#include "types.h"
#include "tree_with_rms.h"

#include <functional>
#include <iostream>

#define LOG_DEBUG(message) std::cout << "Debug: " << message << std::endl;
#define LOG_ERROR(message) std::cout << "Error: " << message << std::endl;

class App
{
public:
    App()
    {
        server.SetOnClientConnectedCallback(std::bind(&App::OnClientConnected, this, std::placeholders::_1));
        server.SetOnClientDisconnectedCallback(std::bind(&App::OnClientDisconnected, this, std::placeholders::_1));
        server.SetOnReadCallback(std::bind(&App::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        server.SetOnWriteCallback(std::bind(&App::OnWrite, this, std::placeholders::_1, std::placeholders::_2));
    }

    void Run()
    {
        server.Init("127.0.0.1", 123);
        server.StartAccept();
        server.Run();
    }

    void OnClientConnected(const Networking::Connection* conn)
    {
        LOG_DEBUG("New client connected");
        server.ReadAsync(conn);
    }

    void OnClientDisconnected(const Networking::Connection* /*conn*/)
    {
        LOG_DEBUG("Client disconnected");
    }

    void OnRead(const Networking::Connection* conn, void* data, std::size_t size)
    {
        unsigned value = *reinterpret_cast<unsigned*>(data);
        LOG_DEBUG("Got new data. Size: " << size << ", data: " << value);

        Networking::Types::ResultType result = tree.AddNewValueAndCalculateRms(value);
        server.WriteAsync(conn, &result, sizeof(result));
    }

    void OnWrite(const Networking::Connection* conn, std::size_t bytes_transferred)
    {
        LOG_DEBUG("Successfully written " << bytes_transferred << " bytes");
        server.ReadAsync(conn);
    }

    Networking::Server server;
    TreeWithRms tree;
};
