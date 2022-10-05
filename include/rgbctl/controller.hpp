#ifndef RGBCTL_CONTROLLER_HPP_INCLUDED
#define RGBCTL_CONTROLLER_HPP_INCLUDED

#include "./acquire.hpp"
#include "./assert.hpp"
#include "./device_context.hpp"
#include "./narrow.hpp"
#include <cinttypes>
#include <memory>
#include <span>
#include <vector>

namespace rgbctl
{

template <typename ReadWriteStream, typename Effect>
struct Controller
{
    using stream_type = ReadWriteStream;
    using device_context_type = DeviceContext<ReadWriteStream>;
    using effect_type = Effect;

    Controller(Controller&&) noexcept = default;

    Controller(Module&& mod,
               device_context_type&& device_context,
               effect_type&& effect)
        : module_ { std::move(mod) }
        , device_context_ { std::move(device_context) }
        , effect_ { std::move(effect) }
    {
        auto zones = module_.query_zones(device_context);
        if (effect_.zone_index() < zones.size())
            rgb_value_buffer_.resize(zones[effect_.zone_index()].rgb_count);
    }

    ~Controller()
    {
        if (module_)
            module_.release(device_context_);
    }

    auto tick(std::uint32_t elapsed_milliseconds) -> void
    {
        std::span<rgbctl_rgb_value> out_val { rgb_value_buffer_.data(),
                                              rgb_value_buffer_.size() };

        auto rgbs_processed = effect().tick(elapsed_milliseconds, out_val);
        RGBCTL_EXPECTS(can_narrow<std::uint32_t>(rgbs_processed));
        module_.send_rgb_data(device_context_,
                              effect().zone_index(),
                              out_val.data(),
                              narrow_cast<std::uint32_t>(rgbs_processed));
    }

    auto module() noexcept -> Module&
    {
        return module_;
    }

    auto module() const noexcept -> Module const&
    {
        return module_;
    }

    auto device_context() noexcept -> device_context_type&
    {
        return device_context_;
    }

    auto device_context() const noexcept -> device_context_type const&
    {
        return device_context_;
    }

    auto effect() noexcept -> effect_type&
    {
        return effect_;
    }

    auto effect() const noexcept -> effect_type const&
    {
        return effect_;
    }

private:
    auto send_rgb_data(std::uint32_t zone_index,
                       rgbctl_rgb_value const* data,
                       std::uint32_t len) -> void
    {
        module_.send_rgb_data(device_context_, zone_index, data, len);
    }

    Module module_;
    device_context_type device_context_;
    effect_type effect_;
    std::vector<rgbctl_rgb_value> rgb_value_buffer_;
};

struct AnyController
{
    template <typename ReadWriteStream, typename Effect>
    explicit AnyController(Controller<ReadWriteStream, Effect>&& inner)
        : inner_ { new Controller<ReadWriteStream, Effect> { std::move(inner) },
                   deleter<Controller<ReadWriteStream, Effect>> }
        , tick_ { tick_impl<Controller<ReadWriteStream, Effect>> }
    { }

    auto tick(std::uint32_t elapsed_milliseconds) -> void;

private:
    template <typename T>
    static auto tick_impl(void* inner, std::uint32_t elapsed_milliseconds)
        -> void
    {
        (*reinterpret_cast<T*>(inner)).tick(elapsed_milliseconds);
    }

    template <typename T>
    static auto deleter(void* p) noexcept -> void
    {
        delete reinterpret_cast<T*>(p);
    }

    using Deleter = auto (*)(void*) noexcept -> void;

    std::unique_ptr<void, Deleter> inner_;
    auto (*tick_)(void*, std::uint32_t) -> void;
};

template <typename ReadWriteStream, typename Effect>
auto make_controller(DeviceContext<ReadWriteStream>&& ctx,
                     Effect&& effect,
                     rgbctl_product_id id,
                     rgbctl_module_acquisition_callback entry) -> AnyController
{
    auto mod = acquire_module(ctx, id, entry);
    return AnyController { Controller<ReadWriteStream, Effect> {
        std::move(mod), std::move(ctx), std::move(effect) } };
}

} // namespace rgbctl

#endif // RGBCTL_CONTROLLER_HPP_INCLUDED
