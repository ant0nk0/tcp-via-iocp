#include "server.h"
#include "overlapped.h"
#include <WinSock2.h>
#include <Windows.h>


Server::Server()
    : _acceptex_func(nullptr)
    , _connections_count()
{
    _wsa_inited = !WSAStartup(MAKEWORD(2, 2), &_wsa_data);
}

Server::~Server()
{
    if (_wsa_inited)
        WSACleanup();
}

void Server::Init(const std::string& address, unsigned port)
{
    // Check WSA version
    WSA_CHECK
    (
        LOBYTE(_wsa_data.wVersion) == 2 && HIBYTE(_wsa_data.wVersion) == 2,
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
        !!CreateIoCompletionPort((HANDLE) _socket.GetSocket(), _completion_port, (u_long) 0, 0),
        "Failed to associate listening socket with the IO Completion port"
    );

    //  Place a socket in a state in which it is listening for an incoming connection.
    WSA_CHECK
    (
        !listen(_socket.GetSocket(), 1), 
        "Failed to switch socket to listening mode"
    );

    GUID acceptex_guid = WSAID_ACCEPTEX;
    DWORD bytes_returned;

    // Get AcceptEx function address
    WSA_CHECK
    (
        !WSAIoctl
        (
            _socket.GetSocket(), SIO_GET_EXTENSION_FUNCTION_POINTER,
            &acceptex_guid, sizeof(acceptex_guid),
            &_acceptex_func, sizeof(_acceptex_func),
            &bytes_returned, NULL, NULL
        ),
        "Failed to get AcceptEx function address"
    );
}

void Server::StartAccept()
{
    CheckInited();

    SOCKET accepted_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    WSA_CHECK(accepted_socket != INVALID_SOCKET, "Failed to initialize the socket for connection accept");

    std::unique_ptr<Connection> new_connection(new Connection(accepted_socket));
    ++_connections_count;

    DWORD dwBytes = 0;
    const int accept_ex_result = _acceptex_func
    (
        _socket.GetSocket(),
        accepted_socket,
        new_connection->GetReadBuffer(),
        0,
        sizeof (sockaddr_in) + 16, sizeof (sockaddr_in) + 16,
        &dwBytes, reinterpret_cast<LPOVERLAPPED>(new_connection->GetAcceptOverlapped())
    );

    WSA_CHECK(accept_ex_result == TRUE || WSAGetLastError() == WSA_IO_PENDING, "Failed to call AcceptEx");

    // Associate the accept socket with the completion port
    CreateIoCompletionPort((HANDLE)accepted_socket, _completion_port, (u_long) 0, 0);

    // free connection's ownership
    new_connection.release();
}


void Server::Run()
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

        if (overlapped->type == Overlapped::Type::Accept)
        {
            // accept new connection
            StartAccept();

            if (OnClientConnected)
                OnClientConnected(overlapped->connection);

            continue;
        }

        if (bytes_transferred == 0)
        {
            if (OnClientDisconnected)
                OnClientDisconnected(overlapped->connection);

            delete overlapped->connection;
            overlapped = nullptr;

            --_connections_count;

            continue;
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

void Server::ReadAsync(const Connection* conn)
{
    CheckInited();

    Overlapped* overlapped = conn->GetReadOverlapped();
    // read next
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

void Server::WriteAsync(const Connection* conn, void* data, std::size_t size)
{
    CheckInited();

    Connection* mutable_conn = const_cast<Connection*>(conn);

    if (mutable_conn->GetWriteBufferSize() < size)
        mutable_conn->ResizeWriteBuffer(size);

    memcpy(mutable_conn->GetWriteBuffer(), data, size);

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

void Server::SetOnClientDisconnectedCallback(OnClientDisconnectedCallback callback)
{
    OnClientDisconnected = callback;
}

void Server::SetOnClientConnectedCallback(OnClientConnectedCallback callback)
{
    OnClientConnected = callback;
}

void Server::SetOnReadCallback(OnReadCallback callback)
{
    OnRead = callback;
}

void Server::SetOnWriteCallback(OnWriteCallback callback)
{
    OnWrite = callback;
}

void Server::CheckInited() const
{
    CHECK(!!_acceptex_func, "Server must be initialized before start");
}

