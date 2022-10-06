#ifndef RGBCTL_RGBCTL_H_INCLUDED
#define RGBCTL_RGBCTL_H_INCLUDED

#include "./texture.h"
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    RGBCTL_SUCCESS = 0,

    RGBCTL_ERR_NOT_INITIALIZED = 0x7ffffffb,
    RGBCTL_ERR_WRITE = 0x7ffffffc,
    RGBCTL_ERR_READ = 0x7ffffffd,
    RGBCTL_ERR_MODULE_ALLOCATION = 0x7ffffffe,
    RGBCTL_ERR_NOT_IMPLEMENTED = 0x7fffffff
};

/* Errors...
 */
typedef int32_t rgbctl_errno;

/* Modules...
 */
struct rgbctl_device_context;

struct rgbctl_rgb_value
{
    uint8_t red, green, blue;
};

struct rgbctl_zone
{
    uint32_t rgb_count;
    char const* name;
};

struct rgbctl_module
{
    rgbctl_errno (*on_rgb_data)(struct rgbctl_device_context*,  /* context */
                                uint32_t,                       /* zone_index */
                                struct rgbctl_rgb_value const*, /* rgb_data */
                                uint32_t, /* rgb_data_count */
                                void*);   /* user_data */
    rgbctl_errno (*on_query_zones)(struct rgbctl_device_context*,
                                   struct rgbctl_zone const**,
                                   void*);
    void (*on_release)(struct rgbctl_device_context*, void*);
    void (*on_shutdown)(void*);
};

struct rgbctl_module_acquisition
{
    struct rgbctl_module* module;
    void* user_data;
};

struct rgbctl_product_id
{
    uint32_t vendor_id;
    uint32_t product_id;
};

typedef rgbctl_errno (*rgbctl_module_acquisition_callback)(
    struct rgbctl_device_context*,
    struct rgbctl_product_id,
    struct rgbctl_module_acquisition*,
    void*);

struct rgbctl_module_registration
{
    struct rgbctl_product_id const* products;
    uint32_t product_count;
    rgbctl_module_acquisition_callback acquire_callback;
    void* user_data;
};

rgbctl_errno
rgbctl_write(struct rgbctl_device_context*, unsigned char const*, uint32_t);

rgbctl_errno
rgbctl_read(struct rgbctl_device_context*, unsigned char*, uint32_t);

#ifdef __cplusplus
}
#endif
#endif // RGBCTL_RGBCTL_H_INCLUDED
