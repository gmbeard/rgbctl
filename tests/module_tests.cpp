#include "../src/builtins/asus/asus_x570.hpp"
#include "./mock_read_write_stream.hpp"
#include "rgbctl/rgbctl.h"
#include "rgbctl/rgbctl.hpp"
#include "testing.hpp"
#include <array>
#include <iostream>
#include <span>
#include <tuple>

struct TestReport
{
    std::array<std::uint8_t, 3> padding_a_;
    std::uint8_t test_value;
    std::array<std::uint8_t, 59> padding_b_;
};

auto should_have_correct_report_size() -> void
{
    using namespace rgbctl::modules::builtin;
    std::cerr << sizeof(asus::Report<asus::RawData>) << '\n';
    EXPECT(sizeof(asus::Report<asus::RawData>) == asus::kAsusAuraReportSize);
}

auto should_initialize_default_constructed_report_to_zeros() -> void
{
    rgbctl::modules::builtin::asus::Report<
        rgbctl::modules::builtin::asus::RawData> const rpt {};

    auto const* first = reinterpret_cast<unsigned char const*>(&rpt);
    auto const* last = first + sizeof(rpt);

    /* I know this is technically UB if any bytes weren't
     * initialized, but the sanitizers _should_ catch this
     * :pray:
     */
    EXPECT(std::all_of(first, last, [](auto const& val) { return val == 0; }));
}

auto should_read() -> void
{
    using namespace rgbctl::modules::builtin;

    using asus::AsusX570;
    using asus::RawData;
    using asus::Report;

    TestReport data {};
    data.test_value = 0x02;

    Report<TestReport> const in_rpt { 0x03, data };
    std::array<unsigned char, 64> write_buffer {};

    rgbctl::DeviceContext ctx { MockReadWriteStream {
        { reinterpret_cast<unsigned char const*>(&in_rpt), sizeof(in_rpt) },
        { &write_buffer[0], write_buffer.size() } } };

    AsusX570 driver;
    Report<TestReport> out_rpt;

    EXPECT(driver.read_report(&ctx, out_rpt) == sizeof(out_rpt));
    EXPECT(out_rpt.report_id == in_rpt.report_id);
    EXPECT(out_rpt.report_data.test_value == in_rpt.report_data.test_value);
}

auto main() -> int
{
    return rgbctl::testing::run(
        { TEST(should_have_correct_report_size),
          TEST(should_read),
          TEST(should_initialize_default_constructed_report_to_zeros) });
}
