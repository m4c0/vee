export module vee:surface_format;
import :alias;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
static constexpr const auto get_pd_surf_fmts =
    calls::enumerate<&::vkGetPhysicalDeviceSurfaceFormatsKHR,
                     VkSurfaceFormatKHR>();

inline auto find_best_surface_format(VkPhysicalDevice pd, VkSurfaceKHR surf) {
  auto formats = get_pd_surf_fmts(pd, surf);
  for (const auto &fmt : formats) {
    if ((fmt.format == VK_FORMAT_B8G8R8A8_SRGB) &&
        (fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
      return fmt;
    }
  }
  for (const auto &fmt : formats) {
    if ((fmt.format == VK_FORMAT_B8G8R8A8_SRGB)) {
      return fmt;
    }
  }
  return *formats.begin();
}
export image_format find_best_surface_image_format(VkPhysicalDevice pd, VkSurfaceKHR s) {
  return static_cast<image_format>(find_best_surface_format(pd, s).format);
}
} // namespace vee
