#include "application.h"
#include "server.h"
#include "exceptions.h"
#include "connection.h"
#include "btree.h"
#include "types.h"
#include "tree_with_rms.h"
#include "log.h"

#include <functional>
#include <future>

App::App()
{
    server.SetOnClientConnectedCallback(std::bind(&App::OnClientConnected, this, std::placeholders::_1));
    server.SetOnClientDisconnectedCallback(std::bind(&App::OnClientDisconnected, this, std::placeholders::_1));
    server.SetOnReadCallback(std::bind(&App::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    server.SetOnWriteCallback(std::bind(&App::OnWrite, this, std::placeholders::_1, std::placeholders::_2));
}

void App::Run()
{
    //server.Init("127.0.0.1", 123);
    server.Init("", 123);
    server.StartAccept();

    std::async(std::launch::async, [&]
    {
        for (;;)
        {
            std::cout << tree.GetRms() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });

    for(;;)
    {
        try
        {
            server.Run();
        }
        catch (const std::exception& e)
        {
            LOG_ERROR(e.what());
        }
    }
}

void App::OnWrite(const Networking::Connection* conn, std::size_t bytes_transferred)
{
    LOG_DEBUG("Successfully written " << bytes_transferred << " bytes");
    server.AsyncRead(conn);
}

void App::OnRead(const Networking::Connection* conn, void* data, std::size_t size)
{
    unsigned value = *reinterpret_cast<unsigned*>(data);
    LOG_DEBUG("Got new data. Size: " << size << ", data: " << value);

    Networking::Types::ResultType result = tree.AddNewValueAndCalculateRms(value);
    server.AsyncWrite(conn, &result, sizeof(result));
}

void App::OnClientDisconnected(const Networking::Connection* /*conn*/)
{
    LOG_DEBUG("Client disconnected");
}

void App::OnClientConnected(const Networking::Connection* conn)
{
    LOG_DEBUG("New client connected");
    server.AsyncRead(conn);
}