#include "rgbctl/controller.hpp"
#include "rgbctl/assert.hpp"

namespace rgbctl
{

auto AnyController::tick(std::uint32_t elapsed_milliseconds) -> void
{
    RGBCTL_EXPECTS(inner_);
    tick_(inner_.get(), elapsed_milliseconds);
}

} // namespace rgbctl
