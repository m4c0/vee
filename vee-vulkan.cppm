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

namespace vee {
constexpr const auto vk_api_version = VK_API_VERSION_1_0;

using VkApplicationInfo = ::VkApplicationInfo;
using VkAllocationCallbacks = ::VkAllocationCallbacks;
using VkInstanceCreateInfo = ::VkInstanceCreateInfo;
using VkInstance = ::VkInstance;

inline auto vkCreateInstance = ::vkCreateInstance;

void initialise() {
  static struct init {
    init() { volkInitialize(); }
  } i;
}

void load_instance(VkInstance i) { volkLoadInstance(i); }
VkInstance get_instance() { return volkGetLoadedInstance(); }

void load_device(VkDevice d) { volkLoadDevice(d); }
VkDevice get_device() { return volkGetLoadedDevice(); }
} // namespace vee
