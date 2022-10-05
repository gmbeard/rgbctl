#include "./corsair_utils.hpp"
#include <numeric>

namespace rgbctl::modules::builtin::corsair
{

constexpr auto compute_checksum(std::span<std::uint8_t const> data) noexcept
    -> std::uint8_t
{
    return compute_checksum(data.begin(), data.end());
}

} // namespace rgbctl::modules::builtin::corsair
