export module vee:image;
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

export inline auto create_srgba_image(VkExtent2D ext) {
  auto info = create_info_for_extent(ext);
  info.format = VK_FORMAT_R8G8B8A8_SRGB;
  info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  return image{&info};
}

export inline auto create_renderable_image(VkExtent2D ext) {
  auto info = create_info_for_extent(ext);
  info.format = VK_FORMAT_R8G8B8A8_SRGB;
  info.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  return image{&info};
}
} // namespace vee
