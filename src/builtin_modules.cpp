#include "./builtin_modules.hpp"
#include "./builtins/builtins.hpp"
#include "rgbctl/rgbctl.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <limits>

namespace
{

using namespace rgbctl::modules::builtin;

using ModuleAcquire
    = auto (*)(rgbctl_device_context*, rgbctl_module_acquisition*) noexcept
      -> rgbctl_errno;

using AcquisitionEntry = std::tuple<rgbctl_product_id, ModuleAcquire>;

// clang-format off
rgbctl_product_id constexpr builtin_products[] = {
    asus::AsusX570::product_id,
    corsair::CorsairH100iProXt::product_id,
};

AcquisitionEntry constexpr builtin_aquisition_table[] = {
    { asus::AsusX570::product_id, asus::AsusX570::acquire },
    { corsair::CorsairH100iProXt::product_id, corsair::CorsairH100iProXt::acquire }
};
// clang-format on

static_assert(std::size(builtin_products)
                  == std::size(builtin_aquisition_table),
              "number of product IDs advertised doesn't match number "
              "of items in acquisition table!");

static_assert(std::size(builtin_products)
                  <= std::numeric_limits<std::uint32_t>::max(),
              "Number of products would overflow numberic limits");

auto acquire(rgbctl_device_context* ctx,
             rgbctl_product_id id,
             rgbctl_module_acquisition* acquisition,
             void*) noexcept -> rgbctl_errno
{
    auto it = std::find_if(
        std::begin(builtin_aquisition_table),
        std::end(builtin_aquisition_table),
        [&](auto const& item) { return std::get<0>(item) == id; });

    if (it == std::end(builtin_aquisition_table))
        return -RGBCTL_ERR_NOT_IMPLEMENTED;

    return std::get<1>(*it)(ctx, acquisition);
}

} // namespace

auto rgbctl::modules::init(rgbctl_module_registration* module_out) noexcept
    -> rgbctl_errno
{
    *module_out = { .products = builtin_products,
                    .product_count
                    = static_cast<std::uint32_t>(std::size(builtin_products)),
                    .acquire_callback = acquire,
                    .user_data = nullptr };

    return RGBCTL_SUCCESS;
}
