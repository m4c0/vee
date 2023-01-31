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

namespace vee::calls {
template <auto *Fn, typename Ret> consteval auto wrap() {
  return [](auto &in) {
    Ret out{};
    if ((*Fn)(&in, nullptr, &out) != VK_SUCCESS) {
      // TODO: log? throw?
    }
    return out;
  };
}

constexpr const auto create_instance = wrap<&::vkCreateInstance, VkInstance>();
} // namespace vee::calls

namespace vee {
constexpr const auto vk_api_version = VK_API_VERSION_1_0;

using VkApplicationInfo = ::VkApplicationInfo;
using VkInstanceCreateInfo = ::VkInstanceCreateInfo;

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
