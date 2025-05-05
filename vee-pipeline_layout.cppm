export module vee:pipeline_layout;
import :calls;
import hai;
import wagen;

using namespace wagen;

namespace vee {
export template <typename Tp>
inline auto compute_push_constant_range(unsigned offset = 0) {
  VkPushConstantRange r;
  r.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  r.offset = offset;
  r.size = sizeof(Tp);
  return r;
}
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

export struct pipeline_layout_params {
  hai::array<VkDescriptorSetLayout> descriptor_set_layouts;
  hai::array<VkPushConstantRange> push_constant_ranges;
};
export inline auto create_pipeline_layout(const pipeline_layout_params & p) {
  VkPipelineLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = p.descriptor_set_layouts.size();
  info.pSetLayouts = p.descriptor_set_layouts.begin();
  info.pushConstantRangeCount = p.push_constant_ranges.size();
  info.pPushConstantRanges = p.push_constant_ranges.begin();
  return pipeline_layout{&info};
}

export inline auto create_pipeline_layout(VkDescriptorSetLayout dsl) {
  return create_pipeline_layout({{{ dsl }}, {}});
}
export inline auto create_pipeline_layout(VkPushConstantRange pcr) {
  return create_pipeline_layout({{}, {{ pcr }}});
}
export inline auto create_pipeline_layout(VkDescriptorSetLayout dsl,
                                          VkPushConstantRange pcr) {
  return create_pipeline_layout(pipeline_layout_params {
    {{ dsl }},
    {{ pcr }},
  });
}
} // namespace vee
