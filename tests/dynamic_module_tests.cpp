#include "testing.hpp"
#include <cstddef>
#include <libtcc.h>
#include <memory>
#include <vector>

struct TCCStateDeleter
{
    auto operator()(TCCState* state) const noexcept -> void
    {
        tcc_delete(state);
    }
};

using TCCStatePtr = std::unique_ptr<TCCState, TCCStateDeleter>;

TEST_WITH_CONTEXT(should_compile_module)
{
    EXPECT(test_context.argc == 1);

    TCCStatePtr compiler_ctx { tcc_new() };
    EXPECT(compiler_ctx);

    EXPECT(tcc_set_output_type(compiler_ctx.get(), TCC_OUTPUT_MEMORY) >= 0);
    EXPECT(tcc_add_file(compiler_ctx.get(), *test_context.argv) >= 0);

    auto const bin_size = tcc_relocate(compiler_ctx.get(), nullptr);
    EXPECT(bin_size > 0);

    std::vector<char> memory(static_cast<std::size_t>(bin_size));

    EXPECT(tcc_relocate(compiler_ctx.get(), memory.data()) >= 0);

    auto* sym = reinterpret_cast<auto (*)(int, char**)->int>(
        tcc_get_symbol(compiler_ctx.get(), "main"));

    EXPECT(sym);

    EXPECT(sym(0, nullptr) == 0);
}

auto main(int argc, char const** argv) -> int
{
    return rgbctl::testing::run_with_context(
        argc, argv, { TEST(should_compile_module) });
}
