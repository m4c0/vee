module;
#include "vulkan.hpp"

export module vee:descriptor_pool;
import :calls;

namespace vee {
export using descriptor_pool =
    calls::handle<VkDescriptorPool, &::vkCreateDescriptorPool,
                  &::vkDestroyDescriptorPool>;

export auto combined_image_sampler(unsigned qty = 1) {
  VkDescriptorPoolSize ps{};
  ps.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  ps.descriptorCount = qty;
  return ps;
}
export auto storage_buffer(unsigned qty = 1) {
  VkDescriptorPoolSize ps{};
  ps.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  ps.descriptorCount = qty;
  return ps;
}
export auto uniform_buffer(unsigned qty = 1) {
  VkDescriptorPoolSize ps{};
  ps.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ps.descriptorCount = qty;
  return ps;
}

export template <unsigned N>
inline auto create_descriptor_pool(unsigned max_sets,
                                   const VkDescriptorPoolSize (&dps)[N]) {
  VkDescriptorPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.maxSets = max_sets;
  info.pPoolSizes = dps;
  info.poolSizeCount = N;
  return descriptor_pool{&info};
}
} // namespace vee
