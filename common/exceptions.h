#pragma once

#include <exception>
#include <Winsock2.h>

namespace Networking
{

//! Exception containing Error message and WSA Error if some operation fails
class wsa_exception : public std::exception
{
    char _message[1024];

    const char* get_message(const char* prefix = "")
    {
        wsprintfA(_message, "WinSock Error: %s. Error code: %d", prefix, WSAGetLastError());
        return _message;
    }

public:
    wsa_exception() : std::exception(get_message()) { }
    wsa_exception(const char* prefix) : std::exception(get_message(prefix)) { }
};

//! Plain exception, for non-winsock operations 
class check_exception : public std::exception
{
public:
    check_exception(const char* message) : std::exception(message) {}
};

inline void WSA_CHECK(bool condition, const char* message)
{
    if (!condition)
        throw wsa_exception(message);
}

inline void CHECK(bool condition, const char* message)
{
    if (!condition)
        throw check_exception(message);
}

} // namespace Networking
