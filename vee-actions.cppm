module;
#include "vulkan.hpp"

export module vee:actions;
import :calls;

namespace vee {
export inline auto bind_buffer_memory(VkBuffer buf, VkDeviceMemory mem,
                                      VkDeviceSize sz = 0) {
  calls::call(vkBindBufferMemory, buf, mem, sz);
  return nullptr; // Returns "something" to allow binding via RAII
}
export inline auto bind_image_memory(VkImage img, VkDeviceMemory mem,
                                     VkDeviceSize sz = 0) {
  calls::call(vkBindImageMemory, img, mem, sz);
  return nullptr; // Returns "something" to allow binding via RAII
}

export inline auto allocate_primary_command_buffer(VkCommandPool pool) {
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandBufferCount = 1;
  info.commandPool = pool;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  return calls::create<VkCommandBuffer, &::vkAllocateCommandBuffers>(&info);
}
export inline auto allocate_secondary_command_buffer(VkCommandPool pool) {
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandBufferCount = 1;
  info.commandPool = pool;
  info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  return calls::create<VkCommandBuffer, &::vkAllocateCommandBuffers>(&info);
}
} // namespace vee
