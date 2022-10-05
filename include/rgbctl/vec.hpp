#ifndef RGBCTL_VEC_HPP_INCLUDED
#define RGBCTL_VEC_HPP_INCLUDED

#include <array>
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace rgbctl
{

template <typename T, std::size_t N>
struct Vec
{
    using value_type = T;
    using reference = T&;
    using const_reference = T const&;

    constexpr Vec() noexcept = default;

    template <typename... U>
    constexpr Vec(U&&... args) noexcept
        requires(sizeof...(U) == N && (std::is_convertible_v<U, T> && ...))
        : elements_ { std::forward<U>(args)... }
    { }

    auto constexpr size() const noexcept -> std::size_t
    {
        return N;
    }

    auto constexpr begin() const noexcept
    {
        return elements_.begin();
    }

    auto constexpr end() const noexcept
    {
        return elements_.end();
    }

    auto constexpr begin() noexcept
    {
        return elements_.begin();
    }

    auto constexpr end() noexcept
    {
        return elements_.end();
    }

    auto constexpr data() noexcept
    {
        return elements_.data();
    }

    auto constexpr data() const noexcept
    {
        return elements_.data();
    }

    auto constexpr operator[](std::size_t n) noexcept -> reference
    {
        return elements_[n];
    }

    auto constexpr operator[](std::size_t n) const noexcept -> const_reference
    {
        return elements_[n];
    }

    auto constexpr operator-=(Vec const& rhs) noexcept -> Vec&
    {
        auto rhs_pos = rhs.begin();
        for (auto& e : elements_)
            e = e - *rhs_pos++;

        return *this;
    }

    auto constexpr operator-(Vec const& rhs) noexcept -> Vec
    {
        auto ret_val { *this };
        return ret_val -= rhs;
    }

    auto constexpr operator*=(Vec const& rhs) noexcept -> Vec&
    {
        auto rhs_pos = rhs.begin();
        for (auto& e : elements_)
            e = *rhs_pos++ * e;

        return *this;
    }

    auto constexpr operator*=(T const& rhs) noexcept -> Vec&
    {
        for (auto& e : elements_)
            e = rhs * e;

        return *this;
    }

    auto constexpr operator*(T const& val) const noexcept -> Vec
    {
        auto ret_val { *this };
        return ret_val *= val;
    }

    auto constexpr operator*(Vec const& val) const noexcept -> Vec
    {
        auto ret_val { *this };
        return ret_val *= val;
    }

    auto constexpr operator/=(T const& rhs) noexcept -> Vec&
    {
        for (auto& e : elements_)
            e = rhs / e;

        return *this;
    }

    auto constexpr operator/(T const& val) const noexcept -> Vec
    {
        auto ret_val { *this };
        return ret_val /= val;
    }

    auto constexpr operator+=(Vec const& rhs) noexcept -> Vec&
    {
        auto rhs_pos = rhs.begin();
        for (auto& e : elements_)
            e = *rhs_pos++ + e;

        return *this;
    }

    auto constexpr operator+(Vec const& val) const noexcept -> Vec
    {
        auto ret_val { *this };
        return ret_val += val;
    }

private:
    std::array<T, N> elements_;
};

template <std::size_t I, typename T, std::size_t N>
constexpr auto get(Vec<T, N> const&&) noexcept -> T const& = delete;

template <std::size_t I, typename T, std::size_t N>
constexpr auto get(Vec<T, N> const& vec) noexcept -> T const& requires(I < N)
{
    return vec[I];
}

template <std::size_t I, typename T, std::size_t N>
constexpr auto get(Vec<T, N>& vec) noexcept -> T& requires(I < N)
{
    return vec[I];
}

template <typename T, std::size_t N>
constexpr auto lerp(Vec<T, N> const& a, Vec<T, N> const& b, T const& t) noexcept
    -> Vec<T, N>
{
    Vec<T, N> ret_val;
    auto a_input = a.begin();
    auto b_input = b.begin();
    auto out = ret_val.begin();

    while (out != ret_val.end())
        *out++ = std::lerp(*a_input++, *b_input++, t);

    return ret_val;
}

template <typename T, typename... Ts>
constexpr auto to_vec(Ts&&... vals) noexcept
{
    return Vec<T, sizeof...(vals)> { std::forward<Ts>(vals)... };
}

} // namespace rgbctl

#endif // RGBCTL_VEC_HPP_INCLUDED
