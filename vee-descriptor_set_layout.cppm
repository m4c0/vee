module;
#include "vulkan.hpp"

export module vee:descriptor_set_layout;
import :calls;

namespace vee {
export using descriptor_set_layout =
    calls::handle<VkDescriptorSetLayout, &::vkCreateDescriptorSetLayout,
                  &::vkDestroyDescriptorSetLayout>;

export enum dsl_type {
  dsl_fragment_sampler,
  dsl_fragment_uniform,
  dsl_vertex_sampler,
  dsl_vertex_uniform,
};
template <dsl_type D> constexpr VkDescriptorSetLayoutBinding dsl();
template <> constexpr VkDescriptorSetLayoutBinding dsl<dsl_fragment_sampler>() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  return b;
}
template <> constexpr VkDescriptorSetLayoutBinding dsl<dsl_fragment_uniform>() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  return b;
}
template <> constexpr VkDescriptorSetLayoutBinding dsl<dsl_vertex_sampler>() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  return b;
}
template <> constexpr VkDescriptorSetLayoutBinding dsl<dsl_vertex_uniform>() {
  VkDescriptorSetLayoutBinding b{};
  b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  return b;
}

export template <dsl_type... D> inline auto create_descriptor_set_layout() {
  VkDescriptorSetLayoutBinding bindings[sizeof...(D)]{dsl<D>()...};
  for (auto i = 0UL; i < sizeof...(D); i++) {
    bindings[i].binding = i;
  }

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = sizeof...(D);
  info.pBindings = bindings;
  return descriptor_set_layout{&info};
}
} // namespace vee
