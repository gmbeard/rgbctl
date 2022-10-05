#ifndef RGBCTL_TEXTURE_HPP_INCLUDED
#define RGBCTL_TEXTURE_HPP_INCLUDED

#include "./rgb.hpp"
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

constexpr struct NearestFiltering
{
} texture_filtering_nearest;

constexpr struct LinearFiltering
{
} texture_filtering_linear;

struct Texture
{
    static std::size_t constexpr one_row = static_cast<std::size_t>(-1);

    Texture(std::span<RgbFloat const> texels, std::size_t width = one_row);

    auto width() const noexcept -> std::size_t;
    auto height() const noexcept -> std::size_t;

    auto sample(Vec<float, 2> const&, Filtering) const noexcept -> RgbFloat;
    auto sample(Vec<float, 2> const&, NearestFiltering) const noexcept
        -> RgbFloat;
    auto sample(Vec<float, 2> const&, LinearFiltering) const noexcept
        -> RgbFloat;

private:
    std::vector<RgbFloat> texels_;
    std::size_t width_;
};

} // namespace rgbctl

#endif // RGBCTL_TEXTURE_HPP_INCLUDED
