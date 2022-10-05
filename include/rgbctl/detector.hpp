#ifndef RGBCTL_DETECTOR_HPP_INCLUDED
#define RGBCTL_DETECTOR_HPP_INCLUDED

#include "./detected_device.hpp"

namespace rgbctl
{

namespace detail
{

template <typename Container>
auto push_result(DetectedDevice d, void* c) -> void
{
    reinterpret_cast<Container*>(c)->push_back(std::move(d));
}

auto detect(auto (*)(DetectedDevice, void*)->void, void*) -> void;

} // namespace detail

template <typename Container>
auto detect(Container& c) -> void
{
    detail::detect(detail::push_result<Container>, &c);
}

} // namespace rgbctl

#endif // RGBCTL_DETECTOR_HPP_INCLUDED
