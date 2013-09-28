#pragma once

#include "client.h"
#include "exceptions.h"
#include "connection.h"
#include "types.h"

#include <functional>
#include <iostream>

#define LOG_DEBUG(message) std::cout << "Debug: " << message << std::endl;
#define LOG_ERROR(message) std::cout << "Error: " << message << std::endl;

class App
{
public:

    App()
    {
        client.SetOnConnectedCallback(std::bind(&App::OnConnected, this, std::placeholders::_1));
        client.SetOnDisconnectedCallback(std::bind(&App::OnDisconnected, this, std::placeholders::_1));
        client.SetOnReadCallback(std::bind(&App::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        client.SetOnWriteCallback(std::bind(&App::OnWrite, this, std::placeholders::_1, std::placeholders::_2));
    }

    void Run()
    {
        client.Init("127.0.0.1", 123);
        client.Run();
    }

    void OnConnected(const Networking::Connection* /*conn*/)
    {
        LOG_DEBUG("Connected to server");

        SendNextValue();
    }

    void OnDisconnected(const Networking::Connection* /*conn*/)
    {
        LOG_DEBUG("Client disconnected");
    }

    void OnRead(const Networking::Connection* conn, void* data, std::size_t size)
    {
        if (size != sizeof(Networking::Types::ResultType))
            return;

        auto value = *reinterpret_cast<Networking::Types::ResultType*>(data);
        LOG_DEBUG("Got new data. Size: " << size << ", data: " << value);

        SendNextValue();
    }

    void SendNextValue()
    {
        auto next = Generate();
        client.WriteAsync(&next, sizeof(next));
    }

    Networking::Types::InputType Generate()
    {
        return rand() % 1024;
    }

    void OnWrite(const Networking::Connection* conn, std::size_t bytes_transferred)
    {
        LOG_DEBUG("Successfully written " << bytes_transferred << " bytes");
        client.ReadAsync();
    }

    Networking::Client client;
};
