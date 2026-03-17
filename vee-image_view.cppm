export module vee:image_view;
import :calls;
import :image;
import :surface_format;
import wagen;

using namespace wagen;

namespace vee {
  using image_aspect_flags = VkImageAspectFlagBits;
  export inline auto image_subresource_range(VkImageAspectFlags aspect_mask, unsigned layer_count = 1) {
    return VkImageSubresourceRange {
      .aspectMask = aspect_mask,
      .levelCount = 1,
      .layerCount = layer_count,
    };
  }

  export using image_view = calls::handle<VkImageView, &::vkCreateImageView, &::vkDestroyImageView>;

  export inline auto create_image_view(VkImageViewCreateInfo info) {
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    return image_view { &info };
  }

  export inline auto create_depth_image_view(VkImage img) {
    return create_image_view({
      .image = img,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = VK_FORMAT_D32_SFLOAT,
      .subresourceRange = image_subresource_range(VK_IMAGE_ASPECT_DEPTH_BIT),
    });
  }

  export inline auto create_image_view(VkImage img, VkFormat fmt) {
    return create_image_view({
      .image = img,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = static_cast<VkFormat>(fmt),
      .subresourceRange = image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT),
    });
  }

  export inline auto create_yuv420p_image_view(VkImage img, VkSamplerYcbcrConversion conv) {
    VkSamplerYcbcrConversionInfo yuv{};
    yuv.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    yuv.conversion = conv;
  
    return create_image_view({
      .pNext = &yuv,
      .image = img,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
      .subresourceRange = image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT),
    });
  }

  export inline auto create_image_view_for_surface(VkImage img, VkPhysicalDevice pd, VkSurfaceKHR s) {
    auto sfmt = vee::find_best_surface_format(pd, s);
    return create_image_view(img, sfmt.format);
  }
} // namespace vee
