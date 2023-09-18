export module vee:windows;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export using surface = calls::handle<VkSurfaceKHR, &::vkCreateWin32SurfaceKHR,
                                     &::vkDestroySurfaceKHR>;
export inline auto create_surface(HWND hwnd) {
  VkWin32SurfaceCreateInfoKHR ci{};
  ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  ci.hinstance = get_module_handle(nullptr);
  ci.hwnd = hwnd;
  return surface{&ci};
}
} // namespace vee
