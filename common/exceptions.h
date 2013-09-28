#pragma once

#include <exception>
#include <string>
#include <Winsock2.h>

class wsa_exception : public std::exception
{
    std::string _message;

    std::string get_message(const std::string& prefix = "")
    {
        std::stringstream ss;
        ss << "WinSock error: " << prefix << ". Code: " << WSAGetLastError();
        return ss.str();
    }

public:
    wsa_exception() : std::exception(get_message().c_str()) { }
    wsa_exception(const std::string& prefix) : std::exception(get_message(prefix).c_str()) { }
};

class check_exception : public std::exception
{
public:
    check_exception(const std::string& message) : std::exception(message.c_str()) {}
};

inline void WSA_CHECK(bool condition, const std::string& message)
{
    if (!condition)
        throw wsa_exception(message);
}

inline void CHECK(bool condition, const std::string& message)
{
    if (!condition)
        throw check_exception(message);
}
