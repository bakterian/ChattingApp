#pragma once

namespace utils
{
    class NonCopyableMovable
    {
    public:
        NonCopyableMovable(const NonCopyableMovable&) = delete;
        NonCopyableMovable& operator = (const NonCopyableMovable&) = delete;
        NonCopyableMovable(NonCopyableMovable&&) = delete;
        NonCopyableMovable& operator = (const NonCopyableMovable&&) = delete;

    protected:
        NonCopyableMovable() = default;
        ~NonCopyableMovable() = default; /// Protected non-virtual destructor
    };
}