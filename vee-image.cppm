export module vee:image;
import :alias;
import :calls;
import :surface_capabilities;
import wagen;

using namespace wagen;

namespace vee {
inline auto create_info_for_extent(VkExtent2D ext) {
  VkImageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.extent = {ext.width, ext.height, 1};
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.samples = VK_SAMPLE_COUNT_1_BIT;
  return info;
}
export using image =
    calls::handle<VkImage, &::vkCreateImage, &::vkDestroyImage>;
export inline auto create_depth_image(VkExtent2D ext) {
  auto info = create_info_for_extent(ext);
  info.format = VK_FORMAT_D32_SFLOAT;
  info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  return image{&info};
}
export inline auto create_depth_image(VkPhysicalDevice pd, VkSurfaceKHR s) {
  return create_depth_image(get_surface_capabilities(pd, s).currentExtent);
}

export enum image_usage {
  image_usage_colour_attachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
  image_usage_sampled           = VK_IMAGE_USAGE_SAMPLED_BIT,
  image_usage_transfer_dst      = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
  image_usage_transfer_src      = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
};
export inline auto create_image(VkExtent2D ext, image_format fmt, image_usage u, auto ... us) {
  auto info = create_info_for_extent(ext);
  info.format = static_cast<VkFormat>(fmt);
  info.usage = (static_cast<unsigned>(u) | ... | static_cast<unsigned>(us));
  return image{&info};
}
export inline auto create_image(VkExtent2D ext, image_format fmt) {
  return create_image(ext, fmt, image_usage_sampled, image_usage_transfer_dst);
}

export inline auto create_srgba_image(VkExtent2D ext) {
  return create_image(ext, image_format_srgba);
}

export inline auto create_yuv420p_image(VkExtent2D ext) {
  return create_image(ext, image_format_yuv420p);
}

export inline auto create_renderable_image(VkExtent2D ext, image_format fmt = image_format_srgba) {
  auto info = create_info_for_extent(ext);
  info.format = static_cast<VkFormat>(fmt);
  info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  return image{&info};
}
} // namespace vee
