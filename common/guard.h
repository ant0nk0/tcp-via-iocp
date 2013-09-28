#pragma once

#include <functional>

//! Resource guard based on RAII
class Guard
{
public:
    typedef std::function<void()> ScopeExitFunc;

    Guard(ScopeExitFunc func) 
        : _remover(func)
    {
    }

    ~Guard()
    {
        if (_remover)
            _remover();
    }

    void Release()
    {
        _remover = nullptr;
    }

private:
    ScopeExitFunc _remover;
};