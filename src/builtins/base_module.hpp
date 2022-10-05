#ifndef RGBCTL_BUILTINS_BASE_MODULE_HPP_INCLUDED
#define RGBCTL_BUILTINS_BASE_MODULE_HPP_INCLUDED

#include "rgbctl/rgbctl.h"
#include <cassert>
#include <memory>
#include <type_traits>

template <typename This>
struct BaseModule : rgbctl_module
{
    BaseModule() noexcept
        : rgbctl_module {
            &on_rgb_data_, &on_query_zones_, &on_release_, &on_shutdown_
        }
    { }

    static auto acquire(rgbctl_device_context* ctx,
                        rgbctl_module_acquisition* acquisition) noexcept
        -> rgbctl_errno
    {
        std::unique_ptr<This> mod;
        rgbctl_errno err;
        try {
            mod.reset(new This);
            if ((err = mod->on_acquire(ctx)) != RGBCTL_SUCCESS)
                return err;
        }
        catch (std::exception const& e) {
            return -RGBCTL_ERR_MODULE_ALLOCATION;
        }

        *acquisition = { mod.get(), mod.get() };
        mod.release();

        return RGBCTL_SUCCESS;
    }

    template <typename Report>
    auto write_report(rgbctl_device_context* ctx, Report const& rpt) noexcept
        -> rgbctl_errno
    {
        static_assert(std::is_standard_layout_v<Report>,
                      "Must be standard layout");

        std::size_t written = 0;
        while (written < sizeof(rpt)) {
            auto result = rgbctl_write(
                ctx,
                reinterpret_cast<unsigned char const*>(&rpt) + written,
                static_cast<std::uint32_t>(sizeof(rpt) - written));

            if (result < 0)
                return result;

            written += static_cast<std::size_t>(result);
        }

        return static_cast<rgbctl_errno>(written);
    }

    template <typename Report>
    auto read_report(rgbctl_device_context* ctx, Report& rpt) noexcept
        -> rgbctl_errno
    {
        static_assert(std::is_standard_layout_v<Report>,
                      "Must be standard layout");

        std::size_t bytes_read = 0;
        while (bytes_read < sizeof(rpt)) {
            auto result = rgbctl_read(
                ctx,
                reinterpret_cast<unsigned char*>(&rpt) + bytes_read,
                static_cast<std::uint32_t>(sizeof(rpt) - bytes_read));

            if (result < 0)
                return result;

            bytes_read += static_cast<std::size_t>(result);
        }

        return static_cast<rgbctl_errno>(bytes_read);
    }

private:
    static auto on_rgb_data_(rgbctl_device_context* ctx,
                             std::uint32_t zone_index,
                             rgbctl_rgb_value const* rgb_data,
                             std::uint32_t rgb_data_size,
                             void* user_data) noexcept -> rgbctl_errno
    {
        assert(user_data);
        return reinterpret_cast<This*>(user_data)->on_rgb_data(
            ctx, zone_index, rgb_data, rgb_data_size);
    }

    static auto on_query_zones_(rgbctl_device_context* ctx,
                                rgbctl_zone const** zones,
                                void* user_data) noexcept -> rgbctl_errno
    {
        assert(user_data);
        return reinterpret_cast<This*>(user_data)->on_query_zones(ctx, zones);
    }

    static auto on_release_(rgbctl_device_context* ctx,
                            void* user_data) noexcept -> void
    {
        assert(user_data);
        reinterpret_cast<This*>(user_data)->on_release(ctx);
    }

    static auto on_shutdown_(void* user_data) noexcept -> void
    {
        assert(user_data);
        delete reinterpret_cast<This*>(user_data);
    }
};

#endif // RGBCTL_BUILTINS_BASE_MODULE_HPP_INCLUDED
