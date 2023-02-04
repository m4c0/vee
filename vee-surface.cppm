module;
#include "vulkan.hpp"

export module vee:surface;
import :calls;

namespace vee {
export using surface = calls::handle<VkSurfaceKHR, &::vkCreateMetalSurfaceEXT,
                                     &::vkDestroySurfaceKHR>;
} // namespace vee
