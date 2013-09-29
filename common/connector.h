#pragma once

#include <WinSock2.h>
#include <mswsock.h>

namespace Networking
{

class Context;

//! Extending 'Context' and helps to start async Connect operation
//! (used on client side)
class Connector
{
public:
    Connector(Context& context);

    //! Initialize connector
    void Init();

    //! Start async Connect operation
    void Start();

private:
    //! Main network context
    Context& _context;

    //! ConnectEx function pointer
    LPFN_CONNECTEX _connectex_func;
};

} // namespace Networking