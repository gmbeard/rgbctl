#include "rgbctl/rgbctl.hpp"
#include "testing.hpp"
#include <array>
#include <iomanip>
#include <iostream>

auto linear_should_return_correct_val() -> void
{
    std::array<rgbctl::RgbFloat, 6> inputs {};
    EXPECT(hex_string_to_rgb_float("ff0000", inputs[0]));
    EXPECT(hex_string_to_rgb_float("ff0000", inputs[1]));
    EXPECT(hex_string_to_rgb_float("00ff00", inputs[2]));
    EXPECT(hex_string_to_rgb_float("00ff00", inputs[3]));
    EXPECT(hex_string_to_rgb_float("0000ff", inputs[4]));
    EXPECT(hex_string_to_rgb_float("0000ff", inputs[5]));

    rgbctl::effects::Linear effect {
        0, 2, 1000, { inputs.data(), inputs.size() }
    };

    std::array<rgbctl_rgb_value, 2> result;
    auto num = effect.tick(1000, { result.data(), result.size() });

    EXPECT(num == result.size());

    std::cerr << std::hex << std::setfill('0') << std::setw(2)
              << (int)result[0].red << std::setw(2) << (int)result[0].green
              << std::setw(2) << (int)result[0].blue << '\n';

    EXPECT(result[0].red == 0x00);
    EXPECT(result[0].green == 0x00);
    EXPECT(result[0].blue == 0xff);
}

auto rotate_should_return_correct_val() -> void
{
    std::array<rgbctl::RgbFloat, 3> inputs {};
    auto rgb = inputs.begin();
    EXPECT(hex_string_to_rgb_float("ff0000", *rgb++));
    EXPECT(hex_string_to_rgb_float("00ff00", *rgb++));
    EXPECT(hex_string_to_rgb_float("0000ff", *rgb++));

    rgbctl::effects::Rotate effect { 0,
                                     1000,
                                     { inputs.data(), inputs.size() } };

    std::array<rgbctl_rgb_value, inputs.size()> result;
    auto num = effect.tick(1000, { result.data(), result.size() });

    EXPECT(num == result.size());

    std::cerr << std::hex << std::setfill('0') << std::setw(2)
              << (int)result[0].red << std::setw(2) << (int)result[0].green
              << std::setw(2) << (int)result[0].blue << '\n';

    EXPECT(result[0].red == 0xff);
    EXPECT(result[0].green == 0x00);
    EXPECT(result[0].blue == 0x00);
}

auto rotate_should_have_correct_step_values() -> void
{
    std::array<rgbctl::RgbFloat, 4> inputs {};
    std::array<rgbctl_rgb_value, inputs.size()> result;

    auto rgb = inputs.begin();
    EXPECT(hex_string_to_rgb_float("ff0000", *rgb++));
    EXPECT(hex_string_to_rgb_float("00ff00", *rgb++));
    EXPECT(hex_string_to_rgb_float("0000ff", *rgb++));
    EXPECT(hex_string_to_rgb_float("000000", *rgb++));

    rgbctl::effects::Rotate effect { 0,
                                     1000,
                                     { inputs.data(), inputs.size() } };

    auto num = effect.tick(250, { result.data(), result.size() });
    EXPECT(num == result.size());

    std::cerr << std::hex << std::setfill('0') << std::setw(2)
              << (int)result[0].red << std::setw(2) << (int)result[0].green
              << std::setw(2) << (int)result[0].blue << '\n';

    EXPECT(result[0].red == 0x00);
    EXPECT(result[0].green == 0x00);
    EXPECT(result[0].blue == 0x00);

    num = effect.tick(250, { result.data(), result.size() });
    EXPECT(num == result.size());

    std::cerr << std::hex << std::setfill('0') << std::setw(2)
              << (int)result[0].red << std::setw(2) << (int)result[0].green
              << std::setw(2) << (int)result[0].blue << '\n';

    EXPECT(result[0].red == 0x00);
    EXPECT(result[0].green == 0x00);
    EXPECT(result[0].blue == 0xff);

    num = effect.tick(250, { result.data(), result.size() });
    EXPECT(num == result.size());

    std::cerr << std::hex << std::setfill('0') << std::setw(2)
              << (int)result[0].red << std::setw(2) << (int)result[0].green
              << std::setw(2) << (int)result[0].blue << '\n';

    EXPECT(result[0].red == 0x00);
    EXPECT(result[0].green == 0xff);
    EXPECT(result[0].blue == 0x00);
}

auto should_be_compatible_with_effect_concept() -> void
{
    using rgbctl::AnyEffect;
    using rgbctl::RgbFloat;
    using rgbctl::effects::Rotate;

    std::array<rgbctl::RgbFloat, 3> inputs {};
    std::array<rgbctl_rgb_value, inputs.size()> result;

    auto rgb = inputs.begin();
    EXPECT(hex_string_to_rgb_float("ff0000", *rgb++));
    EXPECT(hex_string_to_rgb_float("00ff00", *rgb++));
    EXPECT(hex_string_to_rgb_float("0000ff", *rgb++));

    AnyEffect effect { Rotate { 0, 10, { inputs.data(), inputs.size() } } };

    auto num = effect.tick(1000, { result.data(), result.size() });

    EXPECT(num == result.size());
}

auto main() -> int
{
    return rgbctl::testing::run({
        TEST(linear_should_return_correct_val),
        TEST(rotate_should_return_correct_val),
        TEST(rotate_should_have_correct_step_values),
        TEST(should_be_compatible_with_effect_concept),
    });
}
