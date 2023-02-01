module;
#ifdef __APPLE__
#define VK_USE_PLATFORM_METAL_EXT
#define VEE_VULKAN_PLATFORM_EXT VK_EXT_METAL_SURFACE_EXTENSION_NAME
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#define VEE_VULKAN_PLATFORM_EXT VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#define VEE_VULKAN_PLATFORM_EXT VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif

#define VOLK_IMPLEMENTATION

#include "volk/volk.h"

export module vee:vulkan;
import :calls;
import jute;
import silog;

namespace vee::vk {
static_assert(VK_SUCCESS == 0);

constexpr const auto api_version = VK_API_VERSION_1_0;

constexpr const jute::view debug_utils_ext_name{
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
constexpr const jute::view khr_surf_ext_name{VK_KHR_SURFACE_EXTENSION_NAME};
constexpr const jute::view plat_surf_ext_name{VEE_VULKAN_PLATFORM_EXT};

constexpr const auto enum_instance_layer_props =
    calls::enumerate<&::vkEnumerateInstanceLayerProperties,
                     VkLayerProperties>();
constexpr const auto enum_instance_ext_props =
    calls::enumerate<&::vkEnumerateInstanceExtensionProperties,
                     VkExtensionProperties>();
} // namespace vee::vk

namespace vee::details {
inline VkResult create_instance(const VkInstanceCreateInfo *info,
                                const VkAllocationCallbacks *cb,
                                VkInstance *out) {
  VkResult result = ::vkCreateInstance(info, cb, out);
  if (result == VK_SUCCESS) {
    ::volkLoadInstance(*out);
    silog::log(silog::info, "Vulkan instance created");
  }
  return result;
}
} // namespace vee::details

namespace vee::objects {
using instance =
    calls::handle<VkInstance, &details::create_instance, &::vkDestroyInstance>;
}

namespace vee {
using VkApplicationInfo = ::VkApplicationInfo;
using VkDevice = ::VkDevice;
using VkInstanceCreateInfo = ::VkInstanceCreateInfo;

VkInstance get_instance() { return volkGetLoadedInstance(); }

void load_device(VkDevice d) { volkLoadDevice(d); }
VkDevice get_device() { return volkGetLoadedDevice(); }
} // namespace vee

static struct init {
  init() { vee::calls::call(volkInitialize); }
} i;
