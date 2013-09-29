#pragma once

namespace Utils
{

    class NonCopyable
    {
    public:
        NonCopyable() {}
        ~NonCopyable() {}
    private:
        NonCopyable(const NonCopyable&);
        void operator=(const NonCopyable&);
    };

} // namespace Utils