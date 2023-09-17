export module vee:fence;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export using fence =
    calls::handle<VkFence, &::vkCreateFence, &::vkDestroyFence>;
export inline auto create_fence_signaled() {
  VkFenceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  return fence{&info};
}
} // namespace vee
