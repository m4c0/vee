module;
#include "vulkan.hpp"

export module vee:buffer;
import :calls;

namespace vee {
export using buffer =
    calls::handle<VkBuffer, &::vkCreateBuffer, &::vkDestroyBuffer>;

export inline auto create_vertex_buffer(unsigned size) {
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  info.size = size;
  return buffer{&info};
}

export inline auto create_transfer_src_buffer(unsigned size) {
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  info.size = size;
  return buffer{&info};
}
} // namespace vee
