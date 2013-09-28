#include "connection.h"

Connection::Connection( const SOCKET& socket ) : _socket(socket)
	, _accept_overlapped(CreateOverlapped(Overlapped::Accept))
	, _read_overlapped(CreateOverlapped(Overlapped::Read))
	, _write_overlapped(CreateOverlapped(Overlapped::Write))
	, sent_bytes()
	, total_bytes()
	, write_buffer(nullptr)
	, write_buffer_size()
{
	_accept_overlapped->connection = this;
	_read_overlapped->connection = this;
	_write_overlapped->connection = this;
}

Connection::~Connection()
{
	if (_socket)
		closesocket(_socket);
}
