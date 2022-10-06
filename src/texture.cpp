#include "rgbctl/texture.hpp"
#include "rgbctl/vec.hpp"

using namespace rgbctl;

rgbctl_texture::rgbctl_texture(std::span<RgbFloat const> texels,
                               std::size_t width)
    : texels_ { texels.begin(), texels.end() }
    , width_ { width == one_row ? texels_.size() : width }
{ }

auto rgbctl_texture::width() const noexcept -> std::size_t
{
    return width_;
}

auto rgbctl_texture::height() const noexcept -> std::size_t
{
    return texels_.size() / width_;
}

auto rgbctl_texture::sample(Vec<float, 2> const& coord,
                            Filtering required_filtering) const noexcept
    -> RgbFloat
{
    switch (required_filtering) {
    case Filtering::Nearest:
        return sample(coord, texture_filtering_nearest);
    case Filtering::Linear:
    default:
        return sample(coord, texture_filtering_linear);
    }
}

constexpr auto wrap_around(Vec<float, 2> const& coord) noexcept -> Vec<float, 2>
{
    Vec<float, 2> tmp { coord };

    get<0>(tmp) = std::abs(get<0>(tmp));
    get<1>(tmp) = std::abs(get<1>(tmp));

    get<0>(tmp) = get<0>(tmp) > 1.f
                      ? 0.f + get<0>(tmp) - std::floor(get<0>(tmp))
                      : get<0>(tmp);

    get<1>(tmp) = get<1>(tmp) > 1.f
                      ? 0.f + get<1>(tmp) - std::floor(get<1>(tmp))
                      : get<1>(tmp);

    return tmp;
}

constexpr auto constrain_wrap(Vec<float, 2> const& coord) noexcept
    -> Vec<float, 2>
{
    return {
        coord[0] > 0.f ? std::fmod(coord[0], 1.f)
                       : 1.f - std::fmod(std::abs(coord[0]), 1.f),
        coord[1] > 0.f ? std::fmod(coord[1], 1.f)
                       : 1.f - std::fmod(std::abs(coord[1]), 1.f),
    };
}

constexpr auto constrain_clamp(Vec<float, 2> const& coord) noexcept
    -> Vec<float, 2>
{
    return {
        std::clamp(coord[0], 0.f, 1.f),
        std::clamp(coord[1], 0.f, 1.f),
    };
}

auto rgbctl_texture::sample(Vec<float, 2> const& pos,
                            NearestFiltering) const noexcept -> RgbFloat
{
    //
    using std::begin;
    using std::distance;
    using std::end;
    using std::min;
    using std::next;
    using std::size;

    using distance_type = decltype(distance(begin(texels_), end(texels_)));

    auto const coord = constrain_wrap(pos);

    distance_type u
        = static_cast<distance_type>(coord[0] * static_cast<float>(width()))
          % static_cast<distance_type>(width());
    distance_type v
        = static_cast<distance_type>(coord[1] * static_cast<float>(height()))
          % static_cast<distance_type>(height());

    auto texel_pos = v * static_cast<distance_type>(width()) + u;

    return *next(begin(texels_), texel_pos);
}

auto rgbctl_texture::sample(Vec<float, 2> const& pos,
                            LinearFiltering) const noexcept -> RgbFloat
{
    //
    using std::begin;
    using std::distance;
    using std::end;
    using std::min;
    using std::next;
    using std::size;

    using distance_type = decltype(distance(begin(texels_), end(texels_)));

    auto const coord = constrain_wrap(pos);

    float u = coord[0] * static_cast<float>(width());
    float v = coord[1] * static_cast<float>(height());
    float u_dist = u - std::floor(u);
    float v_dist = v - std::floor(v);

    distance_type const u_index
        = static_cast<distance_type>(u) % static_cast<distance_type>(width());

    distance_type const v_index
        = static_cast<distance_type>(v) % static_cast<distance_type>(height());

    auto c0_pos = v_index * static_cast<distance_type>(width()) + u_index;

    auto c1_pos = v_index * static_cast<distance_type>(width())
                  + ((u_index + 1) % static_cast<distance_type>(width()));

    auto c2_pos = (((v_index + 1) % static_cast<distance_type>(height()))
                   * static_cast<distance_type>(width()))
                  + u_index;

    auto c3_pos = (((v_index + 1) % static_cast<distance_type>(height()))
                   * static_cast<distance_type>(width()))
                  + ((u_index + 1) % static_cast<distance_type>(width()));

    auto c0 = *next(begin(texels_), c0_pos);
    auto c1 = *next(begin(texels_), c1_pos);
    auto c2 = *next(begin(texels_), c2_pos);
    auto c3 = *next(begin(texels_), c3_pos);

    return lerp(lerp(c0, c1, u_dist), lerp(c2, c3, u_dist), v_dist);
}

auto to_rgb_float_value(Vec<float, 3> const& vec) noexcept
    -> rgbctl_rgb_float_value
{
    return rgbctl_rgb_float_value { .red = vec[0],
                                    .green = vec[1],
                                    .blue = vec[2] };
}

rgbctl_rgb_float_value rgb_sample_texture(rgbctl_texture const* texture,
                                          float x,
                                          float y,
                                          int filtering)
{
    switch (filtering) {
    case RGBCTL_SAMPLE_NEAREST:
        return to_rgb_float_value(
            texture->sample(to_vec<float>(x, y), texture_filtering_nearest));
    default:
        return to_rgb_float_value(
            texture->sample(to_vec<float>(x, y), texture_filtering_linear));
    }
}
