#pragma once

#include <WinSock2.h>

//! This class helps initialize winsock
class WinSockInitializer
{
public:
    WinSockInitializer()
        : _version_hi(2), _version_low(2)
    {
        _wsa_inited = !WSAStartup(MAKEWORD(_version_hi, _version_low), &_wsa_data);
    }

    bool IsInitialized()
    {
        return _wsa_inited && LOBYTE(_wsa_data.wVersion) == _version_low && HIBYTE(_wsa_data.wVersion) == _version_hi;
    }

    ~WinSockInitializer()
    {
        if (_wsa_inited)
            WSACleanup();
    }

private:
    //! Flag of successfull WSA startup
    bool _wsa_inited;

    //! Hi version of WSA
    unsigned _version_hi;

    //! Low version of WSA
    unsigned _version_low;

    //! WSA init data
    WSADATA _wsa_data;
};