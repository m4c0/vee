module;
#include "vulkan.hpp"

export module vee:device;
import :calls;

namespace vee {
export using device =
    calls::handle<VkDevice, &::vkCreateDevice, &::vkDestroyDevice>;
export inline auto create_single_queue_device(VkPhysicalDevice pd,
                                              unsigned qf) {
  const float priority = 1.0f;

#ifdef __APPLE__
  // TODO: decide if support for other non-conformant Vulkan devices
  constexpr const auto ext_count = 2;
  const char *ext[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      "VK_KHR_portability_subset",
  };
#else
  constexpr const auto ext_count = 1;
  const char *ext[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif

  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = qf;
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &priority;

  VkPhysicalDeviceFeatures feats{};
  feats.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  ci.pQueueCreateInfos = &queue_create_info;
  ci.queueCreateInfoCount = 1;
  ci.pEnabledFeatures = &feats;
  ci.ppEnabledExtensionNames = ext;
  ci.enabledExtensionCount = ext_count;
  ci.enabledLayerCount = 0; // device layer is legacy

  auto res = device(pd, &ci);
  volkLoadDevice(*res);
  return res;
}

export inline auto get_queue_for_family(unsigned qf) {
  return calls::create<VkQueue, &::vkGetDeviceQueue>(qf, 0);
}
} // namespace vee
