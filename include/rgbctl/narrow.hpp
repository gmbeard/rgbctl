#ifndef RGBCTL_NARROW_HPP_INCLUDED
#define RGBCTL_NARROW_HPP_INCLUDED

#include "./assert.hpp"
#include <limits>
#include <type_traits>

namespace rgbctl
{

template <typename To, typename From>
constexpr auto narrow_cast(From val) noexcept -> To
    requires(std::is_arithmetic_v<To>)
{
    To result = static_cast<To>(val);
    if constexpr (std::is_signed_v<To> == std::is_signed_v<From>) {
        RGBCTL_ENSURES(static_cast<From>(result) == val);
    }
    else {
        RGBCTL_ENSURES(static_cast<From>(result) == val
                       && (result < To {}) == (val < From {}));
    }

    return result;
}

template <typename To, typename From>
constexpr auto can_narrow(From const& val) noexcept -> bool
{
    return val <= std::numeric_limits<To>::max();
}

} // namespace rgbctl

#endif // RGBCTL_NARROW_HPP_INCLUDED
