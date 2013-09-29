#pragma once

#include "noncopyable.h"
#include <WinSock2.h>
#include <mswsock.h>

namespace Networking
{

class Context;

//! Extending 'Context' and helps to start async Connect operation
//! (used on client side)
class Connector : Utils::NonCopyable
{
public:
    Connector(Context& context);

    //! Initialize connector
    void Init();

    //! Start async Connect operation
    void Start();

private:
    // non copyable
    Connector(const Connector&);
    void operator=(const Connector&);

private:
    //! Main network context
    Context& _context;

    //! ConnectEx function pointer
    LPFN_CONNECTEX _connectex_func;
};

} // namespace Networking