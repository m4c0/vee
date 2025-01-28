export module vee:render_pass;
import :calls;
import :image;
import :surface_format;
import hai;
import wagen;

using namespace wagen;

namespace vee {
  export enum image_layout {
    image_layout_undefined                 = VK_IMAGE_LAYOUT_UNDEFINED,
    image_layout_color_attachment_optimal  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    image_layout_present_src_khr           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    image_layout_read_only_optimal         = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    image_layout_shader_read_only_optimal  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    image_layout_transfer_src_optimal      = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
  };

  export struct attachment_description {
    vee::image_format format;
    vee::image_layout initial_layout;
    vee::image_layout final_layout;
  };
  export [[nodiscard]] constexpr auto create_colour_attachment(const attachment_description & d) {
    VkAttachmentDescription res{};
    res.format = static_cast<VkFormat>(d.format);
    res.samples = VK_SAMPLE_COUNT_1_BIT;
    res.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    res.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    res.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    res.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    res.initialLayout = static_cast<VkImageLayout>(d.initial_layout);
    res.finalLayout = static_cast<VkImageLayout>(d.final_layout);
    return res;
  }
  export [[nodiscard]] constexpr auto create_colour_attachment(VkPhysicalDevice pd, VkSurfaceKHR s) {
    return create_colour_attachment({
      .format = static_cast<vee::image_format>(find_best_surface_format(pd, s).format),
      .final_layout = image_layout_present_src_khr,
    });
  }

static constexpr auto create_depth_attachment() {
  VkAttachmentDescription res{};
  res.format = VK_FORMAT_D32_SFLOAT;
  res.samples = VK_SAMPLE_COUNT_1_BIT;
  res.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  res.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  res.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  res.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  res.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  res.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  return res;
}

static constexpr auto create_attachment_ref(unsigned att, VkImageLayout il) {
  VkAttachmentReference ref{};
  ref.attachment = att;
  ref.layout = il;
  return ref;
}
  export inline constexpr auto create_attachment_ref(unsigned att, vee::image_layout il) {
    return create_attachment_ref(att, static_cast<VkImageLayout>(il));
  }

  export struct subpass_description {
    hai::array<VkAttachmentReference> colours;
    hai::array<VkAttachmentReference> inputs;
    VkAttachmentReference depth_stencil;
  };
  export inline constexpr auto create_subpass(const subpass_description & d) {
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = d.colours.size();
    subpass.pColorAttachments = d.colours.begin();
    if (d.depth_stencil.layout) subpass.pDepthStencilAttachment = &d.depth_stencil;
    subpass.inputAttachmentCount = d.inputs.size();
    subpass.pInputAttachments = d.inputs.begin();
    return subpass;
  }

export inline constexpr auto create_colour_dependency() {
  VkSubpassDependency dep{};

  dep.srcSubpass = vk_subpass_external;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0;

  dep.dstSubpass = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  return dep;
}

static constexpr auto create_depth_dependency() {
  VkSubpassDependency dep{};

  dep.srcSubpass = vk_subpass_external;
  dep.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                     VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dep.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  dep.dstSubpass = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                     VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dep.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  return dep;
}

export using render_pass = calls::handle<VkRenderPass, &::vkCreateRenderPass, &::vkDestroyRenderPass>;

export struct create_render_pass_params {
  hai::array<VkAttachmentDescription> attachments;
  hai::array<VkSubpassDescription> subpasses;
  hai::array<VkSubpassDependency> dependencies;
};
export inline auto create_render_pass(const create_render_pass_params & p) {
  VkRenderPassCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.attachmentCount = p.attachments.size();
  info.pAttachments = p.attachments.begin();
  info.subpassCount = p.subpasses.size();
  info.pSubpasses = p.subpasses.begin();
  info.dependencyCount = p.dependencies.size();
  info.pDependencies = p.dependencies.begin();
  return render_pass { &info };
}

export inline auto create_render_pass(hai::array<VkAttachmentDescription> colour_attachments) {
  auto depth_ref = create_attachment_ref(colour_attachments.size(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  subpass_description s {
    .colours { colour_attachments.size() },
    .depth_stencil = depth_ref,
  };
  for (auto i = 0; i < colour_attachments.size(); i++) s.colours[i] = create_attachment_ref(i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  create_render_pass_params p {
    .attachments { colour_attachments.size() + 1 },
    .subpasses {{ create_subpass(s) }},
    .dependencies {{
      create_colour_dependency(),
      create_depth_dependency(),
    }},
  };

  for (auto i = 0; i < colour_attachments.size(); i++) p.attachments[i] = colour_attachments[i];
  p.attachments[colour_attachments.size()] = create_depth_attachment();

  return create_render_pass(p);
}

export inline auto create_depthless_render_pass(hai::array<VkAttachmentDescription> colour_attachments) {
  subpass_description s {
    .colours { colour_attachments.size() },
  };
  for (auto i = 0; i < colour_attachments.size(); i++) s.colours[i] = create_attachment_ref(i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  create_render_pass_params p {
    .attachments { colour_attachments.size() },
    .subpasses {{ create_subpass(s) }},
    .dependencies {{ create_colour_dependency() }},
  };

  for (auto i = 0; i < colour_attachments.size(); i++) p.attachments[i] = colour_attachments[i];

  return create_render_pass(p);
}

export inline auto create_render_pass(VkPhysicalDevice pd, VkSurfaceKHR s) {
  return create_render_pass({{ create_colour_attachment(pd, s) }});
}
} // namespace vee
