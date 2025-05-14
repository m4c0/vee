export module vee:device_memory;
import :calls;
import silog;
import wagen;

using namespace wagen;

namespace vee {
  static constexpr const auto device_local_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  static constexpr const auto host_flags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

inline unsigned find_memory_type_index(VkPhysicalDevice pd, unsigned type_bits, VkMemoryAllocateFlags flags) {
  auto props = calls::create<VkPhysicalDeviceMemoryProperties, &::vkGetPhysicalDeviceMemoryProperties>(pd);

  for (unsigned i = 0; i < props.memoryTypeCount; i++) {
    if (type_bits != 0 && (type_bits & (1U << i)) == 0) continue;
    if ((props.memoryTypes[i].propertyFlags & flags) != flags) continue;
    return i;
  }

  silog::die("Failed to find suitable memory type");
}
  export inline auto find_device_local_memory_type_index(VkPhysicalDevice pd) {
    return find_memory_type_index(pd, 0, device_local_flags);
  }
  export inline auto find_host_memory_type_index(VkPhysicalDevice pd) {
    return find_memory_type_index(pd, 0, host_flags);
  }

export using device_memory = calls::handle<VkDeviceMemory, &::vkAllocateMemory, &::vkFreeMemory>;

export inline auto create_memory(VkDeviceSize size, unsigned memory_type_idx) {
  VkMemoryAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info.allocationSize = size;
  info.memoryTypeIndex = memory_type_idx;
  return device_memory(&info);
}

export inline auto create_memory(VkPhysicalDevice pd, VkMemoryRequirements mr, VkMemoryAllocateFlags flags) {
  return create_memory(mr.size, find_memory_type_index(pd, mr.memoryTypeBits, flags));
}

export inline auto create_local_image_memory(VkPhysicalDevice pd, VkImage img) {
  auto mr = calls::create<VkMemoryRequirements, &::vkGetImageMemoryRequirements>(img);
  return create_memory(pd, mr, device_local_flags);
}
export inline auto create_local_buffer_memory(VkPhysicalDevice pd, VkBuffer buf) {
  auto mr = calls::create<VkMemoryRequirements, &::vkGetBufferMemoryRequirements>(buf);
  return create_memory(pd, mr, device_local_flags);
}

export inline auto create_host_buffer_memory(VkPhysicalDevice pd, VkBuffer buf) {
  auto mr = calls::create<VkMemoryRequirements, &::vkGetBufferMemoryRequirements>(buf);
  return create_memory(pd, mr, host_flags);
}
export inline auto create_host_image_memory(VkPhysicalDevice pd, VkImage buf) {
  auto mr = calls::create<VkMemoryRequirements, &::vkGetImageMemoryRequirements>(buf);
  return create_memory(pd, mr, host_flags);
}

export inline auto create_host_memory(VkPhysicalDevice pd, unsigned sz) {
  return create_memory(pd, VkMemoryRequirements { .size = sz }, host_flags);
}
export inline auto create_local_memory(VkPhysicalDevice pd, unsigned sz) {
  return create_memory(pd, VkMemoryRequirements { .size = sz }, device_local_flags);
}
} // namespace vee
