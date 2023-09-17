export module vee:semaphore;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export using semaphore =
    calls::handle<VkSemaphore, &::vkCreateSemaphore, &::vkDestroySemaphore>;
export inline auto create_semaphore() {
  VkSemaphoreCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  return semaphore{&info};
}
} // namespace vee
