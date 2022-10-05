#include "rgbctl/effects/linear.hpp"

#include <cassert>
#include <cinttypes>
#include <cmath>

namespace rgbctl::effects
{

Linear::Linear(std::uint32_t zone_index,
               std::size_t rgb_count,
               std::size_t duration_ms,
               std::span<RgbFloat const> rgbs)
    : elapsed_ms_ { 0 }
    , zone_index_ { zone_index }
    , duration_ms_ { duration_ms }
    , texture_ { rgbs, rgb_count }
{
    /* Ensure we've got the right amount of RGBs
     * in each frame...
     */
    assert(rgbs.size() % rgb_count == 0);
}

auto Linear::zone_index() const noexcept -> std::uint32_t
{
    return zone_index_;
}

auto Linear::rgb_count() const noexcept -> std::size_t
{
    return texture_.width();
}

auto Linear::duration() const noexcept -> std::size_t
{
    return duration_ms_;
}

auto Linear::remaining() const noexcept -> std::size_t
{
    if (elapsed_ms_ > duration_ms_)
        return 0;

    return static_cast<std::size_t>(duration_ms_ - elapsed_ms_);
}

auto Linear::tick(std::size_t ms, std::span<rgbctl_rgb_value> out_frame)
    -> std::size_t
{
    if (!out_frame.size())
        return 0;

    elapsed_ms_ += ms;
    if (elapsed_ms_ > duration_ms_)
        elapsed_ms_ %= duration_ms_;

    float v
        = static_cast<float>(elapsed_ms_) / static_cast<float>(duration_ms_);

    float u = 1 / static_cast<float>(out_frame.size());

    std::size_t n = 0;
    std::for_each(out_frame.begin(), out_frame.end(), [&](auto& out) {
        Vec<float, 2> coords { u * static_cast<float>(n++), v };
        out = to_rgb_uint8(texture_.sample(coords, Filtering::Linear));
    });

    return n;
}

} // namespace rgbctl::effects
