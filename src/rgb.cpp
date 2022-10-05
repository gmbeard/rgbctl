#include "rgbctl/rgb.hpp"
#include <stdexcept>

namespace rgbctl
{

auto hex_string_to_rgb_uint8(std::string_view str, RgbUint8& out_val) noexcept
    -> bool
{
    try {
        out_val = hex_string_to_rgb_uint8(str);
        return true;
    }
    catch (...) {
        return false;
    }
}

auto hex_string_to_rgb_float(std::string_view str, RgbFloat& out_val) noexcept
    -> bool
{
    try {
        out_val = hex_string_to_rgb_float(str);
        return true;
    }
    catch (...) {
        return false;
    }
}

auto to_rgb_uint8(RgbFloat const& val) noexcept -> rgbctl_rgb_value
{
    return rgbctl_rgb_value { static_cast<std::uint8_t>(get<0>(val) * 255),
                              static_cast<std::uint8_t>(get<1>(val) * 255),
                              static_cast<std::uint8_t>(get<2>(val) * 255) };
}

} // namespace rgbctl
