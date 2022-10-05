#include "rgbctl/rgbctl.hpp"
#include "testing.hpp"

auto should_be_less_than() -> void
{
    rgbctl_product_id a, b;

    a = { 1, 0 };
    b = { 2, 0 };
    EXPECT(a < b);

    a = { 1, 0 };
    b = { 1, 2 };
    EXPECT(a < b);
}

auto should_be_equal() -> void
{
    rgbctl_product_id a, b;

    a = { 1, 0 };
    b = { 1, 0 };
    EXPECT(a == b);

    a = { 1, 2 };
    b = { 1, 2 };
    EXPECT(a == b);
}

auto should_be_less_than_or_equal() -> void
{
    rgbctl_product_id a, b;

    a = { 1, 0 };
    b = { 1, 2 };
    EXPECT(a <= b);

    a = { 1, 2 };
    b = { 1, 2 };
    EXPECT(a <= b);
}

auto should_be_greater_than() -> void
{
    rgbctl_product_id a, b;

    a = { 1, 2 };
    b = { 1, 0 };
    EXPECT(a > b);

    a = { 2, 0 };
    b = { 1, 2 };
    EXPECT(a > b);
}

auto should_be_greater_than_or_equal() -> void
{
    rgbctl_product_id a, b;

    a = { 1, 2 };
    b = { 1, 0 };
    EXPECT(a > b);

    a = { 1, 2 };
    b = { 1, 2 };
    EXPECT(a >= b);
}

auto main() -> int
{
    return rgbctl::testing::run({
        TEST(should_be_less_than),
        TEST(should_be_equal),
        TEST(should_be_less_than_or_equal),
        TEST(should_be_greater_than),
        TEST(should_be_greater_than_or_equal),
    });
}
