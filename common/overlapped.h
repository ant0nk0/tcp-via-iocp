#pragma once

#include <WinSock2.h>

namespace Networking
{

class Connection;

//! Overlapped structure, extended with helper fields
struct Overlapped
{
    enum Type
    {
        Connect,
        Accept,
        Read,
        Write,
    };

    //! Native overlapped
    WSAOVERLAPPED overlapped;

    //! Type of overlapped. Context used this type to resolve operation
    Type type;

    //! Connection associated with current async operation
    Connection* connection;

    //! Current winsock buffers
    WSABUF wsa_buf;
};

//! Create and prepare custom overlapped
inline Overlapped* CreateOverlapped(Overlapped::Type type)
{
    Overlapped* overlapped = new Overlapped;
    memset(overlapped, 0, sizeof(Overlapped));
    overlapped->type = type;
    return overlapped;
}

} // namespace Networking