export module vee:device_memory;
import :calls;
import silog;
import wagen;

using namespace wagen;

namespace vee {
inline unsigned find_memory_type_index(VkPhysicalDevice pd, unsigned type_bits,
                                       VkMemoryAllocateFlags flags) {
  auto props = calls::create<VkPhysicalDeviceMemoryProperties,
                             &::vkGetPhysicalDeviceMemoryProperties>(pd);

  for (unsigned i = 0; i < props.memoryTypeCount; i++) {
    if ((type_bits & (1U << i)) == 0) {
      continue;
    }
    if ((props.memoryTypes[i].propertyFlags & flags) != flags) {
      continue;
    }
    return i;
  }

  silog::log(silog::error, "Failed to find suitable memory type");
  // TODO: throw exception?
  return 0;
}
export using device_memory =
    calls::handle<VkDeviceMemory, &::vkAllocateMemory, &::vkFreeMemory>;
export inline auto create_memory(VkPhysicalDevice pd, VkMemoryRequirements mr,
                                 VkMemoryAllocateFlags flags) {
  VkMemoryAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info.allocationSize = mr.size;
  info.memoryTypeIndex = find_memory_type_index(pd, mr.memoryTypeBits, flags);
  return device_memory(&info);
}

export inline auto create_local_image_memory(VkPhysicalDevice pd, VkImage img) {
  constexpr const auto flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  auto mr =
      calls::create<VkMemoryRequirements, &::vkGetImageMemoryRequirements>(img);
  return create_memory(pd, mr, flags);
}
export inline auto create_local_buffer_memory(VkPhysicalDevice pd,
                                              VkBuffer buf) {
  constexpr const auto flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  auto mr =
      calls::create<VkMemoryRequirements, &::vkGetBufferMemoryRequirements>(
          buf);
  return create_memory(pd, mr, flags);
}

export inline auto create_host_buffer_memory(VkPhysicalDevice pd,
                                             VkBuffer buf) {
  constexpr const auto flags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  auto mr =
      calls::create<VkMemoryRequirements, &::vkGetBufferMemoryRequirements>(
          buf);
  return create_memory(pd, mr, flags);
}
export inline auto create_host_image_memory(VkPhysicalDevice pd, VkImage buf) {
  constexpr const auto flags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  auto mr =
      calls::create<VkMemoryRequirements, &::vkGetImageMemoryRequirements>(buf);
  return create_memory(pd, mr, flags);
}
} // namespace vee
