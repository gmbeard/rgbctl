#ifndef RGBCTL_TEXTURE_H_INCLUDED
#define RGBCTL_TEXTURE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    RGBCTL_SAMPLE_NEAREST = 0x01,
    RGBCTL_SAMPLE_LINEAR = 0x02
};

struct rgbctl_texture;

struct rgbctl_rgb_float_value
{
    float red, green, blue;
};

struct rgbctl_rgb_float_value
rgb_sample_texture(struct rgbctl_texture const*, float x, float y, int);

#ifdef __cplusplus
}
#endif
#endif // RGBCTL_TEXTURE_H_INCLUDED
