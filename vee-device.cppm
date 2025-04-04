export module vee:device;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export using device =
    calls::handle<VkDevice, &::vkCreateDevice, &::vkDestroyDevice>;
export inline auto create_single_queue_device(VkPhysicalDevice pd, unsigned qf, const VkPhysicalDeviceFeatures & feats) {
  const float priority = 1.0f;

  constexpr const auto ext_count = 2;
  const char * ext[] = {
    vk_khr_swapchain_extension_name,
    vk_khr_synchronization2_extension_name,
  };

  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = qf;
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &priority;

  VkPhysicalDeviceSynchronization2Features sync_feats {};
  sync_feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
  sync_feats.synchronization2 = vk_true;

  VkPhysicalDeviceSamplerYcbcrConversionFeatures smp_feats{};
  smp_feats.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
  smp_feats.samplerYcbcrConversion = vk_true;
  smp_feats.pNext = &sync_feats;

  VkDeviceCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
#ifndef LECO_TARGET_LINUX
  ci.pNext = &smp_feats;
#endif
  ci.pQueueCreateInfos = &queue_create_info;
  ci.queueCreateInfoCount = 1;
  ci.pEnabledFeatures = &feats;
  ci.ppEnabledExtensionNames = ext;
  ci.enabledExtensionCount = ext_count;
  ci.enabledLayerCount = 0; // device layer is legacy

  auto res = device(pd, &ci);
  wagen::device() = *res;
  return res;
}
export inline auto create_single_queue_device(VkPhysicalDevice pd, unsigned qf) {
  return create_single_queue_device(pd, qf, {
    .independentBlend  = vk_true,
    .samplerAnisotropy = vk_true,
  });
}

export inline auto get_queue_for_family(unsigned qf) {
  return calls::create<VkQueue, &::vkGetDeviceQueue>(qf, 0);
}
} // namespace vee
