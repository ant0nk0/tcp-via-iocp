#include <functional>

#include "server.h"
#include "exceptions.h"
#include "connection.h"

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

    void OnClientConnected(const Connection* conn)
    {
        LOG_DEBUG("New client connected");
        server.ReadAsync(conn);
    }

    void OnClientDisconnected(const Connection* /*conn*/)
    {
        LOG_DEBUG("Client disconnected");
    }

    void OnRead(const Connection* conn, void* data, std::size_t size)
    {
        LOG_DEBUG("Got new data. Size: " << size << ", data: " << static_cast<const char*>(data));
        server.WriteAsync(conn, "-", 1);
    }

    void OnWrite(const Connection* conn, std::size_t bytes_transferred)
    {
        LOG_DEBUG("Successfully written " << bytes_transferred << " bytes");
        server.ReadAsync(conn);
    }

    Server server;
};


int main(int argc, char* argv[])
{
    try
    {
        App app;
        app.Run();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(e.what());
    }
  
    return 0;
}