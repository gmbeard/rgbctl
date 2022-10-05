#include "./corsair_h100i_pro_xt.hpp"
#include "./corsair_utils.hpp"
#include "rgbctl/assert.hpp"
#include <array>
#include <cassert>
#include <cstring>
#include <iterator>
#include <tuple>

/* NOTES:
 * Initialization must send the following sequence...
 * - Send magic number 1
 * - Send magic number 2
 * - Send magic number 3
 *
 * Each of the magic number reports will result in the
 * firmware being reported.
 *
 * Doesn't support numbered reports, so the first byte
 * of every report written should be 0x00.
 *
 * Every report written must have a CRC as the last
 * byte.
 *
 * Each report needs a monotonic sequence number. The
 * sequence number starts a 1 and cycles after 31. The
 * sequence number must be placed in the upper 5 bits of
 * the command field (i.e. `sequence_num << 3 | command`).
 *
 * Must perform a read after every write. OpenRGB seems
 * to drop the data from the read on the floor but liquidctl
 * actually checks the CRC from the result. We should
 * probably report failure if the returned CRC doesn't match.
 *
 * OpenRGB appears to add a 5ms delay after every read
 * to "prevent[s] the AIO from soft-locking when using EE",
 * though I don't see liquidctl doing this. Maybe this _is_
 * required if we want effects.
 */

#include <iomanip>
#include <iostream>
namespace
{
template <typename ReportType>
decltype(auto)
operator<<(std::ostream& os,
           rgbctl::modules::builtin::corsair::Report<ReportType> const& val)
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

decltype(auto)
operator<<(std::ostream& os,
           rgbctl::modules::builtin::corsair::Response const& val)
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

    os << '\n';
    os << "SEQ:         " << std::setw(2) << std::uint32_t(val.sequence >> 3)
       << '\n'
       << "FW_1:        " << std::setw(2) << std::uint32_t(val.firmware_1)
       << '\n'
       << "FW_2:        " << std::setw(2) << std::uint32_t(val.firmware_2)
       << '\n'
       << "CNT_LSB:     " << std::setw(2) << std::uint32_t(val.counter_lsb)
       << '\n'
       << "CNT_MSB:     " << std::setw(2) << std::uint32_t(val.counter_msb)
       << '\n'
       << "LIQUID_LSB:  " << std::setw(2) << std::uint32_t(val.liquid_temp_lsb)
       << '\n'
       << "LIQUID_MSB:  " << std::setw(2) << std::uint32_t(val.liquid_temp_msb)
       << '\n'
       << "FAN_1_LSB:   " << std::setw(2) << std::uint32_t(val.fan_1_speed_lsb)
       << '\n'
       << "FAN_1_MSB:   " << std::setw(2) << std::uint32_t(val.fan_1_speed_msb)
       << '\n'
       << "FAN_2_LSB:   " << std::setw(2) << std::uint32_t(val.fan_2_speed_lsb)
       << '\n'
       << "FAN_2_MSB:   " << std::setw(2) << std::uint32_t(val.fan_2_speed_msb)
       << '\n'
       << "PUMP_LSB:    " << std::setw(2) << std::uint32_t(val.pump_speed_lsb)
       << '\n'
       << "PUMP_MSB:    " << std::setw(2) << std::uint32_t(val.pump_speed_msb)
       << '\n'
       << "CHECKSUM:    " << std::setw(2) << std::uint32_t(val.checksum);

    return os;
}

} // namespace
namespace rgbctl::modules::builtin::corsair
{

struct RawReportData
{
    std::array<std::uint8_t, 61> raw {};
};

struct ColourData
{
    std::array<rgbctl_rgb_value, 20> rgbs {};
    std::uint8_t RGBCTL_UNUSED();
};

auto CorsairH100iProXt::on_acquire(rgbctl_device_context* ctx) noexcept
    -> rgbctl_errno
{
    Report<RawReportData> rpt {};
    Response rpt_out {};
    rgbctl_errno result;

    std::fill(begin(rgb_data_), end(rgb_data_), rgbctl_rgb_value { 0, 0, 0 });

    std::cerr << "Acquiring...\n";

    rpt.sequence_command = 0x01;
    std::copy(std::begin(kMagicNumber1),
              std::end(kMagicNumber1),
              rpt.report_data.raw.begin());
    if ((result = write_report_with_result(ctx, rpt, rpt_out)) < 0)
        return result;

    sequence_number_ = (((rpt_out.sequence & 0xf8) >> 3) % 31) + 2;
    std::cerr << rpt_out << "\n\n";

    rpt.sequence_command = 0x02;
    std::copy(std::begin(kMagicNumber2),
              std::end(kMagicNumber2),
              rpt.report_data.raw.begin());
    if ((result = write_report_with_result(ctx, rpt, rpt_out)) < 0)
        return result;

    sequence_number_ = (((rpt_out.sequence & 0xf8) >> 3) % 31) + 2;
    std::cerr << rpt_out << "\n\n";

    rpt.sequence_command = 0x03;
    std::copy(std::begin(kMagicNumber3),
              std::end(kMagicNumber3),
              rpt.report_data.raw.begin());
    if ((result = write_report_with_result(ctx, rpt, rpt_out)) < 0)
        return result;

    sequence_number_ = (((rpt_out.sequence & 0xf8) >> 3) % 31) + 2;
    std::cerr << rpt_out << "\n\n";

    std::cerr << "...Acquiring done\n";
    return RGBCTL_SUCCESS;
}

struct ZoneMap
{
    std::size_t const* data;
    std::size_t len;

    constexpr auto size() const noexcept
    {
        return len;
    }

    constexpr auto begin() const noexcept
    {
        return data;
    }

    constexpr auto end() const noexcept
    {
        return data + len;
    }
};

template <std::size_t N>
struct ZoneMapIterator
{
    using category = std::forward_iterator_tag;
    using distance_type = std::ptrdiff_t;

    ZoneMapIterator(ZoneMap map,
                    std::array<rgbctl_rgb_value, N>& output) noexcept
        : map_iter_ { map.begin() }
        , map_iter_end_ { map.end() }
        , output_ { output }
    { }

    auto operator*() -> rgbctl_rgb_value&
    {
        RGBCTL_EXPECTS(map_iter_ != map_iter_end_);
        RGBCTL_EXPECTS(*map_iter_ < output_.size());
        return output_[*map_iter_];
    }

    auto operator->() -> rgbctl_rgb_value*
    {
        RGBCTL_EXPECTS(map_iter_ != map_iter_end_);
        RGBCTL_EXPECTS(*map_iter_ < output_.size());
        return &output_[*map_iter_];
    }

    auto operator++() -> ZoneMapIterator&
    {
        RGBCTL_EXPECTS(map_iter_ != map_iter_end_);
        ++map_iter_;
        return *this;
    }

private:
    decltype(std::declval<ZoneMap>().begin()) map_iter_;
    decltype(std::declval<ZoneMap>().begin()) map_iter_end_;
    std::array<rgbctl_rgb_value, N>& output_;
};

template <std::size_t N>
auto zone_map_iterator(ZoneMap map,
                       std::array<rgbctl_rgb_value, N>& output) noexcept
{
    return ZoneMapIterator<N> { map, output };
}

rgbctl_zone constexpr kZones[] = { { 4, "Center" }, { 12, "Ring" } };

std::array<std::size_t, 4> constexpr kCenterZoneMap = { 0, 1, 2, 3 };
std::array<std::size_t, 12> constexpr kRingZoneMap
    = { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

std::array<ZoneMap, 2> constexpr kZoneMaps
    = { { { &kCenterZoneMap[0], std::size(kCenterZoneMap) },
          { &kRingZoneMap[0], std::size(kRingZoneMap) } } };

static_assert(std::size(kZones) == std::size(kZoneMaps),
              "size(kZones) != size(kZoneMaps)");

auto CorsairH100iProXt::on_rgb_data(rgbctl_device_context* ctx,
                                    std::uint32_t zone_index,
                                    rgbctl_rgb_value const* data,
                                    uint32_t n) noexcept -> rgbctl_errno
{
    if (zone_index >= std::size(kZones))
        return -RGBCTL_ERR_WRITE;

    auto const rgb_count
        = static_cast<std::size_t>(std::min(n, kZones[zone_index].rgb_count));

    std::transform(
        data,
        data + rgb_count,
        zone_map_iterator(kZoneMaps[zone_index], rgb_data_),
        [](auto const item) {
            return rgbctl_rgb_value { item.blue, item.green, item.red };
        });

    Report<ColourData> rpt {};
    Response rpt_out {};

    rpt.sequence_command = 0x04;

    /* Red and blue channels are swapped on this device...
     */
    std::copy_n(begin(rgb_data_),
                std::min(std::size_t { n }, std::size(rgb_data_)),
                std::begin(rpt.report_data.rgbs));

    auto result = write_report_with_result(ctx, rpt, rpt_out);
    if (result < 0)
        return result;

    return result;
}

auto CorsairH100iProXt::on_query_zones(rgbctl_device_context*,
                                       rgbctl_zone const** zones) noexcept
    -> rgbctl_errno
{
    *zones = kZones;
    return static_cast<rgbctl_errno>(std::size(kZones));
}

auto CorsairH100iProXt::on_release(rgbctl_device_context*) noexcept -> void
{ }

auto CorsairH100iProXt::next_sequence_number() noexcept -> std::uint8_t
{
    std::uint8_t constexpr kMaxSequenceNumber = 31;

    auto const val = sequence_number_++;
    if (sequence_number_ > kMaxSequenceNumber)
        sequence_number_ = 1;

    return static_cast<std::uint8_t>(val << 3);
}

template <typename ReportData>
auto CorsairH100iProXt::write_report_with_result(rgbctl_device_context* ctx,
                                                 Report<ReportData>& rpt,
                                                 Response& out_rpt)
    -> rgbctl_errno
{
    // using namespace std::chrono_literals;
    // constexpr auto kDelay = 5ms;

    rpt.report_number = 0x00;
    rpt.prefix = 0x3f;
    rpt.sequence_command = next_sequence_number() | rpt.sequence_command;

    auto checksum = compute_checksum(
        reinterpret_cast<unsigned char const*>(&rpt) + 2,
        reinterpret_cast<unsigned char const*>(&rpt) + sizeof(rpt) - 1);

    rpt.checksum = checksum;

    auto result = write_report(ctx, rpt);
    if (result < 0)
        return result;

    if ((result = read_report(ctx, out_rpt)) < 0)
        return result;

    // std::this_thread::sleep_for(kDelay);

    return result;
}

} // namespace rgbctl::modules::builtin::corsair
