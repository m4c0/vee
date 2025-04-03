export module vee:gr_pipeline;
import :calls;
import hai;
import silog;
import traits;
import wagen;

using namespace wagen;

namespace vee {
  export template<typename T>
  auto specialisation_map_entry() {
    VkSpecializationMapEntry res {};
    res.size = sizeof(T);
    return res;
  }
  export template<typename K> auto specialisation_info(K * data, hai::view<VkSpecializationMapEntry> map) {
    unsigned sz = 0;
    for (auto i = 0; i < map.size(); i++) {
      map[i].constantID = i;
      map[i].offset = sz;
      sz += map[i].size;
    }
    VkSpecializationInfo res {};
    res.mapEntryCount = map.size();
    res.pMapEntries = map.begin();
    res.pData = data;
    res.dataSize = sizeof(K);
    return res;
  }

export auto pipeline_frag_stage(VkShaderModule s, const char *fn, VkSpecializationInfo * si = {}) {
  VkPipelineShaderStageCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  ci.module = s;
  ci.pName = fn;
  ci.pSpecializationInfo = si;
  return ci;
}
export auto pipeline_vert_stage(VkShaderModule s, const char *fn, VkSpecializationInfo * si = {}) {
  VkPipelineShaderStageCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
  ci.module = s;
  ci.pName = fn;
  ci.pSpecializationInfo = si;
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
export auto vertex_attribute_uint(unsigned binding, unsigned offset) {
  VkVertexInputAttributeDescription a{};
  a.binding = binding;
  a.offset = offset;
  a.format = VK_FORMAT_R32_UINT;
  return a;
}
export auto vertex_attribute_float(unsigned binding, unsigned offset) {
  VkVertexInputAttributeDescription a{};
  a.binding = binding;
  a.offset = offset;
  a.format = VK_FORMAT_R32_SFLOAT;
  return a;
}
export auto vertex_attribute_vec2(unsigned binding, unsigned offset) {
  VkVertexInputAttributeDescription a{};
  a.binding = binding;
  a.offset = offset;
  a.format = VK_FORMAT_R32G32_SFLOAT;
  return a;
}
export auto vertex_attribute_vec3(unsigned binding, unsigned offset) {
  VkVertexInputAttributeDescription a{};
  a.binding = binding;
  a.offset = offset;
  a.format = VK_FORMAT_R32G32B32_SFLOAT;
  return a;
}
export auto vertex_attribute_vec4(unsigned binding, unsigned offset) {
  VkVertexInputAttributeDescription a{};
  a.binding = binding;
  a.offset = offset;
  a.format = VK_FORMAT_R32G32B32A32_SFLOAT;
  return a;
}

export auto colour_blend_none() {
  VkPipelineColorBlendAttachmentState b {};
  b.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  return b;
}
export auto colour_blend_classic() {
  VkPipelineColorBlendAttachmentState b {};
  b.blendEnable = true;
  b.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  b.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  b.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  return b;
}

export using gr_pipeline =
    calls::handle<VkPipeline, &::vkCreateGraphicsPipelines,
                  &::vkDestroyPipeline>;
export struct gr_pipeline_params {
  VkPipelineLayout pipeline_layout;
  VkRenderPass render_pass;
  VkPrimitiveTopology topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
  VkPolygonMode polygon_mode { VK_POLYGON_MODE_FILL };
  bool back_face_cull{true};
  bool depth_test{true};
  unsigned subpass { 0 };
  hai::view<VkPipelineColorBlendAttachmentState> blends { colour_blend_classic() };
  hai::view<VkPipelineShaderStageCreateInfo> shaders;
  hai::view<VkVertexInputBindingDescription> bindings;
  hai::view<VkVertexInputAttributeDescription> attributes;
};
export inline auto create_graphics_pipeline(gr_pipeline_params &&gpp) {
  for (unsigned i = 0; i < gpp.bindings.size(); i++) {
    gpp.bindings[i].binding = i;
  }
  for (unsigned i = 0; i < gpp.attributes.size(); i++) {
    gpp.attributes[i].location = i;
  }

  VkPipelineColorBlendStateCreateInfo color_blend{};
  color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend.logicOp = VK_LOGIC_OP_COPY;
  color_blend.attachmentCount = gpp.blends.size();
  color_blend.pAttachments = gpp.blends.begin();

  VkPipelineDepthStencilStateCreateInfo depth_stencil{};
  depth_stencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = gpp.depth_test ? vk_true : vk_false;
  depth_stencil.depthWriteEnable = vk_true;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;

  VkPipelineInputAssemblyStateCreateInfo in_asm{};
  in_asm.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  in_asm.topology = gpp.topology;

  VkPipelineVertexInputStateCreateInfo vtx_in{};
  vtx_in.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vtx_in.pVertexBindingDescriptions = gpp.bindings.begin();
  vtx_in.vertexBindingDescriptionCount = gpp.bindings.size();
  vtx_in.pVertexAttributeDescriptions = gpp.attributes.begin();
  vtx_in.vertexAttributeDescriptionCount = gpp.attributes.size();

  VkPipelineMultisampleStateCreateInfo multisample{};
  multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineRasterizationStateCreateInfo raster{};
  raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  if (gpp.back_face_cull)
    raster.cullMode = VK_CULL_MODE_BACK_BIT;
  raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  raster.lineWidth = 1;
  raster.polygonMode = gpp.polygon_mode;

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
  info.layout = gpp.pipeline_layout;
  info.pColorBlendState = &color_blend;
  info.pDepthStencilState = &depth_stencil;
  info.pDynamicState = &dynamic_state;
  info.pInputAssemblyState = &in_asm;
  info.pMultisampleState = &multisample;
  info.pRasterizationState = &raster;
  info.pStages = gpp.shaders.begin();
  info.pVertexInputState = &vtx_in;
  info.pViewportState = &viewport;
  info.renderPass = gpp.render_pass;
  info.subpass = gpp.subpass;
  info.stageCount = gpp.shaders.size();
  return gr_pipeline(vk_null_handle, 1, &info);
}
} // namespace vee
