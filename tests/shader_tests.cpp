#include "rgbctl/rgbctl.hpp"
#include "testing.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <span>
#include <vector>

namespace rgbctl
{

using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;

} // namespace rgbctl

struct Shader
{
    auto operator()(rgbctl::Vec2f const& coord,
                    rgbctl::Texture const& texture,
                    float /*dt*/) const -> rgbctl::Vec3f
    {
        return texture.sample(coord, rgbctl::Filtering::Linear);
    }
};

auto operator<<(std::ostream& os, rgbctl::Vec3f const& val) -> std::ostream&
{
    return os << "(" << val[0] << ", " << val[1] << ", " << val[2] << ")";
}

constexpr auto operator==(rgbctl::Vec3f const& lhs,
                          rgbctl::Vec3f const& rhs) noexcept -> bool
{
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

constexpr auto operator!=(rgbctl::Vec3f const& lhs,
                          rgbctl::Vec3f const& rhs) noexcept -> bool
{
    return !(lhs == rhs);
}

auto single_row()
{
    std::vector<rgbctl::Vec3f> rgbs;
    auto texture_pos = std::back_inserter(rgbs);

    *texture_pos++ = rgbctl::Vec3f(1.f, 0.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(1.f, 0.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 1.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 1.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 0.f, 1.f);
    *texture_pos++ = rgbctl::Vec3f(1.f, 0.f, 1.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 0.f, 1.f);

    rgbctl::Texture texture { { rgbs.data(), rgbs.size() } };

    Shader shader;

    EXPECT(shader({ 0.f, 1.f }, texture, 0.f)
           == rgbctl::to_vec<float>(1.f, 0.f, 0.f));
    EXPECT(shader({ 0.5f, 0.f }, texture, 0.f)
           == rgbctl::to_vec<float>(0.f, 0.5f, 0.5f));
    EXPECT(shader({ 1.f, 0.f }, texture, 0.f)
           == rgbctl::to_vec<float>(0.f, 0.f, 1.f));

    /* Should wrap...
     */
    EXPECT(shader({ 2.f, 0.f }, texture, 0.f)
           == rgbctl::to_vec<float>(1.f, 0.f, 0.f));
    EXPECT(shader({ 4.5f, 0.f }, texture, 0.f)
           == rgbctl::to_vec<float>(0.f, 0.5f, 0.5f));
    EXPECT(shader({ -2.f, 0.f }, texture, 0.f)
           == rgbctl::to_vec<float>(1.f, 0.f, 0.f));
    EXPECT(shader({ -4.5f, 0.f }, texture, 0.f)
           == rgbctl::to_vec<float>(0.f, 0.5f, 0.5f));
}

auto multiple_row()
{
    std::vector<rgbctl::Vec3f> rgbs;
    auto texture_pos = std::back_inserter(rgbs);

    *texture_pos++ = rgbctl::Vec3f(1.f, 0.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(1.f, 0.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(1.f, 0.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(1.f, 0.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 1.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 1.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 1.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 1.f, 0.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 0.f, 1.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 0.f, 1.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 0.f, 1.f);
    *texture_pos++ = rgbctl::Vec3f(0.f, 0.f, 1.f);

    rgbctl::Texture texture { { rgbs.data(), rgbs.size() }, 4 };

    Shader shader;

    auto result = shader({ 0.f, 1.f }, texture, 0.f);
    std::cerr << result << '\n';
    EXPECT(result == rgbctl::to_vec<float>(0.f, 0.f, 1.f));

    result = shader({ 0.5f, 0.5f }, texture, 0.f);
    std::cerr << result << '\n';
    EXPECT(result == rgbctl::to_vec<float>(0.f, 0.5f, 0.5f));

    result = shader({ 0.f, 0.f }, texture, 0.f);
    std::cerr << result << '\n';
    EXPECT(result == rgbctl::to_vec<float>(1.f, 0.f, 0.f));
}

auto main() -> int
{
    return rgbctl::testing::run({
        TEST(single_row),
        TEST(multiple_row),
    });
}
