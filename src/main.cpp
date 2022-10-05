#include "./builtin_modules.hpp"
#include "./builtins/asus/asus_x570.hpp"
#include "./builtins/corsair/corsair_h100i_pro_xt.hpp"
#include "./fixed_config.hpp"
#include "rgbctl/rgbctl.hpp"
#include <iostream>
#include <libtcc.h>
#include <stdexcept>
#include <tuple>
#include <vector>

using Mod = std::tuple<rgbctl_product_id, rgbctl_module_acquisition_callback>;
using RegisteredModules = std::vector<Mod>;
using Devices = std::vector<rgbctl::DetectedDevice>;

auto create_rotate_effect(std::uint32_t zone_index) -> rgbctl::effects::Rotate
{
    // std::array<rgbctl::RgbFloat, 32> inputs {};
    // auto rgb = inputs.begin();
    // hex_string_to_rgb_float("000000", *rgb++);
    // hex_string_to_rgb_float("000000", *rgb++);
    // hex_string_to_rgb_float("070050", *rgb++);
    // hex_string_to_rgb_float("3f00ff", *rgb++);

    auto texture_config = rgbctl::fixed_texture_config();

    return rgbctl::effects::Rotate { zone_index,
                                     5000,
                                     { data(texture_config.data),
                                       texture_config.data.size() } };
}

auto create_linear_effect(std::uint32_t zone_index) -> rgbctl::effects::Linear
{
    std::array<rgbctl::RgbFloat, 3> inputs {};

    auto rgb = inputs.begin();
    for (; rgb != std::next(std::begin(inputs), inputs.size() / 3); ++rgb)
        hex_string_to_rgb_float("ff0000", *rgb);

    for (; rgb != std::next(std::begin(inputs), (inputs.size() / 3) * 2); ++rgb)
        hex_string_to_rgb_float("00ff00", *rgb);

    for (; rgb != std::end(inputs); ++rgb)
        hex_string_to_rgb_float("0000ff", *rgb);

    return rgbctl::effects::Linear {
        zone_index,
        inputs.size() / 3,
        60000,
        { inputs.data(), static_cast<std::size_t>(rgb - inputs.begin()) }
    };
}

template <typename Effect>
auto create_controller(rgbctl_product_id id,
                       Effect&& effect,
                       RegisteredModules const& registered_modules,
                       Devices const& devices) -> rgbctl::AnyController
{
    auto mod_pos = std::find_if(
        registered_modules.begin(),
        registered_modules.end(),
        [&](auto const& item) { return std::get<0>(item) == id; });

    auto device_pos
        = std::find_if(devices.begin(), devices.end(), [&](auto const& item) {
              return item.product_id == id;
          });

    if (mod_pos == registered_modules.end() || device_pos == devices.end())
        throw std::runtime_error { "app: match device to module" };

    rgbctl::DeviceContext<rgbctl::RawDeviceStream> ctx {
        rgbctl::RawDeviceStream { device_pos->device_path }
    };

    return rgbctl::make_controller(std::move(ctx),
                                   std::move(effect),
                                   std::get<0>(*mod_pos),
                                   std::get<1>(*mod_pos));
}

auto app() -> void
{
    rgbctl_module_registration reg {};
    if (rgbctl::modules::init(&reg) != RGBCTL_SUCCESS)
        throw std::runtime_error { "app: init modules" };

    RegisteredModules registered_modules;
    while (reg.product_count--)
        registered_modules.push_back({ *reg.products++, reg.acquire_callback });

    Devices devices;
    rgbctl::detect(devices);

    using rgbctl::modules::builtin::asus::AsusX570;
    using rgbctl::modules::builtin::corsair::CorsairH100iProXt;

    std::vector<rgbctl::AnyController> controllers;
    controllers.push_back(create_controller(AsusX570::product_id,
                                            create_rotate_effect(0),
                                            registered_modules,
                                            devices));
    controllers.push_back(create_controller(CorsairH100iProXt::product_id,
                                            create_rotate_effect(1),
                                            registered_modules,
                                            devices));

    rgbctl::loop(33, [&](auto elapsed) {
        for (auto& ctrl : controllers)
            ctrl.tick(elapsed);

        return true;
    });

    std::cerr << "Exited loop\n";
}

auto main() -> int
{
    try {
        app();
        return 0;
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 1;
}
