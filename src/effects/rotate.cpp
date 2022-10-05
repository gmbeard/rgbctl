#include "rgbctl/effects/rotate.hpp"
#include <cassert>
#include <cmath>

namespace rgbctl::effects
{

Rotate::Rotate(std::uint32_t zone_index,
               std::size_t duration_ms,
               std::span<RgbFloat const> data)
    : elapsed_ms_ { 0 }
    , zone_index_ { zone_index }
    , duration_ms_ { duration_ms }
    , texture_ { { data.begin(), data.end() } }
{ }

auto Rotate::zone_index() const noexcept -> std::uint32_t
{
    return zone_index_;
}

auto Rotate::rgb_count() const noexcept -> std::size_t
{
    return texture_.width();
}

auto Rotate::duration() const noexcept -> std::size_t
{
    return duration_ms_;
}

auto Rotate::remaining() const noexcept -> std::size_t
{
    if (elapsed_ms_ > duration_ms_)
        return 0;

    return duration_ms_ - elapsed_ms_;
}

auto Rotate::tick(std::size_t ms, std::span<rgbctl_rgb_value> out_frame)
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
        Vec<float, 2> coords { u * static_cast<float>(n++) - v, 0.f };
        out = to_rgb_uint8(texture_.sample(coords, Filtering::Linear));
    });

    return n;
}

} // namespace rgbctl::effects
