module;
#include "vulkan.hpp"

export module vee:gr_pipeline;
import :calls;

namespace vee {
export auto pipeline_frag_stage(VkShaderModule s, const char *fn) {
  VkPipelineShaderStageCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  ci.module = s;
  ci.pName = fn;
  return ci;
}
export auto pipeline_vert_stage(VkShaderModule s, const char *fn) {
  VkPipelineShaderStageCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
  ci.module = s;
  ci.pName = fn;
  return ci;
}

export using gr_pipeline =
    calls::handle<VkPipeline, &::vkCreateGraphicsPipelines,
                  &::vkDestroyPipeline>;
export template <unsigned S>
inline auto
create_graphics_pipeline(VkPipelineLayout pl, VkRenderPass rp,
                         const VkPipelineShaderStageCreateInfo (&shd)[S]) {
  VkPipelineInputAssemblyStateCreateInfo in_asm{};
  in_asm.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  in_asm.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkPipelineVertexInputStateCreateInfo vtx_in{};
  vtx_in.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkPipelineRasterizationStateCreateInfo raster{};
  raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  raster.cullMode = VK_CULL_MODE_BACK_BIT;
  raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  raster.lineWidth = 1;
  raster.polygonMode = VK_POLYGON_MODE_FILL;
  raster.rasterizerDiscardEnable = VK_TRUE;

  VkGraphicsPipelineCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.layout = pl;
  info.pInputAssemblyState = &in_asm;
  info.pRasterizationState = &raster;
  info.pStages = shd;
  info.pVertexInputState = &vtx_in;
  info.renderPass = rp;
  info.stageCount = S;
  return gr_pipeline(VK_NULL_HANDLE, 1, &info);
}
} // namespace vee
