#pragma once

#include <memory>
#include <WinSock2.h>

namespace Networking
{

// forward declaration
struct Overlapped;

class Connection
{
public:

    Connection(const SOCKET& socket);
    ~Connection();

public:

	static const std::size_t ReadBufferSize = 1024;

	SOCKET& GetSocket();

	void* GetReadBuffer();
	void* GetWriteBuffer();

	std::size_t GetWriteBufferSize() const;
	void ResizeWriteBuffer(std::size_t new_size);

	std::size_t GetSentBytes() const;
	void SetSentBytes(std::size_t value);

	std::size_t GetTotalBytes() const;
	void SetTotalBytes(std::size_t value);

	Overlapped* GetAcceptOverlapped() const;
	Overlapped* GetReadOverlapped() const;
	Overlapped* GetWriteOverlapped() const;

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

} // namespace Networking