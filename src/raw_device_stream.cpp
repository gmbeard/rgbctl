#include "rgbctl/raw_device_stream.hpp"
#include <errno.h>
#include <fcntl.h>
#include <system_error>
#include <unistd.h>
#include <utility>

namespace rgbctl
{

RawDeviceStream::RawDeviceStream() noexcept
    : file_no_ { -1 }
{
}

RawDeviceStream::RawDeviceStream(RawDeviceStream&& other) noexcept
    : file_no_ { std::exchange(other.file_no_, -1) }
{
}

RawDeviceStream::~RawDeviceStream()
{
    if (file_no_ >= 0)
        close(file_no_);
}

RawDeviceStream::RawDeviceStream(std::string const& path)
    : file_no_ { open(path.c_str(), O_RDWR) }
{
    if (file_no_ < 0)
        throw std::system_error { errno, std::system_category() };
}

auto RawDeviceStream::operator=(RawDeviceStream&& rhs) noexcept
    -> RawDeviceStream&
{
    auto tmp { std::move(rhs) };
    swap(*this, tmp);
    return *this;
}

auto swap(RawDeviceStream& lhs, RawDeviceStream& rhs) noexcept -> void
{
    using std::swap;
    swap(lhs.file_no_, rhs.file_no_);
}

auto RawDeviceStream::read(unsigned char* buffer, std::uint32_t n) noexcept
    -> rgbctl_errno
{
    std::int32_t result;
    if ((result = static_cast<std::int32_t>(::read(file_no_, buffer, n))) < 0)
        return -RGBCTL_ERR_READ;

    return result;
}

auto RawDeviceStream::write(unsigned char const* buffer,
                            std::uint32_t n) noexcept -> rgbctl_errno
{
    std::int32_t result;
    if ((result = static_cast<std::int32_t>(::write(file_no_, buffer, n))) < 0)
        return -RGBCTL_ERR_WRITE;

    return result;
}

} // namespace rgbctl
