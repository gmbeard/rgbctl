#include "./mock_read_write_stream.hpp"
#include "rgbctl/rgbctl.hpp"
#include "testing.hpp"
#include <array>
#include <dlfcn.h>
#include <iostream>
#include <memory>

struct DlDeleter
{
    auto operator()(void* handle) const noexcept -> void
    {
        dlclose(handle);
    }
};

using DlPtr = std::unique_ptr<void, DlDeleter>;
using Init = auto (*)(rgbctl_module_registration*) -> rgbctl_errno;

auto should_load_plugin()
{
    DlPtr handle { dlopen("/home/greg/.local/tmp/test_plugin.so", RTLD_LAZY) };
    if (!handle.get())
        std::cerr << dlerror() << '\n';

    EXPECT(handle.get());

    Init init = reinterpret_cast<Init>(dlsym(handle.get(), "init"));
    EXPECT(init);

    rgbctl_module_registration reg {};
    EXPECT(init(&reg) == RGBCTL_SUCCESS);

    std::array<unsigned char, 16> write_buffer {};
    std::array<unsigned char const, 16> read_buffer {};

    MockReadWriteStream stream { { read_buffer.data(), read_buffer.size() },
                                 { write_buffer.data(), write_buffer.size() } };
    rgbctl::DeviceContext<MockReadWriteStream> ctx { std::move(stream) };

    auto mod
        = rgbctl::acquire_module(ctx, reg.products[0], reg.acquire_callback);

    rgbctl_rgb_value const kData[] = { { 0, 0, 0 } };
    mod.send_rgb_data(
        ctx, 0, kData, rgbctl::narrow_cast<std::uint32_t>(std::size(kData)));
}

auto main() -> int
{
    return rgbctl::testing::run({
        IGNORE(should_load_plugin, "external module"),
    });
}
