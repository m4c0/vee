module;
#include "vulkan.hpp"

export module vee:descriptor_set_layout;
import :calls;

namespace vee {
export using descriptor_set_layout =
    calls::handle<VkDescriptorSetLayout, &::vkCreateDescriptorSetLayout,
                  &::vkDestroyDescriptorSetLayout>;

export constexpr VkDescriptorSetLayoutBinding dsl_fragment_sampler() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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
  return descriptor_set_layout{&info};
}
} // namespace vee
