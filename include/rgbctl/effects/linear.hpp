#ifndef RGBCTL_EFFECTS_LINEAR_HPP_INCLUDED
#define RGBCTL_EFFECTS_LINEAR_HPP_INCLUDED

#include "../rgb.hpp"
#include "../texture.hpp"
#include <cinttypes>
#include <cstddef>
#include <span>
#include <vector>

namespace rgbctl::effects
{

struct Linear
{
    explicit Linear(std::uint32_t zone_index,
                    std::size_t rgb_count,
                    std::size_t duration_ms,
                    std::span<RgbFloat const> data);

    auto rgb_count() const noexcept -> std::size_t;

    auto zone_index() const noexcept -> std::uint32_t;

    auto duration() const noexcept -> std::size_t;

    auto remaining() const noexcept -> std::size_t;

    auto tick(std::size_t ms, std::span<rgbctl_rgb_value> out_frame)
        -> std::size_t;

private:
    std::size_t elapsed_ms_;
    std::uint32_t zone_index_;
    std::size_t duration_ms_;
    Texture texture_;
};

} // namespace rgbctl::effects

#endif // RGBCTL_EFFECTS_LINEAR_HPP_INCLUDED
