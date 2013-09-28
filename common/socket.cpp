#include "socket.h"

Socket::Socket() 
	: _inited(false)
	, _socket()
{
}

Socket::~Socket()
{
	if (_inited)
		WSACleanup();
}

void Socket::Init( const std::string& address, unsigned port )
{
	if (_inited)
		return;

	_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	WSA_CHECK(_socket != INVALID_SOCKET, "Failed to initialize socket");

	Guard socket_guard([&] { closesocket(_socket); });

	unsigned long resolved_address = ResolveAddress(address);
	WSA_CHECK(resolved_address != INADDR_NONE, "Failed to resolve address: " + address);

	sockaddr_in socket_address;
	memset(&socket_address, 0, sizeof(socket_address));
	socket_address.sin_family = AF_INET;
	socket_address.sin_addr.s_addr = resolved_address;
	socket_address.sin_port = htons(port);

	WSA_CHECK
	(
		!bind(_socket, reinterpret_cast<SOCKADDR*>(&socket_address), sizeof(socket_address)),
		"Failed to bind socket to address " + address + " and specified port"
	);

	socket_guard.Release();
	_inited = true;
}

unsigned long Socket::ResolveAddress( const std::string& address )
{
	if (address.empty())
		return INADDR_ANY;

	hostent* host = gethostbyname(address.c_str());
	return inet_addr(inet_ntoa(*reinterpret_cast<struct in_addr *>(*host->h_addr_list)));
}

SOCKET& Socket::GetSocket()
{
	return _socket;
}
