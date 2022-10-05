#ifndef RGBCTL_FIXED_CONFIG_HPP_INCLUDED
#define RGBCTL_FIXED_CONFIG_HPP_INCLUDED

#include "rgbctl/rgb.hpp"

#include <array>
#include <cinttypes>
#include <cstddef>

namespace rgbctl
{

template <std::size_t N>
struct TextureConfig
{
    std::size_t width;
    std::array<RgbFloat, N> data;
};

auto constexpr fixed_texture_config()
{
    auto constexpr texture_data
        = std::to_array<RgbFloat>({ hex_string_to_rgb_float("000000"),
                                    hex_string_to_rgb_float("000000"),
                                    hex_string_to_rgb_float("070050"),
                                    hex_string_to_rgb_float("3f00ff") });

    return TextureConfig<texture_data.size()> { .width = 4,
                                                .data = { texture_data } };
}

} // namespace rgbctl

#endif // RGBCTL_FIXED_CONFIG_HPP_INCLUDED
