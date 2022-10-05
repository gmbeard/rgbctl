#ifndef RGBCTL_ACQUIRE_HPP_INCLUDED
#define RGBCTL_ACQUIRE_HPP_INCLUDED

#include "./assert.hpp"
#include "./device_context.hpp"
#include "./rgbctl.h"
#include <cinttypes>
#include <span>
#include <stdexcept>

namespace rgbctl
{

struct Module
{
    explicit Module(rgbctl_module_acquisition) noexcept;
    Module(Module&&) noexcept;
    ~Module();

    Module(Module const&) = delete;
    auto operator=(Module const&) -> Module& = delete;
    auto operator=(Module&&) noexcept -> Module&;

    friend auto swap(Module&, Module&) noexcept -> void;

    template <typename ReadWriteStream>
    auto send_rgb_data(DeviceContext<ReadWriteStream>& ctx,
                       std::uint32_t zone_index,
                       rgbctl_rgb_value const* data,
                       std::uint32_t len) -> void
    {
        RGBCTL_EXPECTS(acquisition_.module);
        if (0 > acquisition_.module->on_rgb_data(
                &ctx, zone_index, data, len, acquisition_.user_data))
            throw std::runtime_error { "send_rgb_data" };
    }

    template <typename ReadWriteStream>
    auto release(DeviceContext<ReadWriteStream>& ctx) -> void
    {
        RGBCTL_EXPECTS(acquisition_.module);
        acquisition_.module->on_release(&ctx, acquisition_.user_data);
    }

    template <typename ReadWriteStream>
    auto query_zones(DeviceContext<ReadWriteStream>& ctx)
        -> std::span<rgbctl_zone const>
    {
        RGBCTL_EXPECTS(acquisition_.module);
        rgbctl_zone const* zones_out;
        auto zone_count = acquisition_.module->on_query_zones(
            &ctx, &zones_out, acquisition_.user_data);

        if (zone_count < 0)
            return {};

        return { zones_out, static_cast<std::size_t>(zone_count) };
    }

    operator bool() const noexcept;

private:
    auto shutdown() noexcept -> void;

    rgbctl_module_acquisition acquisition_;
};

auto swap(Module&, Module&) noexcept -> void;

template <typename ReadWriteStream>
auto acquire_module(DeviceContext<ReadWriteStream>& ctx,
                    rgbctl_product_id id,
                    rgbctl_module_acquisition_callback fn,
                    void* user_data) -> Module
{
    rgbctl_module_acquisition acquisition {};
    if (0 != fn(&ctx, id, &acquisition, user_data))
        throw std::runtime_error { "acquire_module" };

    return Module { acquisition };
}

template <typename ReadWriteStream>
auto acquire_module(DeviceContext<ReadWriteStream>& ctx,
                    rgbctl_product_id id,
                    rgbctl_module_acquisition_callback fn) -> Module
{
    return acquire_module(ctx, id, fn, nullptr);
}

template <typename ReadWriteStream>
auto acquire_module(DeviceContext<ReadWriteStream>& ctx,
                    auto (*fn)(rgbctl_device_context*,
                               rgbctl_module_acquisition*)
                        ->rgbctl_errno) -> Module
{
    rgbctl_module_acquisition acquisition {};
    if (0 != fn(&ctx, &acquisition))
        throw std::runtime_error { "acquire_module" };

    return Module { acquisition };
}

} // namespace rgbctl

#endif // RGBCTL_ACQUIRE_HPP_INCLUDED
