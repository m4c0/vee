module;
#ifdef __APPLE__
#define VK_USE_PLATFORM_METAL_EXT
#define VEE_VULKAN_PLATFORM_EXT VK_EXT_METAL_SURFACE_EXTENSION_NAME
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#define VEE_VULKAN_PLATFORM_EXT VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#elif __WIN32__
#define VK_USE_PLATFORM_WIN32_KHR
#define VEE_VULKAN_PLATFORM_EXT VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif

#define VOLK_IMPLEMENTATION

#include "volk/volk.h"

export module vee:vulkan;
import :calls;
import jute;

namespace vee::vk {
static_assert(VK_SUCCESS == 0);

constexpr const auto api_version = VK_API_VERSION_1_0;

constexpr const jute::view debug_utils_ext_name{
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
constexpr const jute::view khr_surf_ext_name{VK_KHR_SURFACE_EXTENSION_NAME};
constexpr const jute::view plat_surf_ext_name{VEE_VULKAN_PLATFORM_EXT};

constexpr const auto create_instance =
    calls::create<&::vkCreateInstance, VkInstance>();
constexpr const auto destroy_instance = calls::destroy<&::vkDestroyInstance>();
constexpr const auto enum_instance_layer_props =
    calls::enumerate<&::vkEnumerateInstanceLayerProperties,
                     VkLayerProperties>();
constexpr const auto enum_instance_ext_props =
    calls::enumerate<&::vkEnumerateInstanceExtensionProperties,
                     VkExtensionProperties>();
} // namespace vee::vk

namespace vee {
using VkApplicationInfo = ::VkApplicationInfo;
using VkInstanceCreateInfo = ::VkInstanceCreateInfo;
using VkInstance = ::VkInstance;

void initialise() {
  static struct init {
    init() { calls::call(volkInitialize); }
  } i;
}

void load_instance(VkInstance i) { volkLoadInstance(i); }
VkInstance get_instance() { return volkGetLoadedInstance(); }

void load_device(VkDevice d) { volkLoadDevice(d); }
VkDevice get_device() { return volkGetLoadedDevice(); }
} // namespace vee
