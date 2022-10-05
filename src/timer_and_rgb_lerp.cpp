#include <cmath>
#include <cstddef>
#include <errno.h>
#include <iomanip>
#include <iostream>
#include <sys/select.h>
#include <system_error>
#include <thread>
#include <time.h>

auto to_milliseconds(timespec tv) noexcept -> std::uint32_t
{
    return static_cast<std::uint32_t>((tv.tv_sec * 1000) +
                                      (tv.tv_nsec / 1'000'000));
}

auto from_milliseconds(std::uint32_t val) noexcept -> timespec
{
    return timespec { .tv_sec = val / 1000,
                      .tv_nsec = (val % 1'000) * 1'000'000 };
}

template <typename UnaryFunction>
auto loop(std::uint32_t loop_time_ms, UnaryFunction f) -> void
{
    timespec then {};
    timespec now {};
    clock_gettime(CLOCK_MONOTONIC, &then);
    std::uint32_t target_ms = 0;

    f(std::uint32_t { 0 });

    while (true) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        auto elapsed_ms = to_milliseconds(now) - to_milliseconds(then);
        then = now;

        target_ms += elapsed_ms;
        if (target_ms >= loop_time_ms) {
            f(target_ms);
            auto loop_count = target_ms / loop_time_ms;
            target_ms -= loop_time_ms * loop_count;
        }

        auto sleep_for_ms = from_milliseconds(loop_time_ms - target_ms);

        auto select_result =
            pselect(0, nullptr, nullptr, nullptr, &sleep_for_ms, nullptr);

        if (select_result < 0)
            throw std::system_error { errno, std::system_category() };
    }
}

template <typename T>
struct RGB
{
    T r;
    T g;
    T b;
};

auto normalize(RGB<float> const& val, float max) noexcept -> RGB<float>
{
    return { std::min(max, val.r / max),
             std::min(max, val.g / max),
             std::min(max, val.b / max) };
}

decltype(auto) operator<<(std::ostream& os, RGB<float> const& val)
{
    auto const normalized = normalize(val, 255.f);

    os << std::setfill('0') << std::hex << std::setw(2)
       << static_cast<std::uint32_t>(normalized.r * 255) << std::hex
       << std::setw(2) << static_cast<std::uint32_t>(normalized.g * 255)
       << std::hex << std::setw(2)
       << static_cast<std::uint32_t>(normalized.b * 255);
    return (os);
}

auto rgb(float r, float g, float b) noexcept -> RGB<float>
{
    return { r, g, b };
}

auto lerp(RGB<float> const& a, RGB<float> const& b, float t) noexcept
    -> RGB<float>
{
    return { std::lerp(a.r, b.r, t),
             std::lerp(a.g, b.g, t),
             std::lerp(a.b, b.b, t) };
}

auto app() -> void
{
    std::uint32_t const duration_ms = 10000;
    std::array<RGB<float>, 3> const key_frames { rgb(255.f, 0.f, 0.f),
                                                 rgb(0.f, 255.f, 0.f),
                                                 rgb(0.f, 0.f, 255.f) };

    std::uint32_t total_ms = 0;

    auto constexpr kLoopTimeMillisecons = 200;
    loop(kLoopTimeMillisecons, [&](auto const loop_time_ms) {
        total_ms += loop_time_ms;
        if (total_ms >= duration_ms)
            total_ms -= duration_ms;

        auto duration_vector = static_cast<float>(total_ms) / duration_ms;
        auto key_frame_index =
            static_cast<long>((key_frames.size() - 1) * duration_vector);

        auto key_frame = std::next(begin(key_frames), key_frame_index);
        std::cout << lerp(*key_frame, *std::next(key_frame), duration_vector)
                  << std::dec << " (" << duration_vector << ")" << '\n';
    });
}

auto main() -> int
{
    try {
        app();
        return 0;
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 1;
}
