#include "rgbctl/utils.hpp"

namespace rgbctl
{

AssertionError::AssertionError(std::string const& msg)
    : std::logic_error { msg }
{ }

} // namespace rgbctl

auto operator<(rgbctl_product_id const& lhs,
               rgbctl_product_id const& rhs) noexcept -> bool
{
    return lhs.vendor_id < rhs.vendor_id
           || (lhs.vendor_id == rhs.vendor_id
               && lhs.product_id < rhs.product_id);
}

auto operator==(rgbctl_product_id const& lhs,
                rgbctl_product_id const& rhs) noexcept -> bool
{
    return lhs.vendor_id == rhs.vendor_id && lhs.product_id == rhs.product_id;
}

auto operator!=(rgbctl_product_id const& lhs,
                rgbctl_product_id const& rhs) noexcept -> bool
{
    return !(lhs == rhs);
}

auto operator<=(rgbctl_product_id const& lhs,
                rgbctl_product_id const& rhs) noexcept -> bool
{
    return lhs < rhs || lhs == rhs;
}

auto operator>(rgbctl_product_id const& lhs,
               rgbctl_product_id const& rhs) noexcept -> bool
{
    return !(lhs <= rhs);
}

auto operator>=(rgbctl_product_id const& lhs,
                rgbctl_product_id const& rhs) noexcept -> bool
{
    return lhs == rhs || lhs > rhs;
}
