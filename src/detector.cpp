#include "rgbctl/detector.hpp"
#include <iostream>
#include <libudev.h>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace
{
using UDevContext = struct udev;
using UDevDevice = struct udev_device;
using UDevEnumerate = struct udev_enumerate;

struct NoI2CDevicesException : std::runtime_error
{
    NoI2CDevicesException()
        : std::runtime_error { "No i2c devices found" }
    {
    }
};

struct UDevContextDeleter
{
    auto operator()(UDevContext* ptr) const noexcept -> void
    {
        udev_unref(ptr);
    }
};

struct UDevDeviceDeleter
{
    auto operator()(UDevDevice* ptr) const noexcept -> void
    {
        udev_device_unref(ptr);
    }
};

struct UDevEnumerateDeleter
{
    auto operator()(UDevEnumerate* ptr) const noexcept -> void
    {
        udev_enumerate_unref(ptr);
    }
};

using UDevContextPtr = std::unique_ptr<UDevContext, UDevContextDeleter>;
using UDevDevicePtr = std::unique_ptr<UDevDevice, UDevDeviceDeleter>;
using UDevEnumeratePtr = std::unique_ptr<UDevEnumerate, UDevEnumerateDeleter>;

using UDevListEntry = struct udev_list_entry;

struct UDevListIterator
{
    UDevListIterator() noexcept
        : current_ { nullptr }
    {
    }

    UDevListIterator(UDevListEntry* entry) noexcept
        : current_ { entry }
    {
    }

    auto operator++() noexcept -> UDevListIterator&
    {
        current_ = udev_list_entry_get_next(current_);
        return *this;
    }

    auto operator++(int) noexcept -> UDevListIterator
    {
        auto tmp { *this };
        ++(*this);
        return tmp;
    }

    auto operator*() noexcept -> UDevListEntry& { return *current_; }

    friend auto operator==(UDevListIterator const& lhs,
                           UDevListIterator const& rhs) noexcept -> bool
    {
        return lhs.current_ == rhs.current_;
    }

    friend auto operator!=(UDevListIterator const& lhs,
                           UDevListIterator const& rhs) noexcept -> bool
    {
        return !(lhs == rhs);
    }

private:
    UDevListEntry* current_;
};

auto get_device(UDevContext* ctx, UDevListEntry& it) noexcept -> UDevDevicePtr
{
    return UDevDevicePtr { udev_device_new_from_syspath(
        ctx, udev_list_entry_get_name(&it)) };
}

struct UDevList
{
    explicit UDevList(UDevListEntry* item) noexcept
        : begin_ { UDevListIterator { item } }
        , end_ { UDevListIterator {} }
    {
    }

    auto begin() noexcept { return begin_; }

    auto end() noexcept { return end_; }

private:
    UDevListIterator begin_;
    UDevListIterator end_;
};

struct UDevDeviceScan
{
    explicit UDevDeviceScan(UDevContext* context,
                            std::string subsystem) noexcept
        : enumerate_ { udev_enumerate_new(context) }
    {
        udev_enumerate_add_match_subsystem(enumerate_.get(), subsystem.c_str());
        udev_enumerate_scan_devices(enumerate_.get());
    }

    explicit UDevDeviceScan(UDevContext* context,
                            char const* subsystem) noexcept
        : UDevDeviceScan { context, std::string { subsystem } }
    {
    }

    auto begin() const noexcept
    {
        return UDevListIterator { udev_enumerate_get_list_entry(
            enumerate_.get()) };
    }

    auto end() const noexcept { return UDevListIterator { nullptr }; }

private:
    UDevEnumeratePtr enumerate_;
};

template <typename Iterator, typename T, typename UnaryFunction>
auto split_at(Iterator first, Iterator last, T const& val, UnaryFunction f)
    -> void
{
    auto saved_first = first;
    while (saved_first != last) {
        auto delim_pos = std::find(saved_first, last, val);
        f(saved_first, delim_pos);
        if (delim_pos == last)
            break;

        saved_first = std::next(delim_pos);
    }

    f(saved_first, last);
}

template <typename Map>
auto parse_uevent(char const* str, Map& result) -> void
{
    std::string_view block { str };

    split_at(
        begin(block), end(block), '\n', [&](auto line_begin, auto line_end) {
            if (line_begin == line_end)
                return;

            std::array<std::string, 2> kv;
            auto kv_pos = begin(kv);
            split_at(
                line_begin, line_end, '=', [&](auto val_begin, auto val_end) {
                    if (kv_pos == end(kv) || val_begin == val_end)
                        return;

                    *kv_pos++ = std::string { val_begin, val_end };
                });

            if (kv_pos == end(kv))
                result[kv[0]] = kv[1];
        });
}

template <typename T>
auto parse_hex(std::string_view str, T& result) -> bool
{
    T val {};
    for (auto const& c : str) {
        if (c >= '0' && c <= '9') {
            val = val * 16 + static_cast<T>(c - '0');
        }
        else if (c >= 'a' && c <= 'f') {
            val = val * 16 + 10 + static_cast<T>(c - 'a');
        }
        else if (c >= 'A' && c <= 'F') {
            val = val * 16 + 10 + static_cast<T>(c - 'A');
        }
        else
            return false;
    }

    result = val;
    return true;
}

auto parse_device_id(std::string const& str,
                     std::uint32_t& vendor_id,
                     std::uint32_t& product_id) noexcept -> bool
{
    std::array<std::string_view, 3> parts {};
    auto part_pos = begin(parts);

    split_at(begin(str), end(str), ':', [&](auto val_first, auto val_last) {
        if (part_pos == end(parts) || val_first == val_last)
            return;

        *part_pos++ = std::string_view { val_first, val_last };
    });

    if (part_pos != end(parts))
        return false;

    if (!parse_hex(parts[1], vendor_id))
        return false;

    return parse_hex(parts[2], product_id);
}

struct DeviceDescriptor
{
    std::string device_name;
    std::string device_node;
    std::string product_name;
    std::uint16_t bus_id;
    std::uint32_t vendor_id;
    std::uint32_t product_id;
};

} // namespace

namespace rgbctl::detail
{

auto detect(auto (*cb)(DetectedDevice, void*)->void, void* caller_data) -> void
{
    UDevContextPtr udev { udev_new() };
    UDevDeviceScan scan { udev.get(), "hidraw" };
    std::vector<DeviceDescriptor> devices;

    for (auto& item : scan) {
        auto device = get_device(udev.get(), item);

        if (!device)
            throw std::system_error { errno, std::system_category() };

        UDevDevice* parent = udev_device_get_parent_with_subsystem_devtype(
            device.get(), "hid", nullptr);

        char const* devname =
            udev_device_get_property_value(device.get(), "DEVNAME");

        std::unordered_map<std::string, std::string> uevent_map;
        char const* uevent;
        if (parent &&
            (uevent = udev_device_get_sysattr_value(parent, "uevent"))) {

            parse_uevent(uevent, uevent_map);
        }

        DetectedDevice detected_device { .product_id = { .vendor_id = 0,
                                                         .product_id = 0 },
                                         .device_path = devname };

        parse_device_id(uevent_map["HID_ID"],
                        detected_device.product_id.vendor_id,
                        detected_device.product_id.product_id);

        cb(std::move(detected_device), caller_data);
    }
}

} // namespace rgbctl::detail
