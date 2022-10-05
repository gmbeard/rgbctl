#ifndef RGBCTL_DEVICE_CONTEXT_HPP_INCLUDED
#define RGBCTL_DEVICE_CONTEXT_HPP_INCLUDED

#include "./rgbctl.h"
#include <cinttypes>
#include <utility>

struct rgbctl_device_context
{
    auto (*read)(rgbctl_device_context*, unsigned char*, std::uint32_t)
        -> rgbctl_errno;
    auto (*write)(rgbctl_device_context*, unsigned char const*, std::uint32_t)
        -> rgbctl_errno;
};

namespace rgbctl
{

template <typename ReadWriteStream>
struct DeviceContext : rgbctl_device_context
{
    using stream_type = ReadWriteStream;

    DeviceContext(stream_type stream) noexcept
        : rgbctl_device_context { &read_, &write_ }
        , stream_ { std::move(stream) }
    {
    }

    auto stream() noexcept -> stream_type& { return stream_; }
    auto stream() const noexcept -> stream_type const& { return stream_; }

private:
    static auto read_(rgbctl_device_context* ctx,
                      unsigned char* buffer,
                      std::uint32_t len) noexcept -> rgbctl_errno
    {
        auto& self = *static_cast<DeviceContext*>(ctx);
        return self.stream().read(buffer, len);
    }

    static auto write_(rgbctl_device_context* ctx,
                       unsigned char const* buffer,
                       std::uint32_t len) noexcept -> rgbctl_errno
    {
        auto& self = *static_cast<DeviceContext*>(ctx);
        auto const result = self.stream().write(buffer, len);
        return result;
    }

    ReadWriteStream stream_;
};

} // namespace rgbctl

#endif // RGBCTL_DEVICE_CONTEXT_HPP_INCLUDED
