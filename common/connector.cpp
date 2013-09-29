#include "connector.h"
#include "context.h"

namespace Networking
{

Connector::Connector(Context& context) 
    : _context(context)
{
}

void Connector::Init()
{
    _context.GetSocket().BindBeforeConnect();

    GUID connectex_guid = WSAID_CONNECTEX;
    DWORD bytes_returned;

    // Get ConnectEx function address
    WSA_CHECK
    (
        !WSAIoctl
        (
            _context.GetSocket().Native(), SIO_GET_EXTENSION_FUNCTION_POINTER,
            &connectex_guid, sizeof(connectex_guid),
            &_connectex_func, sizeof(_connectex_func),
            &bytes_returned, NULL, NULL
        ),
        "Failed to get AcceptEx function address"
    );
}
    
void Connector::Start()
{
    std::unique_ptr<Connection> new_connection(new Connection(_context.GetSocket().Native()));
    auto socket_address = _context.GetSocket().GetAddress();

    DWORD bytes = 0;
    const int connect_ex_result = _connectex_func
    (
        _context.GetSocket().Native(),
        reinterpret_cast<SOCKADDR*>(&socket_address),
        sizeof(socket_address),
        NULL,
        0,
        &bytes, 
        reinterpret_cast<LPOVERLAPPED>(new_connection->GetConnectOverlapped())
    );

    WSA_CHECK(connect_ex_result == TRUE || WSAGetLastError() == WSA_IO_PENDING, "Failed to call ConnectEx");

    // free connection's ownership
    new_connection.release();
}

} // namespace Networking