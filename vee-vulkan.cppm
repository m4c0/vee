module;
#ifdef __APPLE__
#define VK_USE_PLATFORM_METAL_EXT
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#elif __WIN32__
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VOLK_IMPLEMENTATION

#include "volk/volk.h"

export module vee:vulkan;
