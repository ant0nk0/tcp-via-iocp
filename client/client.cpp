#include "client.h"
#include "overlapped.h"
#include "exceptions.h"
#include "connection.h"

#include <memory>
#include <WinSock2.h>
#include <Windows.h>

namespace Networking
{

Client::Client()
{
}

Client::~Client()
{
}

void Client::Init(const char* address, unsigned port)
{
    // Check WinSock initialized
    WSA_CHECK
    (
        _winsock.IsInitialized(),
        "Unsupported version of WinSock"
    );

    // Create a handle for the completion port
    _completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    WSA_CHECK(!!_completion_port, "Failed to create IO Completion port");

    // Init socket and bind em on address and port
    _socket.Init(address, port);

    // Associate the listening socket with the completion port
    WSA_CHECK
    (
        !!CreateIoCompletionPort((HANDLE) _socket.GetSocket(), _completion_port, 0, 0),
        "Failed to associate listening socket with the IO Completion port"
    );

    auto socket_address = _socket.GetAddress();

    //  Place a socket in a state in which it is listening for an incoming connection.
    WSA_CHECK
    (
        !WSAConnect(_socket.GetSocket(), (SOCKADDR*)(&socket_address), sizeof(socket_address), NULL, NULL, NULL, NULL),
        "Failed to connect to the server"
    );

    _connection.reset(new Connection(_socket.GetSocket()));

    if (OnConnected)
        OnConnected(_connection.get());
}

void Client::Run()
{
    CheckInited();

    DWORD bytes_transferred;
    ULONG_PTR completion_key;
    DWORD Flags = 0;
    Overlapped* overlapped = nullptr;

    while (GetQueuedCompletionStatus(_completion_port, &bytes_transferred, &completion_key, reinterpret_cast<LPOVERLAPPED*>(&overlapped), INFINITE))
    {
        if (!overlapped)
            continue;

        if (bytes_transferred == 0)
        {
            if (OnDisconnected)
                OnDisconnected(overlapped->connection);

            break;
        }

        if (overlapped->type == Overlapped::Type::Read)
        {
            if (OnRead)
                OnRead(overlapped->connection, overlapped->connection->GetReadBuffer(), bytes_transferred);

            continue;
        }

        if (overlapped->type == Overlapped::Type::Write)
        {
            overlapped->connection->SetSentBytes(overlapped->connection->GetSentBytes() + bytes_transferred);

            if (overlapped->connection->GetSentBytes() < overlapped->connection->GetTotalBytes())
            {
                // read next
                overlapped->wsa_buf.len = overlapped->connection->GetTotalBytes() - overlapped->connection->GetSentBytes();
                overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(overlapped->connection->GetWriteBuffer()) + overlapped->connection->GetSentBytes();

                if (!WSASend(overlapped->connection->GetSocket(), &overlapped->wsa_buf, 1, &bytes_transferred, 0, reinterpret_cast<LPWSAOVERLAPPED>(overlapped), NULL))
                {
                    // handle error
                    int t = 0;
                }
            }
            else
            {
                if (OnWrite)
                    OnWrite(overlapped->connection, bytes_transferred);
            }
        }
    }
}

void Client::AsyncRead()
{
    CheckInited();

    Overlapped* overlapped = _connection->GetReadOverlapped();
    overlapped->wsa_buf.len = overlapped->connection->ReadBufferSize;
    overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(overlapped->connection->GetReadBuffer());

    DWORD flags = 0;
    DWORD bytes_transferred = 0;

    auto recv_result = WSARecv(overlapped->connection->GetSocket(), &overlapped->wsa_buf, 1, &bytes_transferred, &flags, reinterpret_cast<LPWSAOVERLAPPED>(overlapped), NULL);
    CHECK
    (
        recv_result == NULL || (recv_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING),
        "Failed to receive data"
    );
}

void Client::AsyncWrite(void* data, std::size_t size)
{
    CheckInited();

    Connection* mutable_conn = _connection.get();

    if (mutable_conn->GetWriteBufferSize() < size)
        mutable_conn->ResizeWriteBuffer(size);

    memcpy_s(mutable_conn->GetWriteBuffer(), mutable_conn->GetWriteBufferSize(), data, size);

    mutable_conn->SetSentBytes(0);
    mutable_conn->SetTotalBytes(size);

    Overlapped* overlapped = mutable_conn->GetWriteOverlapped();
    overlapped->wsa_buf.len = size;
    overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(mutable_conn->GetWriteBuffer());

    DWORD bytes;
    auto send_result = WSASend(mutable_conn->GetSocket(), &overlapped->wsa_buf, 1, &bytes, 0, reinterpret_cast<LPWSAOVERLAPPED>(overlapped), NULL);

    CHECK
    (
        send_result == NULL || (send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING),
        "Failed to send data"
    );
}

void Client::SetOnDisconnectedCallback(OnDisconnectedCallback callback)
{
    OnDisconnected = callback;
}

void Client::SetOnConnectedCallback(OnConnectedCallback callback)
{
    OnConnected = callback;
}

void Client::SetOnReadCallback(OnReadCallback callback)
{
    OnRead = callback;
}

void Client::SetOnWriteCallback(OnWriteCallback callback)
{
    OnWrite = callback;
}

void Client::CheckInited() const
{
    CHECK(_connection, "Client must be initialized before start");
}

} // namespace Networking