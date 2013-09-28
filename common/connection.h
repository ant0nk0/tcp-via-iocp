#pragma once

#include <memory>
#include <WinSock2.h>

namespace Networking
{

struct Overlapped;

//! Stores information about incoming connection, read/write buffers and system overlapped
class Connection
{
public:
    Connection(const SOCKET& socket);
    ~Connection();

public:
    //! Maximal read buffer size
    static const std::size_t ReadBufferSize = 1024;

    //! Returns socket associated with connection
    SOCKET& GetSocket();

    //! Returns read buffer ptr
    void* GetReadBuffer();

    //! Returns write buffer ptr
    void* GetWriteBuffer();

    //! Returns current size of the write buffer
    std::size_t GetWriteBufferSize() const;

    //! Resize write buffer to specified size
    void ResizeWriteBuffer(std::size_t new_size);

    //! Bytes were sent from last AsyncWrite call
    std::size_t GetSentBytes() const;

    //! Sets how many bytes were sent
    void SetSentBytes(std::size_t value);

    //! Gets how many bytes will be sent by last AsyncWrite call
    std::size_t GetTotalBytes() const;

    //! Sets how many bytes will be sent by last AsyncWrite call
    void SetTotalBytes(std::size_t value);

    // Gets system overlapped for connection accept request
    Overlapped* GetAcceptOverlapped() const;

    // Gets system overlapped for async read request
    Overlapped* GetReadOverlapped() const;

    // Gets system overlapped for async write request
    Overlapped* GetWriteOverlapped() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace Networking