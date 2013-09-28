#include "connection.h"
#include <WinSock2.h>
#include "overlapped.h"

// Connection's pimpl class declaration
class Connection::Impl
{
public:
	Impl(const SOCKET& socket, Connection* owner);
	~Impl();

public:
	SOCKET _socket;
	char _read_buffer[ReadBufferSize];

	std::size_t _write_buffer_size;
	std::unique_ptr<char> _write_buffer;

	std::size_t _sent_bytes;
	std::size_t _total_bytes;

	std::unique_ptr<Overlapped> _accept_overlapped;
	std::unique_ptr<Overlapped> _read_overlapped;
	std::unique_ptr<Overlapped> _write_overlapped;
};

Connection::Impl::Impl(const SOCKET& socket, Connection* owner) 
	: _socket(socket)
	, _accept_overlapped(CreateOverlapped(Overlapped::Accept))
	, _read_overlapped(CreateOverlapped(Overlapped::Read))
	, _write_overlapped(CreateOverlapped(Overlapped::Write))
	, _sent_bytes()
	, _total_bytes()
	, _write_buffer(nullptr)
	, _write_buffer_size()
{
	_accept_overlapped->connection = owner;
	_read_overlapped->connection = owner;
	_write_overlapped->connection = owner;
}

Connection::Impl::~Impl()
{
	if (_socket)
		closesocket(_socket);
}

Connection::Connection(const SOCKET& socket)
	: _impl(new Impl(socket, this))
{
}

Connection::~Connection()
{
}

Overlapped* Connection::GetWriteOverlapped() const
{
	return _impl->_write_overlapped.get();
}

Overlapped* Connection::GetReadOverlapped() const
{
	return _impl->_read_overlapped.get();
}

Overlapped* Connection::GetAcceptOverlapped() const
{
	return _impl->_accept_overlapped.get();
}

void Connection::SetTotalBytes(std::size_t value)
{
	_impl->_total_bytes = value;
}

std::size_t Connection::GetTotalBytes() const
{
	return _impl->_total_bytes;
}

void Connection::SetSentBytes(std::size_t value)
{
	_impl->_sent_bytes = value;
}

std::size_t Connection::GetSentBytes() const
{
	return _impl->_sent_bytes;
}

void Connection::ResizeWriteBuffer(std::size_t new_size)
{
	_impl->_write_buffer.reset(new char[new_size]);
	_impl->_write_buffer_size = new_size;
}

std::size_t Connection::GetWriteBufferSize() const
{
	return _impl->_write_buffer_size;
}

void* Connection::GetWriteBuffer()
{
	return _impl->_write_buffer.get();
}

void* Connection::GetReadBuffer()
{
	return &_impl->_read_buffer;
}

SOCKET& Connection::GetSocket()
{
	return _impl->_socket;
}
