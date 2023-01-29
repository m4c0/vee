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

export namespace vee::loader {
void initialise() { volkInitialize(); }

void load_instance(VkInstance i) { volkLoadInstance(i); }
VkInstance get_instance() { return volkGetLoadedInstance(); }

void load_device(VkDevice d) { volkLoadDevice(d); }
VkDevice get_device() { return volkGetLoadedDevice(); }
} // namespace vee::loader
