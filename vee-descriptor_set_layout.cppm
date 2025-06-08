export module vee:descriptor_set_layout;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export using descriptor_set_layout =
    calls::handle<VkDescriptorSetLayout, &::vkCreateDescriptorSetLayout,
                  &::vkDestroyDescriptorSetLayout>;

export constexpr VkDescriptorSetLayoutBinding dsl_compute_storage() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  return b;
}
export constexpr VkDescriptorSetLayoutBinding dsl_fragment_sampler(unsigned count = 1) {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  b.descriptorCount = count;
  b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  return b;
}
export template <unsigned N>
constexpr VkDescriptorSetLayoutBinding
dsl_fragment_samplers(VkSampler (&&smp)[N]) {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  b.descriptorCount = N;
  b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  b.pImmutableSamplers = smp;
  return b;
}
export constexpr VkDescriptorSetLayoutBinding dsl_fragment_input_attachment() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  return b;
}
export constexpr VkDescriptorSetLayoutBinding dsl_fragment_storage() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  return b;
}
export constexpr VkDescriptorSetLayoutBinding dsl_fragment_uniform() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  return b;
}
export constexpr VkDescriptorSetLayoutBinding dsl_vertex_sampler() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  return b;
}
export constexpr VkDescriptorSetLayoutBinding dsl_vertex_storage() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  return b;
}
export constexpr VkDescriptorSetLayoutBinding dsl_vertex_uniform() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  return b;
}

export template <unsigned N>
inline auto
create_descriptor_set_layout(VkDescriptorSetLayoutBinding (&&bindings)[N]) {
  for (auto i = 0UL; i < N; i++) {
    bindings[i].binding = i;
  }

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = N;
  info.pBindings = bindings;
#ifdef LECO_TARGET_APPLE
  // Otherwise we can't use descriptor arrays larger than 16
  info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
#endif
  return descriptor_set_layout{&info};
}
} // namespace vee
