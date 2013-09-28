#pragma once

#include <WinSock2.h>

namespace Networking
{

// forward declarations
class Connection;

//! Overlapped structure, extended with helper fields
struct Overlapped
{
    enum Type
    {
        Accept,
        Read,
        Write,
    };

    WSAOVERLAPPED overlapped;
    Type type;
    Connection* connection;
    WSABUF wsa_buf;
};

inline Overlapped* CreateOverlapped(Overlapped::Type type)
{
    Overlapped* overlapped = new Overlapped;
    memset(overlapped, 0, sizeof(Overlapped));
    overlapped->type = type;
    return overlapped;
}

} // namespace Networking