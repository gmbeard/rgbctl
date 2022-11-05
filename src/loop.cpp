#include "rgbctl/loop.hpp"
#include <atomic>
#include <cmath>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <system_error>
#include <time.h>

namespace
{

auto to_milliseconds(timespec tv) noexcept -> std::uint32_t
{
    return static_cast<std::uint32_t>((tv.tv_sec * 1000)
                                      + (tv.tv_nsec / 1'000'000));
}

auto from_milliseconds(std::uint32_t val) noexcept -> timespec
{
    return timespec { .tv_sec = val / 1000,
                      .tv_nsec = (val % 1'000) * 1'000'000 };
}

std::atomic_size_t sigints_received = 0;

auto sigint_handler(int) -> void
{
    sigints_received++;
}

} // namespace

namespace rgbctl
{

auto detail::loop(std::uint32_t ms_per_loop,
                  auto (*f)(std::uint32_t, void*)->bool,
                  void* fn) -> void
{
    sigints_received = 0;
    sigset_t blockset, emptyset, savedset;
    sigemptyset(&emptyset);
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGINT);
    sigprocmask(SIG_BLOCK, &blockset, &savedset);

    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, nullptr);

    timespec then {};
    timespec now {};
    clock_gettime(CLOCK_MONOTONIC, &then);
    std::uint32_t target_ms = 0;

    if (!f(std::uint32_t { 0 }, fn))
        return;

    while (true) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        auto elapsed_ms = to_milliseconds(now) - to_milliseconds(then);
        then = now;

        target_ms += elapsed_ms;
        if (target_ms >= ms_per_loop) {
            if (!f(target_ms, fn))
                break;
            auto loop_count = target_ms / ms_per_loop;
            target_ms -= ms_per_loop * loop_count;
        }

        auto sleep_for_ms = from_milliseconds(ms_per_loop - target_ms);

        auto select_result
            = pselect(0, nullptr, nullptr, nullptr, &sleep_for_ms, &emptyset);

        if (select_result < 0) {
            if (errno == EINTR && sigints_received > 0)
                break;

            throw std::system_error { errno, std::system_category() };
        }
    }

    sigprocmask(SIG_SETMASK, &savedset, nullptr);
}

} // namespace rgbctl
