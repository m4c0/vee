module;
#include "vulkan.hpp"

export module vee:image_view;
import :calls;
import :surface_format;

namespace vee {
static auto create_info_for_aspect_mask(VkImageAspectFlags aspect_mask) {
  VkImageViewCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.subresourceRange.layerCount = 1;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.aspectMask = aspect_mask;
  return info;
}

export using image_view =
    calls::handle<VkImageView, &::vkCreateImageView, &::vkDestroyImageView>;
export inline auto create_depth_image_view(VkImage img) {
  auto info = create_info_for_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT);
  info.image = img;
  info.format = VK_FORMAT_D32_SFLOAT;
  return image_view{&info};
}
export inline auto create_rgba_image_view(VkImage img,
                                          VkSurfaceFormatKHR sfmt) {
  auto info = create_info_for_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
  info.image = img;
  info.format = sfmt.format;
  return image_view{&info};
}
export inline auto create_rgba_image_view(VkImage img, VkPhysicalDevice pd,
                                          VkSurfaceKHR s) {
  auto sfmt = vee::find_best_surface_format(pd, s);

  auto info = create_info_for_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
  info.image = img;
  info.format = sfmt.format;
  return image_view{&info};
}
} // namespace vee
