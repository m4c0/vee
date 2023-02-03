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
} // namespace vee
