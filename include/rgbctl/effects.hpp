#ifndef RGBCTL_EFFECTS_HPP_INCLUDED
#define RGBCTL_EFFECTS_HPP_INCLUDED

#include "./effects/linear.hpp"
#include "./effects/rotate.hpp"
#include "./rgbctl.h"
#include <concepts>
#include <memory>
#include <span>
#include <type_traits>

namespace rgbctl
{
// clang-format off

template <typename T>
concept Effect = requires(T effect, std::size_t ms, std::span<rgbctl_rgb_value> out_val)
{
    { effect.zone_index() } noexcept -> std::convertible_to<std::size_t>;

    { effect.rgb_count() } noexcept -> std::convertible_to<std::size_t>;

    { effect.duration() } noexcept -> std::convertible_to<std::size_t>;

    { effect.remaining() } noexcept -> std::convertible_to<std::size_t>;

    { effect.tick(ms, out_val) } -> std::convertible_to<std::size_t>;
};

// clang-format on

struct AnyEffect
{
    template <Effect T>
    explicit AnyEffect(T&& inner) requires(
        // clang-format off
        !std::is_same_v<AnyEffect, std::decay_t<T>> &&
        !std::is_const_v<std::remove_reference_t<T>>)
        // clang-format on
        : interface_ { &zone_index_impl<std::remove_reference_t<T>>,
                       &rgb_count_impl<std::remove_reference_t<T>>,
                       &duration_impl<std::remove_reference_t<T>>,
                       &remaining_impl<std::remove_reference_t<T>>,
                       &tick_impl<std::remove_reference_t<T>> }
        , inner_ { new T { std::move(inner) },
                   &deleter<std::remove_reference_t<T>> }
    { }

    auto zone_index() const noexcept -> std::size_t;

    auto rgb_count() const noexcept -> std::size_t;

    auto duration() const noexcept -> std::size_t;

    auto remaining() const noexcept -> std::size_t;

    auto tick(std::size_t, std::span<rgbctl_rgb_value>) -> std::size_t;

private:
    template <Effect T>
    static auto zone_index_impl(void const* p) -> std::size_t
    {
        return reinterpret_cast<T const*>(p)->zone_index();
    }

    template <Effect T>
    static auto rgb_count_impl(void const* p) -> std::size_t
    {
        return reinterpret_cast<T const*>(p)->rgb_count();
    }

    template <Effect T>
    static auto duration_impl(void const* p) -> std::size_t
    {
        return reinterpret_cast<T const*>(p)->duration();
    }

    template <Effect T>
    static auto remaining_impl(void const* p) -> std::size_t
    {
        return reinterpret_cast<T const*>(p)->remaining();
    }

    template <Effect T>
    static auto tick_impl(void* p,
                          std::size_t ms,
                          std::span<rgbctl_rgb_value> out_val) -> std::size_t
    {
        return reinterpret_cast<T*>(p)->tick(ms, out_val);
    }

    struct Interface
    {
        auto (*zone_index)(void const*) -> std::size_t;
        auto (*rgb_count)(void const*) -> std::size_t;
        auto (*duration)(void const*) -> std::size_t;
        auto (*remaining)(void const*) -> std::size_t;
        auto (*tick)(void*, std::size_t, std::span<rgbctl_rgb_value>)
            -> std::size_t;
    };

    template <Effect T>
    static auto deleter(void* p) noexcept -> void
    {
        delete reinterpret_cast<T*>(p);
    }

    Interface interface_;
    std::unique_ptr<void, auto (*)(void*) noexcept->void> inner_;
};

} // namespace rgbctl

#endif // RGBCTL_EFFECTS_HPP_INCLUDED
