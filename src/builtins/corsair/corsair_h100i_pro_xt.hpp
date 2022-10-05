#ifndef RGBCTL_BUILTINS_CORSAIR_CORSAIR_H100I_PRO_XT_HPP_INCLUDED
#define RGBCTL_BUILTINS_CORSAIR_CORSAIR_H100I_PRO_XT_HPP_INCLUDED

#include "../base_module.hpp"
#include "./corsair_utils.hpp"
#include "rgbctl/rgbctl.h"
#include "rgbctl/utils.hpp"
#include <array>
#include <stdexcept>

namespace rgbctl::modules::builtin::corsair
{

template <typename ReportData>
struct Report
{
    std::uint8_t report_number = 0x00;
    std::uint8_t prefix = 0x3f;
    std::uint8_t sequence_command = 0x00;
    ReportData report_data;
    std::uint8_t checksum = 0x00;

    static_assert(sizeof(ReportData) == 61, "Incorrect report size");
};

struct Response
{
    std::uint8_t prefix;
    std::uint8_t sequence;
    std::uint8_t firmware_1;
    std::uint8_t firmware_2;
    std::uint8_t RGBCTL_UNUSED();
    std::uint8_t counter_lsb;
    std::uint8_t counter_msb;
    std::uint8_t liquid_temp_lsb;
    std::uint8_t liquid_temp_msb;
    std::array<uint8_t, 6> RGBCTL_UNUSED();
    std::uint8_t fan_1_speed_lsb;
    std::uint8_t fan_1_speed_msb;
    std::array<uint8_t, 5> RGBCTL_UNUSED();
    std::uint8_t fan_2_speed_lsb;
    std::uint8_t fan_2_speed_msb;
    std::array<uint8_t, 5> RGBCTL_UNUSED();
    std::uint8_t pump_speed_lsb;
    std::uint8_t pump_speed_msb;
    std::array<uint8_t, 32> RGBCTL_UNUSED();
    std::uint8_t checksum;
};

static_assert(sizeof(Response) == 64, "Incorrect report size");

struct CorsairH100iProXt : BaseModule<CorsairH100iProXt>
{
    static rgbctl_product_id constexpr product_id = { 0x1B1C, 0x0C20 };

    auto on_acquire(rgbctl_device_context*) noexcept -> rgbctl_errno;

    auto on_rgb_data(rgbctl_device_context*,
                     std::uint32_t,
                     rgbctl_rgb_value const*,
                     std::uint32_t) noexcept -> rgbctl_errno;

    auto on_query_zones(rgbctl_device_context*, rgbctl_zone const**) noexcept
        -> rgbctl_errno;

    auto on_release(rgbctl_device_context*) noexcept -> void;

private:
    auto next_sequence_number() noexcept -> std::uint8_t;

    template <typename ReportData>
    auto write_report_with_result(rgbctl_device_context* ctx,
                                  Report<ReportData>& rpt,
                                  Response& out_rpt) -> rgbctl_errno;

private:
    std::uint8_t sequence_number_ = 1;
    std::array<rgbctl_rgb_value, 17> rgb_data_;
};

} // namespace rgbctl::modules::builtin::corsair

#endif // RGBCTL_BUILTINS_CORSAIR_CORSAIR_H100I_PRO_XT_HPP_INCLUDED
