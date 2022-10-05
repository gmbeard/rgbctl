#include "rgbctl/acquire.hpp"
using namespace rgbctl;

Module::Module(rgbctl_module_acquisition acquisition) noexcept
    : acquisition_ { acquisition }
{
}

Module::Module(Module&& other) noexcept
    : acquisition_ { std::exchange(other.acquisition_,
                                   rgbctl_module_acquisition {}) }
{
}

Module::~Module()
{
    if (acquisition_.module)
        shutdown();
}

auto Module::operator=(Module&& lhs) noexcept -> Module&
{
    auto tmp { std::move(lhs) };
    swap(*this, tmp);
    return *this;
}

auto rgbctl::swap(Module& lhs, Module& rhs) noexcept -> void
{
    using std::swap;
    swap(lhs.acquisition_, rhs.acquisition_);
}

auto Module::shutdown() noexcept -> void
{
    acquisition_.module->on_shutdown(acquisition_.user_data);
}

Module::operator bool() const noexcept
{
    return acquisition_.module != nullptr;
}
