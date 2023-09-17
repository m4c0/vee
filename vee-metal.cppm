
export module vee:metal; // 🤘
import :calls;

namespace vee {
export using surface = calls::handle<VkSurfaceKHR, &::vkCreateMetalSurfaceEXT,
                                     &::vkDestroySurfaceKHR>;
export inline auto create_surface(CAMetalLayer *l) {
  VkMetalSurfaceCreateInfoEXT ci{};
  ci.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
  ci.pLayer = l;
  return surface{&ci};
}
} // namespace vee
