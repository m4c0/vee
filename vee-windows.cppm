module;
#include "vulkan.hpp"
#include <windows.h>

export module vee:metal;
import :calls;

namespace vee {
export inline auto create_surface(HWND hwnd) {
  VkWin32SurfaceCreateInfoKHR ci{};
  ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  ci.hinstance = GetModuleHandle(nullptr);
  ci.hwnd = hwnd;

  return calls::handle<VkSurfaceKHR, &::vkCreateWin32SurfaceKHR,
                       &::vkDestroySurfaceKHR>{&ci};
}
} // namespace vee
