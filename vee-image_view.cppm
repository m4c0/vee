export module vee:image_view;
import :calls;
import :image;
import :surface_format;
import wagen;

using namespace wagen;

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

export inline auto create_image_view(VkImage img, vee::image_format fmt) {
  auto info = create_info_for_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
  info.image = img;
  info.format = static_cast<VkFormat>(fmt);
  if (fmt == vee::image_format_r8) {
    info.components.r = VK_COMPONENT_SWIZZLE_ONE;
    info.components.g = VK_COMPONENT_SWIZZLE_ONE;
    info.components.b = VK_COMPONENT_SWIZZLE_ONE;
    info.components.a = VK_COMPONENT_SWIZZLE_R;
  }
  return image_view{&info};
}
export inline auto create_r8_image_view(VkImage img) {
  return create_image_view(img, vee::image_format_r8);
}
export inline auto create_unorm_rgba_image_view(VkImage img) {
  return create_image_view(img, image_format_rgba_unorm);
}
export inline auto create_srgba_image_view(VkImage img) {
  return create_image_view(img, image_format_srgba);
}
export inline auto create_yuv420p_image_view(VkImage img,
                                             VkSamplerYcbcrConversion conv) {
  VkSamplerYcbcrConversionInfo yuv{};
  yuv.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
  yuv.conversion = conv;

  auto info = create_info_for_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
  info.pNext = &yuv;
  info.image = img;
  info.format = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
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
