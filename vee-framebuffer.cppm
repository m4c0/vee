module;
#include "vulkan.hpp"

export module vee:framebuffer;
import :calls;

namespace vee {
export struct fb_params {
  VkExtent2D extent;
  VkRenderPass render_pass;
  VkImageView depth_buffer;
  VkImageView image_buffer;
};
export using framebuffer = calls::handle<VkFramebuffer, &::vkCreateFramebuffer,
                                         &::vkDestroyFramebuffer>;
export inline auto create_framebuffer(const fb_params &p) {
  VkFramebufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.width = p.extent.width;
  info.height = p.extent.height;
  info.renderPass = p.render_pass;
  info.attachmentCount = 2;
  info.pAttachments = &p.depth_buffer;
  info.layers = 1;
  return framebuffer(&info);
}
} // namespace vee
