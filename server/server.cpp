#include "server.h"
#include "overlapped.h"
#include "exceptions.h"
#include "connection.h"
#include "acceptor.h"
#include "context.h"

#include <WinSock2.h>
#include <Windows.h>

namespace Networking
{

Server::Server(Context& context)
    : CallbacksExecutor(context)
    , _context(context)
    , _acceptor(context)
{
}

Server::~Server()
{
}

void Server::Init(const char* address, unsigned port)
{
    _context.Init(address, port);
    _acceptor.Init();
}

void Server::StartAccept()
{
    _acceptor.Start();
}

void Server::Run()
{
    _context.MainLoop();
}

void Server::AsyncRead(const Connection* conn)
{
    _context.AsyncRead(conn);
}

void Server::AsyncWrite(const Connection* conn, void* data, std::size_t size)
{
    _context.AsyncWrite(conn, data, size);
}

} // namespace Networking