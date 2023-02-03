module;
#include "vulkan.hpp"

export module vee:render_pass;
import :calls;
import :surface_format;

namespace vee {
static constexpr auto create_color_attachment(VkFormat format) {
  VkAttachmentDescription res{};
  res.format = format;
  res.samples = VK_SAMPLE_COUNT_1_BIT;
  res.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  res.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  res.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  res.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  res.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  res.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  return res;
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

static constexpr auto create_color_attachment_ref() {
  VkAttachmentReference ref{};
  ref.attachment = 0;
  ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  return ref;
}
static constexpr auto create_depth_attachment_ref() {
  VkAttachmentReference ref{};
  ref.attachment = 1;
  ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  return ref;
}

static constexpr auto create_subpass(const VkAttachmentReference *color,
                                     const VkAttachmentReference *depth) {
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = color;
  subpass.pDepthStencilAttachment = depth;
  return subpass;
}

static constexpr auto create_color_dependency() {
  VkSubpassDependency dep{};

  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0;

  dep.dstSubpass = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  return dep;
}

static constexpr auto create_depth_dependency() {
  VkSubpassDependency dep{};

  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
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

export inline auto create_render_pass(VkPhysicalDevice pd, VkSurfaceKHR s) {
  static auto sfmt = find_best_surface_format(pd, s);

  const VkAttachmentDescription attachments[2]{
      create_color_attachment(sfmt.format), create_depth_attachment()};

  const auto color_attachment_ref = create_color_attachment_ref();
  const auto depth_attachment_ref = create_depth_attachment_ref();
  const auto subpass =
      create_subpass(&color_attachment_ref, &depth_attachment_ref);

  const VkSubpassDependency deps[2]{create_color_dependency(),
                                    create_depth_dependency()};

  VkRenderPassCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.attachmentCount = 2;
  info.pAttachments = attachments;
  info.subpassCount = 1;
  info.pSubpasses = &subpass;
  info.dependencyCount = 2;
  info.pDependencies = deps;
  return calls::handle<VkRenderPass, &::vkCreateRenderPass,
                       &::vkDestroyRenderPass>(&info);
}
} // namespace vee
