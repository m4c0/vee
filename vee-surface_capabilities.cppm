export module vee:surface_capabilities;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export inline auto get_surface_capabilities(VkPhysicalDevice pd,
                                            VkSurfaceKHR s) {
  return calls::create<VkSurfaceCapabilitiesKHR,
                       &::vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(pd, s);
}
} // namespace vee
