#ifndef RGBCTL_RAW_DEVICE_STREAM_HPP_INCLUDED
#define RGBCTL_RAW_DEVICE_STREAM_HPP_INCLUDED

#include "./rgbctl.h"
#include <cinttypes>
#include <string>

namespace rgbctl
{

struct RawDeviceStream
{
    RawDeviceStream() noexcept;
    explicit RawDeviceStream(std::string const& path);
    RawDeviceStream(RawDeviceStream&&) noexcept;
    ~RawDeviceStream();

    auto operator=(RawDeviceStream&&) noexcept -> RawDeviceStream&;
    friend auto swap(RawDeviceStream&, RawDeviceStream&) noexcept -> void;

    auto read(unsigned char*, std::uint32_t) noexcept -> rgbctl_errno;
    auto write(unsigned char const*, std::uint32_t) noexcept -> rgbctl_errno;

private:
    int file_no_;
};

auto swap(RawDeviceStream&, RawDeviceStream&) noexcept -> void;

} // namespace rgbctl

#endif // RGBCTL_RAW_DEVICE_STREAM_HPP_INCLUDED
