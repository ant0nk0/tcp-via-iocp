#include "acceptor.h"
#include "context.h"

namespace Networking
{

    Acceptor::Acceptor(Context& context) 
        : _context(context)
        , _acceptex_func(nullptr)
    {
    }

    void Acceptor::Init()
    {
        // bind Socket to address and port
        _context.GetSocket().Bind();

        //  Place a socket in a state in which it is listening for an incoming connection.
        WSA_CHECK
        (
            !listen(_context.GetSocket().Native(), 1), 
            "Failed to switch socket to listening mode"
        );

        GUID acceptex_guid = WSAID_ACCEPTEX;
        DWORD bytes_returned;

        // Get AcceptEx function address
        WSA_CHECK
        (
            !WSAIoctl
            (
                _context.GetSocket().Native(), SIO_GET_EXTENSION_FUNCTION_POINTER,
                &acceptex_guid, sizeof(acceptex_guid),
                &_acceptex_func, sizeof(_acceptex_func),
                &bytes_returned, NULL, NULL
            ),
            "Failed to get AcceptEx function address"
        );

        _context._acceptor = this;
    }

    void Acceptor::Start()
    {
        SOCKET accepted_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        WSA_CHECK(accepted_socket != INVALID_SOCKET, "Failed to initialize the socket for connection accept");

        std::unique_ptr<Connection> new_connection(new Connection(accepted_socket));

        DWORD bytes = 0;
        const int accept_ex_result = _acceptex_func
        (
            _context.GetSocket().Native(),
            accepted_socket,
            new_connection->GetReadBuffer(),
            0,
            sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
            &bytes, reinterpret_cast<LPOVERLAPPED>(new_connection->GetAcceptOverlapped())
        );

        WSA_CHECK(accept_ex_result == TRUE || WSAGetLastError() == WSA_IO_PENDING, "Failed to call AcceptEx");

        // Associate the accept socket with the completion port
        CreateIoCompletionPort(reinterpret_cast<HANDLE>(accepted_socket), _context.GetCompletionPort(), 0, 0);

        // free connection's ownership
        new_connection.release();
    }

} // namespace Networking