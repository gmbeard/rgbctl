#include "./asus_x570.hpp"
#include <array>
#include <cinttypes>
#include <cstring>
#include <utility>

#include <iomanip>
#include <iostream>
namespace
{
template <typename ReportType>
decltype(auto)
operator<<(std::ostream& os,
           rgbctl::modules::builtin::asus::Report<ReportType> const& val)
{
    int constexpr kMaxCols = 8;
    int col = 0;
    int row = 0;
    auto const* first = reinterpret_cast<unsigned char const*>(&val);
    auto const* last = first + sizeof(val);

    for (auto const* pos = first; pos != last; ++pos) {
        if (col == kMaxCols) {
            os << '\n';
            col = 0;
            ++row;
        }

        if (col == 0) {
            os << std::hex << std::setfill('0') << std::setw(2)
               << (std::uint32_t)(row * 8) << ": ";
        }

        os << std::hex << std::setfill('0') << std::setw(2)
           << (std::uint32_t)*pos << ' ';

        ++col;
    }

    return os;
}

} // namespace

namespace rgbctl::modules::builtin::asus
{

auto AsusX570::on_acquire(rgbctl_device_context* ctx) noexcept -> rgbctl_errno
{
    //

    rgbctl_errno result;

    auto config_request = make_report(kAsusAuraConfigReportId, RawData {});

    if ((result = write_report(ctx, config_request)) < 0)
        return result;

    Report<ConfigData> config;
    if ((result = read_report(ctx, config)) < 0)
        return result;

    led_count_ = config.report_data.led_count;
    std::cerr << config << '\n';

    std::cerr << "HEADER COUNT     : " << (int)config.report_data.channel_count
              << '\n';
    std::cerr << "LED COUNT        : " << (int)config.report_data.led_count
              << '\n';
    std::cerr << "RGB HEADER COUNT : "
              << (int)config.report_data.rgb_header_count << '\n';

    auto firmware_request = make_report(kAsusAuraFirmwareReportId, RawData {});

    if ((result = write_report(ctx, firmware_request)) < 0)
        return result;

    Report<FirmwareData> firmware;
    if ((result = read_report(ctx, firmware)) < 0)
        return result;

    std::cerr << firmware << '\n';
    std::cerr << &firmware.report_data.firmware_string[0] << '\n';

    EffectMode mode {};
    mode.channel_id = 0x00;
    mode.effect_id = 0xff;

    auto mode_report = make_report(kAsusSetModeReportId, mode);
    if ((result = write_report(ctx, mode_report)) < 0)
        return result;

    return RGBCTL_SUCCESS;
}

rgbctl_zone constexpr kZones[] = { { 4, "Mainboard LEDs" } };

auto AsusX570::on_rgb_data(rgbctl_device_context* ctx,
                           std::uint32_t zone_index,
                           rgbctl_rgb_value const* data,
                           std::uint32_t n) noexcept -> rgbctl_errno
{
    if (led_count_ < 0)
        return -RGBCTL_ERR_NOT_INITIALIZED;

    if (static_cast<std::size_t>(zone_index) >= std::size(kZones))
        return -RGBCTL_ERR_WRITE;

    auto num_to_write = std::min(kZones[zone_index].rgb_count, n);

    std::copy(data, data + num_to_write, std::begin(rgb_data_.rgbs));

    std::fill_n(std::begin(rgb_data_.rgbs) + num_to_write,
                static_cast<std::size_t>(n) - num_to_write,
                rgbctl_rgb_value {});

    rgb_data_.channel_rw = 0x80 | kAsusAuraDirectChannel;
    rgb_data_.start_led = 0;
    rgb_data_.led_count = static_cast<std::uint8_t>(n);

    auto rpt = make_report(kAsusControlDirectReportId, rgb_data_);
    rgbctl_errno result;
    if ((result = write_report(ctx, rpt)) < 0)
        return result;

    return static_cast<rgbctl_errno>(num_to_write);
}

auto AsusX570::on_query_zones(rgbctl_device_context*,
                              rgbctl_zone const** zones) noexcept
    -> rgbctl_errno
{
    *zones = kZones;
    return static_cast<rgbctl_errno>(std::size(kZones));
}

auto AsusX570::on_release(rgbctl_device_context*) noexcept -> void
{
    //
}

} // namespace rgbctl::modules::builtin::asus
