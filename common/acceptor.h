#pragma once

#include <WinSock2.h>
#include <mswsock.h>

namespace Networking
{

class Context;

//! Extending 'Context' and helps to start async Accept operation
//! (used on server side)
class Acceptor
{
public:
    Acceptor(Context& context);

    //! Initialize acceptor
    void Init();

    //! Start new async Accept operation
    void Start();

private:
    //! Main networking context
    Context& _context;

    //! AcceptEx function pointer
    LPFN_ACCEPTEX _acceptex_func;
};

} // namespace Networking