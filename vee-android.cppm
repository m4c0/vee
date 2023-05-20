module;
#include "vulkan.hpp"

export module vee:android;
import :calls;

namespace vee {
export using surface = calls::handle<VkSurfaceKHR, &::vkCreateAndroidSurfaceKHR,
                                     &::vkDestroySurfaceKHR>;
export inline auto create_surface(ANativeWindow *wnd) {
  VkAndroidSurfaceCreateInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
  info.window = wnd;
  return surface{&info};
}
} // namespace vee
