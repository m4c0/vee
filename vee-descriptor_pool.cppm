export module vee:descriptor_pool;
import :calls;
import wagen;

using namespace wagen;

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
export auto input_attachment(unsigned qty = 1) {
  VkDescriptorPoolSize ps{};
  ps.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
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
  // Actively avoiding VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
  // which requires driver management of the pool, leading to fragmentation.
  VkDescriptorPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.maxSets = max_sets;
  info.pPoolSizes = dps;
  info.poolSizeCount = N;
#ifdef LECO_TARGET_APPLE
  // Otherwise we can't use descriptor arrays larger than 16
  info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
#endif
  return descriptor_pool{&info};
}
} // namespace vee
