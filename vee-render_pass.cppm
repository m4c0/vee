export module vee:render_pass;
import :calls;
import :image;
import :surface_format;
import hai;
import wagen;

using namespace wagen;

namespace vee {
  export enum image_layout {
    image_layout_read_only_optimal        = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    image_layout_shader_read_only_optimal = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    image_layout_transfer_src_optimal     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
  };

static constexpr auto create_colour_attachment(VkFormat format, VkImageLayout final_il) {
  VkAttachmentDescription res{};
  res.format = format;
  res.samples = VK_SAMPLE_COUNT_1_BIT;
  res.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  res.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  res.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  res.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  res.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  res.finalLayout = final_il;
  return res;
}
export [[nodiscard]] constexpr auto create_colour_attachment(vee::image_format fmt, vee::image_layout l) {
  return create_colour_attachment(static_cast<VkFormat>(fmt), static_cast<VkImageLayout>(l));
}
export [[nodiscard]] constexpr auto create_colour_attachment(VkPhysicalDevice pd, VkSurfaceKHR s) {
  return create_colour_attachment(find_best_surface_format(pd, s).format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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

static constexpr auto create_ref(unsigned att, VkImageLayout il) {
  VkAttachmentReference ref{};
  ref.attachment = att;
  ref.layout = il;
  return ref;
}

static constexpr auto create_subpass(auto & colour, VkAttachmentReference * depth) {
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = colour.size();
  subpass.pColorAttachments = colour.begin();
  subpass.pDepthStencilAttachment = depth;
  return subpass;
}

static constexpr auto create_colour_dependency() {
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
  hai::array<VkAttachmentReference> refs { colour_attachments.size() }; 
  for (auto i = 0; i < colour_attachments.size(); i++) refs[i] = create_ref(i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  auto depth_ref = create_ref(colour_attachments.size(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

  create_render_pass_params p {
    .attachments { colour_attachments.size() + 1 },
    .subpasses {{ create_subpass(refs, &depth_ref) }},
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
  hai::array<VkAttachmentReference> refs { colour_attachments.size() }; 
  for (auto i = 0; i < colour_attachments.size(); i++) refs[i] = create_ref(i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  create_render_pass_params p {
    .attachments { colour_attachments.size() },
    .subpasses {{ create_subpass(refs, nullptr) }},
    .dependencies {{ create_colour_dependency() }},
  };

  for (auto i = 0; i < colour_attachments.size(); i++) p.attachments[i] = colour_attachments[i];

  return create_render_pass(p);
}

export inline auto create_render_pass(VkPhysicalDevice pd, VkSurfaceKHR s) {
  return create_render_pass({{ create_colour_attachment(pd, s) }});
}
} // namespace vee
