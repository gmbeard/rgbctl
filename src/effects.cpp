#include "rgbctl/effects.hpp"
#include "rgbctl/assert.hpp"

namespace rgbctl
{
auto AnyEffect::zone_index() const noexcept -> std::size_t
{
    RGBCTL_EXPECTS(inner_.get());
    return interface_.zone_index(inner_.get());
}

auto AnyEffect::rgb_count() const noexcept -> std::size_t
{
    RGBCTL_EXPECTS(inner_.get());
    return interface_.rgb_count(inner_.get());
}

auto AnyEffect::duration() const noexcept -> std::size_t
{
    RGBCTL_EXPECTS(inner_.get());
    return interface_.duration(inner_.get());
}

auto AnyEffect::remaining() const noexcept -> std::size_t
{
    RGBCTL_EXPECTS(inner_.get());
    return interface_.remaining(inner_.get());
}

auto AnyEffect::tick(std::size_t ms, std::span<rgbctl_rgb_value> out_val)
    -> std::size_t
{
    RGBCTL_EXPECTS(inner_.get());
    return interface_.tick(inner_.get(), ms, out_val);
}

} // namespace rgbctl
