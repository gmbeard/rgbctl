#include "rgbctl/rgb.hpp"
#include "testing.hpp"
#include <array>
#include <cassert>
#include <iterator>
#include <span>
#include <vector>

using rgbctl::hex_string_to_rgb_float;
using rgbctl::RgbFloat;

namespace rgbctl
{

template <typename InputIt>
struct ResolutionScaler
{
    ResolutionScaler(InputIt first,
                     InputIt last,
                     std::size_t target_size) noexcept
        : input_first_ { first }
        , input_last_ { last }
        , target_size_ { target_size }
        , factor_ { static_cast<float>(std::distance(input_first_, input_last_))
                    / static_cast<float>(target_size_) }
    {
        assert(target_size_);
    }

    struct ScalingIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type
            = std::remove_reference_t<decltype(*std::declval<InputIt>())>;
        using pointer = value_type*;
        using reference = value_type&;

        auto operator++() noexcept -> ScalingIterator&
        {
            ++current_;
            return *this;
        }

        auto operator++(int) noexcept -> ScalingIterator
        {
            auto tmp { *this };
            ++(*this);
            return tmp;
        }

        auto operator*() noexcept
            -> std::remove_reference_t<decltype(*std::declval<InputIt>())>&
        {
            auto pos = std::next(parent_->input_first_,
                                 static_cast<std::ptrdiff_t>(
                                     float(current_) * parent_->factor_));
            assert(pos != parent_->input_last_);
            return *pos;
        }

        auto operator->() noexcept
            -> std::remove_reference_t<decltype(*std::declval<InputIt>())>*
        {
            return &*(*this);
        }

        friend auto operator==(ScalingIterator const& lhs,
                               ScalingIterator const& rhs) noexcept
        {
            return lhs.current_ == rhs.current_;
        }

        friend auto operator!=(ScalingIterator const& lhs,
                               ScalingIterator const& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        ResolutionScaler* parent_;
        std::size_t current_;
    };

    auto begin() noexcept -> ScalingIterator
    {
        return { this, 0 };
    }

    auto end() noexcept -> ScalingIterator
    {
        return { this, target_size_ };
    }

private:
    InputIt input_first_;
    InputIt input_last_;
    std::size_t target_size_;
    float factor_;
};

template <typename InputIt>
auto resolution_scaler(InputIt first, InputIt last, std::size_t target_size)
{
    return ResolutionScaler<InputIt> { first, last, target_size };
}

} // namespace rgbctl

auto should_scale_down() -> void
{
    std::array<RgbFloat, 8> from {};
    auto pos = begin(from);

    EXPECT(hex_string_to_rgb_float("ff0000", *pos++));
    EXPECT(hex_string_to_rgb_float("ff0000", *pos++));
    EXPECT(hex_string_to_rgb_float("00ff00", *pos++));
    EXPECT(hex_string_to_rgb_float("00ff00", *pos++));
    EXPECT(hex_string_to_rgb_float("0000ff", *pos++));
    EXPECT(hex_string_to_rgb_float("0000ff", *pos++));
    EXPECT(hex_string_to_rgb_float("ff0000", *pos++));
    EXPECT(hex_string_to_rgb_float("00ff00", *pos++));

    std::vector<RgbFloat> to;

    auto scaler = rgbctl::resolution_scaler(begin(from), pos, 3);

    std::copy(std::begin(scaler), std::end(scaler), std::back_inserter(to));

    EXPECT(to.size() == 3);
}

auto should_scale_up() -> void
{
    std::array<RgbFloat, 1> from {};
    auto pos = begin(from);

    EXPECT(hex_string_to_rgb_float("ff0000", *pos++));

    std::vector<RgbFloat> to;

    auto scaler = rgbctl::resolution_scaler(begin(from), pos, 16);

    std::copy(std::begin(scaler), std::end(scaler), std::back_inserter(to));

    EXPECT(to.size() == 16);
}

auto main() -> int
{
    return rgbctl::testing::run({
        TEST(should_scale_down),
        TEST(should_scale_up),
    });
}
