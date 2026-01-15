export module vee:image;
import :alias;
import :calls;
import :surface_capabilities;
import wagen;

using namespace wagen;

namespace vee {
export inline auto image_create_info(VkExtent2D ext, VkFormat fmt, VkImageUsageFlags usage) {
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
  info.format = fmt;
  info.usage = usage;
  return info;
}
export inline auto depth_image_create_info(VkExtent2D ext, VkImageUsageFlags usage) {
  return image_create_info(
      ext, VK_FORMAT_D32_SFLOAT,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | usage);
}

export using image = calls::handle<VkImage, &::vkCreateImage, &::vkDestroyImage>;
export inline auto create_depth_image(VkExtent2D ext, VkImageUsageFlags usage) {
  auto info = depth_image_create_info(ext, usage);
  return image{&info};
}
export inline auto create_depth_image(VkPhysicalDevice pd, VkSurfaceKHR s) {
  return create_depth_image(get_surface_capabilities(pd, s).currentExtent, 0);
}

export inline auto create_image(VkExtent2D ext, VkFormat fmt, VkImageUsageFlags usage) {
  auto info = image_create_info(ext, fmt, usage);
  return image{&info};
}
} // namespace vee
