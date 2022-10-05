#include "testing.hpp"

auto passing_test() -> void { EXPECT(2 + 2 == 4); }

auto failing_test() -> void { EXPECT(1 + 2 == 4); }

auto main() -> int
{
    return rgbctl::testing::run({ TEST(passing_test), TEST(failing_test) });
}
