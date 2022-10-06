#ifndef RGBCTL_TEXTURE_HPP_INCLUDED
#define RGBCTL_TEXTURE_HPP_INCLUDED

#include "./rgb.hpp"
#include "./texture.h"
#include <cstddef>
#include <span>
#include <vector>

namespace rgbctl
{

enum class Filtering
{
    Nearest,
    Linear
};

[[maybe_unused]] constexpr struct NearestFiltering
{
} texture_filtering_nearest;

[[maybe_unused]] constexpr struct LinearFiltering
{
} texture_filtering_linear;

} // namespace rgbctl

struct rgbctl_texture
{
    static std::size_t constexpr one_row = static_cast<std::size_t>(-1);

    rgbctl_texture(std::span<rgbctl::RgbFloat const> texels,
                   std::size_t width = one_row);

    auto width() const noexcept -> std::size_t;
    auto height() const noexcept -> std::size_t;

    auto sample(rgbctl::Vec<float, 2> const&, rgbctl::Filtering) const noexcept
        -> rgbctl::RgbFloat;
    auto sample(rgbctl::Vec<float, 2> const&,
                rgbctl::NearestFiltering) const noexcept -> rgbctl::RgbFloat;
    auto sample(rgbctl::Vec<float, 2> const&,
                rgbctl::LinearFiltering) const noexcept -> rgbctl::RgbFloat;

private:
    std::vector<rgbctl::RgbFloat> texels_;
    std::size_t width_;
};

namespace rgbctl
{
using Texture = rgbctl_texture;
}

#endif // RGBCTL_TEXTURE_HPP_INCLUDED
