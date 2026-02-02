export module vee:device;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
  export using device = calls::handle<VkDevice, &::vkCreateDevice, &::vkDestroyDevice>;

  export constexpr inline auto physical_device_sampler_ycbcr_conversion() {
    return VkPhysicalDeviceSamplerYcbcrConversionFeatures {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES,
      .samplerYcbcrConversion = true,
    };
  }
  export constexpr inline auto physical_device_sync_2_features() {
    return VkPhysicalDeviceSynchronization2Features {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
      .synchronization2 = true,
    };
  }

  struct stype_struct {
    VkStructureType sType;
    stype_struct * pNext;
  };
export inline auto create_single_queue_device(VkPhysicalDevice pd, unsigned qf, const VkPhysicalDeviceFeatures & feats, void * next = nullptr) {
  const float priority = 1.0f;

  hai::varray<const char *> exts { 16 };
  exts.push_back(vk_khr_swapchain_extension_name);
  for (auto n = reinterpret_cast<stype_struct *>(next); n; n = n->pNext) {
    if (n->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES) {
      exts.push_back(vk_khr_synchronization2_extension_name);
    } else if (n->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT) {
      exts.push_back("VK_EXT_extended_dynamic_state3");
    }
  }

  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = qf;
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &priority;

  VkDeviceCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  ci.pNext = next;
  ci.pQueueCreateInfos = &queue_create_info;
  ci.queueCreateInfoCount = 1;
  ci.pEnabledFeatures = &feats;
  ci.ppEnabledExtensionNames = exts.begin();
  ci.enabledExtensionCount = exts.size();
  ci.enabledLayerCount = 0; // device layer is legacy

  auto res = device(pd, &ci);
  wagen::device() = *res;
  wagen::physical_device() = pd;
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
