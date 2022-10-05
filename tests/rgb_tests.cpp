#include "rgbctl/rgbctl.hpp"
#include "testing.hpp"
#include <string_view>

auto should_parse_hex() -> void
{
    rgbctl::RgbFloat rgb;
    std::string_view const data = "ffffff";

    EXPECT(rgbctl::hex_string_to_rgb_float(data, rgb));

    EXPECT(get<0>(rgb) == 1.f);

    EXPECT(rgbctl::hex_string_to_rgb_float("012345", rgb));
    EXPECT(rgbctl::hex_string_to_rgb_float("fedcba", rgb));
    EXPECT(rgbctl::hex_string_to_rgb_float("deadbe", rgb));
}

auto should_fail_to_parse_hex() -> void
{
    rgbctl::RgbUint8 rgb;

    EXPECT(!rgbctl::hex_string_to_rgb_uint8("ghijkl", rgb));
    EXPECT(!rgbctl::hex_string_to_rgb_uint8("ff", rgb));
    EXPECT(!rgbctl::hex_string_to_rgb_uint8("ff000", rgb));
}

auto should_allow_arithmetic() -> void
{
    using u8 = std::uint8_t;
    using rgbctl::rgb;

    rgbctl::RgbUint8 result =
        rgb(u8(2), u8(2), u8(2)) - rgb(u8(1), u8(1), u8(1));

    EXPECT(get<0>(result) == u8(1));
    EXPECT(get<1>(result) == u8(1));
    EXPECT(get<2>(result) == u8(1));

    result = rgb(u8(2), u8(2), u8(2)) * u8(2);

    EXPECT(get<0>(result) == u8(4));
    EXPECT(get<1>(result) == u8(4));
    EXPECT(get<2>(result) == u8(4));
}

auto should_allow_interpolation() -> void
{
    rgbctl::RgbFloat start, end;

    EXPECT(hex_string_to_rgb_float("000000", start));
    EXPECT(hex_string_to_rgb_float("ffffff", end));

    auto mid = start + ((end - start) * .5f);

    EXPECT(get<0>(mid) == .5f);
    EXPECT(get<1>(mid) == .5f);
    EXPECT(get<2>(mid) == .5f);
}

auto should_allow_conversion() -> void
{
    rgbctl::RgbFloat float_val;
    EXPECT(hex_string_to_rgb_float("7f7f7f", float_val));

    auto uint8_val = to_rgb_uint8(float_val);

    EXPECT(uint8_val.red == 0x7f);
    EXPECT(uint8_val.green == 0x7f);
    EXPECT(uint8_val.blue == 0x7f);
}

auto main() -> int
{
    return rgbctl::testing::run({
        TEST(should_parse_hex),
        TEST(should_fail_to_parse_hex),
        TEST(should_allow_arithmetic),
        TEST(should_allow_interpolation),
        TEST(should_allow_conversion),
    });
}
