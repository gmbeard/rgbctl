#ifndef RGBCTL_BUILTINS_ASUS_ASUS_X570_HPP_INCLUDED
#define RGBCTL_BUILTINS_ASUS_ASUS_X570_HPP_INCLUDED

#include "../base_module.hpp"
#include "rgbctl/rgbctl.h"
#include "rgbctl/utils.hpp"
#include <array>
#include <type_traits>
#include <vector>

namespace rgbctl::modules::builtin::asus
{

std::uint8_t constexpr kAsusAuraMagicNumber = 0xec;
std::size_t constexpr kAsusAuraReportSize = 65;

std::uint8_t constexpr kAsusAuraConfigReportId = 0xb0;
std::uint8_t constexpr kAsusAuraFirmwareReportId = 0x82;
std::uint8_t constexpr kAsusControlDirectReportId = 0x40;
std::uint8_t constexpr kAsusSetModeReportId = 0x35;

std::uint8_t constexpr kAsusAuraDirectChannel = 0x04;

struct RawData
{
    std::array<std::uint8_t, kAsusAuraReportSize - 2> raw;
};

struct FirmwareData
{
    std::array<char, kAsusAuraReportSize - 2> firmware_string;
};

struct ConfigData
{
    std::array<std::uint8_t, 4> RGBCTL_UNUSED();
    std::uint8_t channel_count;
    std::uint8_t RGBCTL_UNUSED();
    std::array<std::uint8_t, 24> RGBCTL_UNUSED();
    std::uint8_t led_count;
    std::uint8_t RGBCTL_UNUSED();
    std::uint8_t rgb_header_count;
    std::array<std::uint8_t, 30> RGBCTL_UNUSED();
};

struct RgbData
{
    std::uint8_t channel_rw;
    std::uint8_t start_led;
    std::uint8_t led_count;
    rgbctl_rgb_value rgbs[20];
};

struct EffectMode
{
    std::uint8_t channel_id;
    std::uint8_t RGBCTL_UNUSED();
    std::uint8_t RGBCTL_UNUSED();
    std::uint8_t effect_id;
    std::array<std::uint8_t, 59> RGBCTL_UNUSED();
};

template <typename ReportData>
struct Report
{
    Report() noexcept
        : magic_number { 0 }
        , report_id { 0 }
        , report_data { ReportData {} }
    { }

    Report(std::uint8_t id, ReportData data) noexcept
        : magic_number { kAsusAuraMagicNumber }
        , report_id { id }
        , report_data { data }
    { }

    std::uint8_t magic_number;
    std::uint8_t report_id;
    ReportData report_data;

    static_assert(sizeof(ReportData) + sizeof(magic_number) + sizeof(report_id)
                      == kAsusAuraReportSize,
                  "Incorrect report size");

    static_assert(std::is_standard_layout_v<ReportData>,
                  "Must be standard layout");
};

template <typename ReportData>
auto make_report(std::uint8_t id, ReportData data) noexcept
{
    return Report<ReportData> { id, data };
}

struct AsusX570 : BaseModule<AsusX570>
{
    static rgbctl_product_id constexpr product_id = { 0x0B05, 0x18F3 };

    auto on_acquire(rgbctl_device_context* cxt) noexcept -> rgbctl_errno;

    auto on_rgb_data(rgbctl_device_context*,
                     std::uint32_t,
                     rgbctl_rgb_value const*,
                     std::uint32_t) noexcept -> rgbctl_errno;

    auto on_query_zones(rgbctl_device_context*, rgbctl_zone const**) noexcept
        -> rgbctl_errno;

    auto on_release(rgbctl_device_context*) noexcept -> void;

private:
    std::int32_t led_count_ = -1;
    RgbData rgb_data_ {};
};

} // namespace rgbctl::modules::builtin::asus

#endif // RGBCTL_BUILTINS_ASUS_ASUS_X570_HPP_INCLUDED
