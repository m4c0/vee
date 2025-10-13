export module vee:gr_pipeline;
import :calls;
import hai;
import no;
import traits;
import wagen;

using namespace traits::ints;
using namespace wagen;

namespace vee {
  export template<typename T, typename K> auto specialisation_map_entry(uint16_t cid, K T::*m) { 
    return VkSpecializationMapEntry {
      .constantID = cid,
      .offset = traits::offset_of<T>(m),
      .size = sizeof(K),
    };
  }

  template<typename T>
  concept non_ptr_copyable = requires (T t) {
    { t = T() };
    { t } -> traits::is_assignable_from<traits::remove_ptr_t<T>>;
  };
  export template<non_ptr_copyable T> class specialisation_info : public VkSpecializationInfo, no::no {
    hai::array<VkSpecializationMapEntry> m_entries; 
    T m_data;

  public:
    constexpr specialisation_info(T data, hai::view<VkSpecializationMapEntry> e)
      : m_entries { e.size() }
      , m_data { data }
    {
      for (auto i = 0; i < e.size(); i++) m_entries[i] = e[i];

      mapEntryCount = m_entries.size();
      pMapEntries = m_entries.begin();
      pData = &m_data;
      dataSize = sizeof(T);
    }
    constexpr specialisation_info(hai::view<VkSpecializationMapEntry> e) : specialisation_info { T {}, e } {}

    constexpr specialisation_info(T data) 
      : specialisation_info { data, { VkSpecializationMapEntry { .size = sizeof(T) } } }
    {
      m_entries[0].size = sizeof(T);
    }

    constexpr specialisation_info(uint16_t cid, T data) : specialisation_info(data) {
      m_entries[0].constantID = cid;
    }
  };

export auto pipeline_frag_stage(VkShaderModule s, const char *fn) {
  VkPipelineShaderStageCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  ci.module = s;
  ci.pName = fn;
  return ci;
}
export auto pipeline_frag_stage(VkShaderModule s, const char *fn, const VkSpecializationInfo & si) {
  auto ci = pipeline_frag_stage(s, fn);
  ci.pSpecializationInfo = &si;
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
export auto pipeline_vert_stage(VkShaderModule s, const char *fn, const VkSpecializationInfo & si) {
  auto ci = pipeline_vert_stage(s, fn);
  ci.pSpecializationInfo = &si;
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

namespace depth {
  export inline constexpr auto of(VkPipelineDepthStencilStateCreateInfo p) {
    p.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    return p;
  }
  export inline constexpr auto none() {
    return of({ .depthTestEnable = vk_false });
  }
  export inline constexpr auto op_less() {
    return of({
      .depthTestEnable  = vk_true,
      .depthWriteEnable = vk_true,
      .depthCompareOp   = VK_COMPARE_OP_LESS,
    });
  }
  export inline constexpr auto op_greater() {
    return of({
      .depthTestEnable  = vk_true,
      .depthWriteEnable = vk_true,
      .depthCompareOp   = VK_COMPARE_OP_GREATER,
    });
  }
}

export using gr_pipeline =
    calls::handle<VkPipeline, &::vkCreateGraphicsPipelines,
                  &::vkDestroyPipeline>;
export struct gr_pipeline_params {
  VkPipelineLayout pipeline_layout;
  VkRenderPass render_pass;
  VkPrimitiveTopology topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
  VkPolygonMode polygon_mode { VK_POLYGON_MODE_FILL };
  bool primitive_restart { false };
  VkExtent2D extent {};
  bool front_face_cw { false };
  bool back_face_cull{true};
  unsigned subpass { 0 };
  VkPipelineDepthStencilStateCreateInfo depth = depth::none();
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

  VkPipelineInputAssemblyStateCreateInfo in_asm{};
  in_asm.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  in_asm.topology = gpp.topology;
  in_asm.primitiveRestartEnable = gpp.primitive_restart;

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
  if (gpp.back_face_cull) raster.cullMode = VK_CULL_MODE_BACK_BIT;
  raster.frontFace = gpp.front_face_cw
    ? VK_FRONT_FACE_CLOCKWISE
    : VK_FRONT_FACE_COUNTER_CLOCKWISE;
  raster.lineWidth = 1;
  raster.polygonMode = gpp.polygon_mode;

  VkViewport vp {
    .width    = static_cast<float>(gpp.extent.width),
    .height   = static_cast<float>(gpp.extent.height),
    .maxDepth = 1,
  };
  VkRect2D sc {
    .extent = gpp.extent,
  };

  VkPipelineViewportStateCreateInfo viewport{};
  viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport.scissorCount = 1;
  viewport.viewportCount = 1;
  if (gpp.extent.width > 0) {
    viewport.pScissors  = &sc;
    viewport.pViewports = &vp;
  }

  VkDynamicState states[]{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic_state{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.pDynamicStates = states;
  dynamic_state.dynamicStateCount = 2;

  VkGraphicsPipelineCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.layout = gpp.pipeline_layout;
  info.pColorBlendState = &color_blend;
  info.pDepthStencilState = &gpp.depth;
  info.pDynamicState = gpp.extent.width > 0 ? nullptr : &dynamic_state;
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
