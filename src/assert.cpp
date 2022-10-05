#include "rgbctl/assert.hpp"
#include <exception>
#ifndef NDEBUG
    #include <cstdio>
#endif

namespace rgbctl
{

[[noreturn]] auto condition_failure(char const* condition,
                                    char const* type) noexcept -> void
{
#ifndef NDEBUG
    std::fprintf(stderr, "ASSERT: %s failed - %s\n", type, condition);
#endif
    std::terminate();
}

} // namespace rgbctl
