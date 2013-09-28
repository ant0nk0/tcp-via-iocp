#include "application.h"
#include "client.h"
#include "exceptions.h"
#include "connection.h"
#include "types.h"
#include "log.h"

#include <functional>

App::App()
{
    client.SetOnConnectedCallback(std::bind(&App::OnConnected, this, std::placeholders::_1));
    client.SetOnDisconnectedCallback(std::bind(&App::OnDisconnected, this, std::placeholders::_1));
    client.SetOnReadCallback(std::bind(&App::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    client.SetOnWriteCallback(std::bind(&App::OnWrite, this, std::placeholders::_1, std::placeholders::_2));
}

void App::Run(const char* address, unsigned port)
{
    client.Init(address, port);
    client.Run();
}

void App::OnConnected(const Networking::Connection* /*conn*/)
{
    LOG_DEBUG("Connected to server");
    SendNextValue();
}

void App::OnDisconnected(const Networking::Connection* /*conn*/)
{
    LOG_DEBUG("Client disconnected");
}

void App::OnRead(const Networking::Connection* conn, void* data, std::size_t size)
{
//    if (size != sizeof(Networking::Types::ResultType))
//        return;

    auto value = *reinterpret_cast<Networking::Types::ResultType*>(data);
    LOG_DEBUG("Got new data. Size: " << size << ", data: " << value);

    SendNextValue();
}

void App::SendNextValue()
{
    Networking::Types::InputType next = rand() % 1024;
    client.AsyncWrite(&next, sizeof(next));
}

void App::OnWrite(const Networking::Connection* conn, std::size_t bytes_transferred)
{
    LOG_DEBUG("Successfully written " << bytes_transferred << " bytes");
    client.AsyncRead();
}
