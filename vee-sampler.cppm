module;
#include "vulkan.hpp"

export module vee:sampler;

import :calls;

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
  info.anisotropyEnable = VK_TRUE;
  info.maxAnisotropy = max_anisotropy_ever;
  info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.unnormalizedCoordinates = VK_FALSE; // [0, 1) v [0, w)
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
} // namespace vee
