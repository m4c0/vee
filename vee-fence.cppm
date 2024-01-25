export module vee:fence;
import :calls;
import silog;
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

export inline auto wait_for_fence(VkFence fence, unsigned millis) {
  calls::call(vkWaitForFences, 1, &fence, vk_true, millis * 1000000UL);
}
export inline auto wait_for_fence(VkFence fence) {
  calls::call(vkWaitForFences, 1, &fence, vk_true, ~0UL);
}

export inline auto get_fence_status(VkFence fence) {
  switch (auto res = vkGetFenceStatus(device(), fence)) {
  case VK_SUCCESS:
    return true;
  case VK_NOT_READY:
    return false;
  default:
    silog::log(silog::error, "%s", message_for_result(res));
    throw api_failure{};
  }
}

export inline auto reset_fence(VkFence fence) {
  calls::call(vkResetFences, 1, &fence);
}

export inline auto wait_and_reset_fence(VkFence fence, unsigned millis) {
  wait_for_fence(fence, millis);
  reset_fence(fence);
}
export inline auto wait_and_reset_fence(VkFence fence) {
  wait_for_fence(fence);
  reset_fence(fence);
}
} // namespace vee
