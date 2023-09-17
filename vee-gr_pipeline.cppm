
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

export auto vertex_input_bind_per_instance(unsigned stride) {
  VkVertexInputBindingDescription b{};
  b.stride = stride;
  b.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
  return b;
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
export auto vertex_attribute_vec4(unsigned binding, unsigned offset) {
  VkVertexInputAttributeDescription a{};
  a.binding = binding;
  a.offset = offset;
  a.format = VK_FORMAT_R32G32B32A32_SFLOAT;
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

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.blendEnable = true;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo color_blend{};
  color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend.logicOp = VK_LOGIC_OP_COPY;
  color_blend.attachmentCount = 1;
  color_blend.pAttachments = &color_blend_attachment;

  VkPipelineDepthStencilStateCreateInfo depth_stencil{};
  depth_stencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

  VkPipelineInputAssemblyStateCreateInfo in_asm{};
  in_asm.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  in_asm.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkPipelineVertexInputStateCreateInfo vtx_in{};
  vtx_in.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vtx_in.pVertexBindingDescriptions = vb;
  vtx_in.vertexBindingDescriptionCount = B;
  vtx_in.pVertexAttributeDescriptions = va;
  vtx_in.vertexAttributeDescriptionCount = A;

  VkPipelineMultisampleStateCreateInfo multisample{};
  multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineRasterizationStateCreateInfo raster{};
  raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  raster.cullMode = VK_CULL_MODE_BACK_BIT;
  raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  raster.lineWidth = 1;
  raster.polygonMode = VK_POLYGON_MODE_FILL;

  VkPipelineViewportStateCreateInfo viewport{};
  viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport.scissorCount = 1;
  viewport.viewportCount = 1;

  VkDynamicState states[]{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic_state{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.pDynamicStates = states;
  dynamic_state.dynamicStateCount = 2;

  VkGraphicsPipelineCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.layout = pl;
  info.pColorBlendState = &color_blend;
  info.pDepthStencilState = &depth_stencil;
  info.pDynamicState = &dynamic_state;
  info.pInputAssemblyState = &in_asm;
  info.pMultisampleState = &multisample;
  info.pRasterizationState = &raster;
  info.pStages = shd;
  info.pVertexInputState = &vtx_in;
  info.pViewportState = &viewport;
  info.renderPass = rp;
  info.stageCount = S;
  return gr_pipeline(VK_NULL_HANDLE, 1, &info);
}
} // namespace vee
