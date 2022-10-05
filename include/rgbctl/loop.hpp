#ifndef RGBCTL_LOOP_HPP_INCLUDED
#define RGBCTL_LOOP_HPP_INCLUDED

#include <cassert>
#include <cinttypes>

namespace rgbctl
{

namespace detail
{

template <typename UnaryPredicate>
auto loop_callback(std::uint32_t val, void* fn) -> bool
{
    assert(fn);
    return (*reinterpret_cast<UnaryPredicate*>(fn))(val);
}

auto loop(std::uint32_t ms_per_loop,
          auto (*callback)(std::uint32_t, void*)->bool,
          void* fn) -> void;

} // namespace detail

template <typename UnaryPredicate>
auto loop(std::uint32_t ms_per_loop, UnaryPredicate f) -> void
{
    detail::loop(ms_per_loop, &detail::loop_callback<UnaryPredicate>, &f);
}

} // namespace rgbctl

#endif // RGBCTL_LOOP_HPP_INCLUDED
