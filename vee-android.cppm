module;
#include "vulkan.hpp"

export module vee:android;
import :calls;

class ANativeWindow;

namespace vee {
export inline auto create_surface(ANativeWindow *wnd) {
  VkAndroidSurfaceCreateInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
  info.window = wnd;

  return calls::handle<VkSurfaceKHR, &::vkCreateAndroidSurfaceKHR,
                       &::vkDestroySurfaceKHR>{&info};
}
} // namespace vee
