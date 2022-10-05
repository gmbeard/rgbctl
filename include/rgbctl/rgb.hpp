#ifndef RGBCTL_RGB_HPP_INCLUDED
#define RGBCTL_RGB_HPP_INCLUDED

#include "./rgbctl.h"
#include "./vec.hpp"
#include <array>
#include <cinttypes>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace rgbctl
{

template <typename T>
using Rgb = Vec<T, 3>;

using RgbFloat = Rgb<float>;
using RgbUint8 = Rgb<std::uint8_t>;

template <typename T>
auto rgb(T const& r, T const& g, T const& b) noexcept -> Rgb<T>
{
    return to_vec<T>(r, g, b);
}

auto constexpr hex_string_to_rgb_uint8(std::string_view str) -> RgbUint8
{
    std::array<std::uint8_t, 3> buffer {};
    auto pos = buffer.begin();
    int n = 0;

    for (auto c : str) {
        if (pos == buffer.end())
            throw std::runtime_error { "Invalid hex: string too large" };

        if (c >= '0' && c <= '9') {
            *pos = static_cast<std::uint8_t>(*pos * 16 + c - '0');
        }
        else if (c >= 'a' && c <= 'f') {
            *pos = static_cast<std::uint8_t>(*pos * 16 + c - 'a' + 10);
        }
        else if (c >= 'A' && c <= 'F') {
            *pos = static_cast<std::uint8_t>(*pos * 16 + c - 'A' + 10);
        }
        else {
            throw std::runtime_error { "Invalid hex: character" };
        }

        if ((++n % 2) == 0)
            pos++;
    }

    if (pos != buffer.end())
        throw std::runtime_error { "Invalid hex: string too small" };

    return RgbUint8 { buffer[0], buffer[1], buffer[2] };
}

auto constexpr hex_string_to_rgb_float(std::string_view str) -> RgbFloat
{
    RgbUint8 tmp = hex_string_to_rgb_uint8(str);

    return RgbFloat { static_cast<float>(get<0>(tmp)) / 255.f,
                      static_cast<float>(get<1>(tmp)) / 255.f,
                      static_cast<float>(get<2>(tmp)) / 255.f };
}

auto hex_string_to_rgb_uint8(std::string_view, RgbUint8&) noexcept -> bool;
auto hex_string_to_rgb_float(std::string_view, RgbFloat&) noexcept -> bool;
auto to_rgb_uint8(RgbFloat const&) noexcept -> rgbctl_rgb_value;

} // namespace rgbctl

#endif // RGBCTL_RGB_HPP_INCLUDED
