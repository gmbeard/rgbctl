#ifndef RGBCTL_TESTS_MOCK_READ_WRITE_STREAM_HPP_INCLUDED
#define RGBCTL_TESTS_MOCK_READ_WRITE_STREAM_HPP_INCLUDED

#include "rgbctl/rgbctl.h"

#include <algorithm>
#include <iterator>
#include <span>
#include <tuple>

struct MockReadWriteStream
{
    explicit MockReadWriteStream(std::span<unsigned char const> read_from,
                                 std::span<unsigned char> write_to) noexcept
        : read_from_ { std::make_pair(read_from, 0) }
        , write_to_ { std::make_pair(write_to, 0) }
    {
    }

    auto read(unsigned char* buffer, std::uint32_t len) noexcept -> rgbctl_errno
    {
        auto read_range = available_read_range();

        auto end_read_pos = std::copy_n(
            read_range.begin(),
            std::min(std::distance(read_range.begin(), read_range.end()),
                     static_cast<std::ptrdiff_t>(len)),
            buffer);

        auto bytes_read = std::distance(buffer, end_read_pos);

        std::get<1>(read_from_) += bytes_read;

        return static_cast<rgbctl_errno>(bytes_read);
    }

    auto write(unsigned char const* buffer, std::uint32_t len) noexcept
        -> rgbctl_errno
    {
        auto write_range = available_write_range();

        auto end_write_pos = std::copy_n(
            buffer,
            std::min(std::distance(write_range.begin(), write_range.end()),
                     static_cast<std::ptrdiff_t>(len)),
            write_range.begin());

        auto bytes_written = std::distance(write_range.begin(), end_write_pos);

        std::get<1>(write_to_) += bytes_written;

        return static_cast<rgbctl_errno>(bytes_written);
    }

    auto byte_range_written() const noexcept -> std::span<unsigned char>
    {
        return std::get<0>(write_to_).subspan(
            0, static_cast<std::size_t>(std::get<1>(write_to_)));
    }

    auto byte_range_read() const noexcept -> std::span<unsigned char const>
    {
        return std::get<0>(read_from_)
            .subspan(0, static_cast<std::size_t>(std::get<1>(read_from_)));
    }

private:
    auto available_read_range() const noexcept -> std::span<unsigned char const>
    {
        return std::get<0>(read_from_)
            .subspan(static_cast<std::size_t>(std::get<1>(read_from_)));
    }

    auto available_write_range() const noexcept -> std::span<unsigned char>
    {
        return std::get<0>(write_to_).subspan(
            static_cast<std::size_t>(std::get<1>(write_to_)));
    }

    std::tuple<std::span<unsigned char const>, std::ptrdiff_t> read_from_;
    std::tuple<std::span<unsigned char>, std::ptrdiff_t> write_to_;
};

#endif // RGBCTL_TESTS_MOCK_READ_WRITE_STREAM_HPP_INCLUDED
