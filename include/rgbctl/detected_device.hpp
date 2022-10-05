#ifndef RGBCTL_DETECTED_DEVICE_HPP_INCLUDED
#define RGBCTL_DETECTED_DEVICE_HPP_INCLUDED

#include "./rgbctl.h"
#include <string>

namespace rgbctl
{

struct DetectedDevice
{
    rgbctl_product_id product_id;
    std::string device_path;
};

auto operator<(DetectedDevice const&, DetectedDevice const& rhs) noexcept
    -> bool;
auto operator==(DetectedDevice const&, DetectedDevice const& rhs) noexcept
    -> bool;
auto operator!=(DetectedDevice const&, DetectedDevice const& rhs) noexcept
    -> bool;

} // namespace rgbctl

#endif // RGBCTL_DETECTED_DEVICE_HPP_INCLUDED
