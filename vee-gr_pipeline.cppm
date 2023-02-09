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

export auto vertex_input_bind(unsigned stride) {
  VkVertexInputBindingDescription b{};
  b.stride = stride;
  return b;
}
export auto vertex_attribute_vec2(unsigned binding, unsigned offset) {
  VkVertexInputAttributeDescription a{};
  a.binding = binding;
  a.offset = offset;
  a.format = VK_FORMAT_R32G32_SFLOAT;
  return a;
}

export using gr_pipeline =
    calls::handle<VkPipeline, &::vkCreateGraphicsPipelines,
                  &::vkDestroyPipeline>;
export template <unsigned S, unsigned B, unsigned A>
inline auto
create_graphics_pipeline(VkPipelineLayout pl, VkRenderPass rp,
                         const VkPipelineShaderStageCreateInfo (&shd)[S],
                         VkVertexInputBindingDescription (&&vb)[B],
                         VkVertexInputAttributeDescription (&&va)[A]) {
  for (unsigned i = 0; i < B; i++) {
    vb[i].binding = i;
  }
  for (unsigned i = 0; i < A; i++) {
    va[i].location = i;
  }

  VkPipelineInputAssemblyStateCreateInfo in_asm{};
  in_asm.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  in_asm.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkPipelineVertexInputStateCreateInfo vtx_in{};
  vtx_in.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vtx_in.pVertexBindingDescriptions = vb;
  vtx_in.vertexBindingDescriptionCount = B;
  vtx_in.pVertexAttributeDescriptions = va;
  vtx_in.vertexAttributeDescriptionCount = A;

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
