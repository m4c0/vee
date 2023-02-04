module;
#include "vulkan.hpp"

export module vee:command_pool;
import :calls;

namespace vee {
export using command_pool = calls::handle<VkCommandPool, &::vkCreateCommandPool,
                                          &::vkDestroyCommandPool>;
export inline auto create_command_pool(unsigned qf) {
  VkCommandPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = qf;
  return command_pool(&info);
}
} // namespace vee
