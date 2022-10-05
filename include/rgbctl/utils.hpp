#ifndef RGBCTL_UTILS_HPP_INCLUDED
#define RGBCTL_UTILS_HPP_INCLUDED

#include "./rgbctl.h"
#include <stdexcept>

#define RGBCTL_STRINGIFY_IMPL(x)  #x
#define RGBCTL_STRINGIFY(x)       RGBCTL_STRINGIFY_IMPL(x)
#define RGBCTL_COMBINE_IMPL(x, y) x##y
#define RGBCTL_COMBINE(x, y)      RGBCTL_COMBINE_IMPL(x, y)
#define RGBCTL_UNUSED()           RGBCTL_COMBINE(unused_, __LINE__)

namespace rgbctl
{

struct AssertionError : std::logic_error
{
    AssertionError(std::string const&);
};

} // namespace rgbctl

auto operator<(rgbctl_product_id const&, rgbctl_product_id const&) noexcept
    -> bool;

auto operator<=(rgbctl_product_id const&, rgbctl_product_id const&) noexcept
    -> bool;

auto operator==(rgbctl_product_id const&, rgbctl_product_id const&) noexcept
    -> bool;

auto operator!=(rgbctl_product_id const&, rgbctl_product_id const&) noexcept
    -> bool;

auto operator>(rgbctl_product_id const&, rgbctl_product_id const&) noexcept
    -> bool;

auto operator>=(rgbctl_product_id const&, rgbctl_product_id const&) noexcept
    -> bool;

#endif // RGBCTL_UTILS_HPP_INCLUDED
