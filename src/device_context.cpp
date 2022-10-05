#include "rgbctl/device_context.hpp"
#include <cassert>

auto rgbctl_read(rgbctl_device_context* ctx,
                 unsigned char* buffer,
                 std::uint32_t len) -> rgbctl_errno
{
    assert(ctx);
    assert(buffer || len == 0);
    return ctx->read(ctx, buffer, len);
}

auto rgbctl_write(rgbctl_device_context* ctx,
                  unsigned char const* buffer,
                  std::uint32_t len) -> rgbctl_errno
{
    assert(ctx);
    assert(buffer || len == 0);
    return ctx->write(ctx, buffer, len);
}
