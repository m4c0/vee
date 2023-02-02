module;
#include "vulkan.hpp"

export module vee:metal; // 🤘
import :calls;

namespace vee {
export inline auto create_surface(CAMetalLayer *l) {
  VkMetalSurfaceCreateInfoEXT ci{};
  ci.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
  ci.pLayer = l;

  return calls::handle<VkSurfaceKHR, &::vkCreateMetalSurfaceEXT,
                       &::vkDestroySurfaceKHR>{&ci};
}
} // namespace vee
