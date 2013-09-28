#pragma once

#include <memory>
#include <WinSock2.h>
#include "overlapped.h"

class Connection
{
public:

    Connection(const SOCKET& socket);

    ~Connection();

    SOCKET _socket;

    std::unique_ptr<Overlapped> _accept_overlapped;
    std::unique_ptr<Overlapped> _read_overlapped;
    std::unique_ptr<Overlapped> _write_overlapped;

    static const std::size_t read_buffer_size = 1024;
    char read_buffer[read_buffer_size];

    std::size_t write_buffer_size;
    std::unique_ptr<char> write_buffer;

    std::size_t sent_bytes;
    std::size_t total_bytes;
};