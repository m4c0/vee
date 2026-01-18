export module vee:buffer;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
  export using buffer = calls::handle<VkBuffer, &::vkCreateBuffer, &::vkDestroyBuffer>;

  export inline auto create_buffer(unsigned size, VkBufferUsageFlags usage) {
    VkBufferCreateInfo info {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size  = size,
      .usage = usage,
    };
    return buffer { &info };
  }
}
