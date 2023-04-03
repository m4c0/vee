module;
#include "vulkan.hpp"

export module vee:pipeline_layout;
import :calls;

namespace vee {
export template <typename Tp>
inline auto fragment_push_constant_range(unsigned offset = 0) {
  VkPushConstantRange r;
  r.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  r.offset = offset;
  r.size = sizeof(Tp);
  return r;
}
export template <typename Tp>
inline auto vertex_push_constant_range(unsigned offset = 0) {
  VkPushConstantRange r;
  r.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  r.offset = offset;
  r.size = sizeof(Tp);
  return r;
}
export template <typename Tp>
inline auto vert_frag_push_constant_range(unsigned offset = 0) {
  VkPushConstantRange r;
  r.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  r.offset = offset;
  r.size = sizeof(Tp);
  return r;
}

export using pipeline_layout =
    calls::handle<VkPipelineLayout, &::vkCreatePipelineLayout,
                  &::vkDestroyPipelineLayout>;
export template <unsigned D, unsigned P>
inline auto create_pipeline_layout(const VkDescriptorSetLayout (&dsl)[D],
                                   const VkPushConstantRange (&pcr)[P]) {
  VkPipelineLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = D;
  info.pSetLayouts = dsl;
  info.pushConstantRangeCount = P;
  info.pPushConstantRanges = pcr;
  return pipeline_layout{&info};
}
export template <unsigned D>
inline auto create_pipeline_layout(const VkDescriptorSetLayout (&dsl)[D]) {
  VkPipelineLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = D;
  info.pSetLayouts = dsl;
  return pipeline_layout{&info};
}
export template <unsigned P>
inline auto create_pipeline_layout(const VkPushConstantRange (&pcr)[P]) {
  VkPipelineLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.pushConstantRangeCount = P;
  info.pPushConstantRanges = pcr;
  return pipeline_layout{&info};
}
export inline auto create_pipeline_layout() {
  VkPipelineLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  return pipeline_layout{&info};
}
} // namespace vee
