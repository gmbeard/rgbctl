#ifndef RGBCTL_ASSERT_HPP_INCLUDED
#define RGBCTL_ASSERT_HPP_INCLUDED

#ifndef RGBCTL_STRINGIFY
    #define RGBCTL_STRINGIFY_IMPL(x) #x
    #define RGBCTL_STRINGIFY(x)      RGBCTL_STRINGIFY_IMPL(x)
#endif

#define RGBCTL_LIKELY(cond) __builtin_expect(!!(cond), 1)

#define RGBCTL_CHECK_CONDITION(cond, type)                                     \
    RGBCTL_LIKELY(cond)                                                        \
    ? static_cast<void>(0)                                                     \
    : ::rgbctl::condition_failure(                                             \
        RGBCTL_STRINGIFY(cond) " in " __FILE__ ":" RGBCTL_STRINGIFY(__LINE__), \
        type)

#define RGBCTL_EXPECTS(cond) RGBCTL_CHECK_CONDITION(cond, "Pre-condition")
#define RGBCTL_ENSURES(cond) RGBCTL_CHECK_CONDITION(cond, "Post-condition")

namespace rgbctl
{

[[noreturn]] auto condition_failure(char const* /*condition*/,
                                    char const* /*type*/) noexcept -> void;

} // namespace rgbctl

#endif // RGBCTL_ASSERT_HPP_INCLUDED
