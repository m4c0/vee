module;
#ifdef LECO_TARGET_LINUX
#include <X11/Xlib.h>
#endif

struct casein_native_handle {
  Display * display;
  Window window;
};

export module vee:linux;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export using surface = calls::handle<VkSurfaceKHR, &::vkCreateXlibSurfaceKHR,
                                     &::vkDestroySurfaceKHR>;
export inline auto create_surface(auto *ptr) {
  auto [d, w] = *ptr;

  VkXlibSurfaceCreateInfoKHR ci{};
  ci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
  ci.dpy = d;
  ci.window = w;
  return surface{&ci};
}
} // namespace vee
