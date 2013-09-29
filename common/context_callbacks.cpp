#include "context_callbacks.h"

namespace Networking
{

CallbacksExecutor::CallbacksExecutor(CallbacksHolder& callbacks_holder)
    : _callbacks_holder(callbacks_holder)
{
}

void CallbacksExecutor::SetOnWriteCallback(Callback::OnWrite callback)
{
    _callbacks_holder.OnWrite = callback;
}

void CallbacksExecutor::SetOnReadCallback(Callback::OnRead callback)
{
    _callbacks_holder.OnRead = callback;
}

void CallbacksExecutor::SetOnConnectedCallback(Callback::OnConnected callback)
{
    _callbacks_holder.OnConnected = callback;
}

void CallbacksExecutor::SetOnDisconnectedCallback(Callback::OnDisconnected callback)
{
    _callbacks_holder.OnDisconnected = callback;
}

} // namespace Networking