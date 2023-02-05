module;
#include "vulkan.hpp"

export module vee:surface_capabilities;
import :calls;

namespace vee {
inline auto get_surface_capabilities(VkPhysicalDevice pd, VkSurfaceKHR s) {
  return calls::create<VkSurfaceCapabilitiesKHR,
                       &::vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(pd, s);
}
} // namespace vee
