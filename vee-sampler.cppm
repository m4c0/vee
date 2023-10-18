export module vee:sampler;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export using sampler =
    calls::handle<VkSampler, &::vkCreateSampler, &::vkDestroySampler>;

export enum sampler_type { linear_sampler, nearest_sampler };
export inline auto create_sampler(sampler_type st) {
  static constexpr const auto max_anisotropy_ever = 16.0;

  VkSamplerCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.anisotropyEnable = vk_true;
  info.maxAnisotropy = max_anisotropy_ever;
  info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.unnormalizedCoordinates = vk_false; // [0, 1) v [0, w)
  info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  switch (st) {
  case linear_sampler:
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    break;
  case nearest_sampler:
    info.magFilter = VK_FILTER_NEAREST;
    info.minFilter = VK_FILTER_NEAREST;
    break;
  }
  return sampler{&info};
}

export using sampler_ycbcr_conversion =
    calls::handle<VkSamplerYcbcrConversion, &::vkCreateSamplerYcbcrConversion,
                  &::vkDestroySamplerYcbcrConversion>;
export inline auto create_sampler_yuv420p_conversion(VkPhysicalDevice pd) {
  auto fp =
      calls::create<VkFormatProperties, &::vkGetPhysicalDeviceFormatProperties>(
          pd, VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM);
  auto coff =
      fp.optimalTilingFeatures & VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT
          ? VK_CHROMA_LOCATION_COSITED_EVEN
          : VK_CHROMA_LOCATION_MIDPOINT;

  VkComponentMapping c{};
  c.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  c.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  c.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  c.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  VkSamplerYcbcrConversionCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
  info.format = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
  info.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709;
  info.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
  info.components = c;
  info.xChromaOffset = coff;
  info.yChromaOffset = coff;
  return sampler_ycbcr_conversion{&info};
}
} // namespace vee
