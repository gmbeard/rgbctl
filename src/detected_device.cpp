#include "rgbctl/detected_device.hpp"
#include "rgbctl/utils.hpp"

namespace rgbctl
{

auto operator<(DetectedDevice const& lhs, DetectedDevice const& rhs) noexcept
    -> bool
{
    return lhs.product_id < rhs.product_id;
}

auto operator==(DetectedDevice const& lhs, DetectedDevice const& rhs) noexcept
    -> bool
{
    return lhs.product_id == rhs.product_id;
}

auto operator!=(DetectedDevice const& lhs, DetectedDevice const& rhs) noexcept
    -> bool
{
    return lhs.product_id != rhs.product_id;
}

} // namespace rgbctl
