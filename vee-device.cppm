module;
#include "vulkan.hpp"

export module vee:device;
import :calls;

namespace vee {
export inline auto create_single_queue_device(VkPhysicalDevice pd,
                                              unsigned qf) {
  const auto extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  const float priority = 1.0f;

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
  ci.ppEnabledExtensionNames = &extension;
  ci.enabledExtensionCount = 1;
  ci.enabledLayerCount = 0; // device layer is legacy

  auto res =
      calls::handle<VkDevice, &::vkCreateDevice, &::vkDestroyDevice>(pd, &ci);
  volkLoadDevice(*res);
  return res;
}
} // namespace vee
