#include "client.h"
#include "overlapped.h"
#include "exceptions.h"
#include "connection.h"

#include <WinSock2.h>
#include <Windows.h>

namespace Networking
{

Client::Client(Context& context)
    : CallbacksExecutor(context)
    , _context(context)
    , _connector(context)
{
}

Client::~Client()
{
}

void Client::Init(const char* address, unsigned port)
{
    // Init winsock/iocp context
    _context.Init(address, port);
    
    // Init all need for connect
    _connector.Init();
}

void Client::Run()
{
    // Start async connect operation
    _connector.Start();

    // Process async operations
    _context.MainLoop();
}

void Client::AsyncRead(const Connection* conn)
{
    // Start async read operation for conn
    _context.AsyncRead(conn);
}

void Client::AsyncWrite(const Connection* conn, void* data, std::size_t size)
{
    // Start async write operation for conn
    _context.AsyncWrite(conn, data, size);
}

} // namespace Networking