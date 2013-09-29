#pragma once
#include <functional>

namespace Networking
{
    class Connection;

    namespace Callback
    {
        //! OnWrite callback called when data was successfully transferred via socket
        typedef std::function<void(const Connection*, std::size_t)> OnWrite;

        //! OnRead callback called when data was successfully read from socket
        typedef std::function<void(const Connection*, void*, std::size_t)> OnRead;

        //! OnClientConnected callback called when new client connected to listening socket
        typedef std::function<void(const Connection*)> OnConnected;

        //! OnClientDisconnected callback called when client closed connection
        typedef std::function<void(const Connection*)> OnDisconnected;
    }

    //! Holder for async callbacks. Need for abstraction between callbacks and the holder
    class CallbacksHolder
    {
    public:
        virtual ~CallbacksHolder() {}

        Callback::OnWrite OnWrite;
        Callback::OnRead OnRead;
        Callback::OnConnected OnConnected;
        Callback::OnDisconnected OnDisconnected;
    };

    //! Sets callbacks to the callbacks holder
    class CallbacksExecutor
    {
    public:
        CallbacksExecutor(CallbacksHolder& callbacks_holder);
        virtual ~CallbacksExecutor() {}

    public:
        void SetOnWriteCallback(Callback::OnWrite callback);
        void SetOnReadCallback(Callback::OnRead callback);
        void SetOnConnectedCallback(Callback::OnConnected callback);
        void SetOnDisconnectedCallback(Callback::OnDisconnected callback);
    
    private:
        CallbacksHolder& _callbacks_holder;
    };

} // namespace Networking