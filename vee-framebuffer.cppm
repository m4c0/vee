export module vee:framebuffer;
import :calls;
import :surface_capabilities;
import hai;
import wagen;

using namespace wagen;

namespace vee {
export struct fb_params {
  VkRenderPass render_pass;
  hai::array<VkImageView> attachments;
  VkExtent2D extent;
  unsigned layers = 1;
};
export using framebuffer = calls::handle<VkFramebuffer, &::vkCreateFramebuffer,
                                         &::vkDestroyFramebuffer>;
export inline auto create_framebuffer(const fb_params &p) {
  VkFramebufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.renderPass = p.render_pass;
  info.attachmentCount = p.attachments.size();
  info.pAttachments = p.attachments.begin();
  info.layers = p.layers;
  info.width = p.extent.width;
  info.height = p.extent.height;

  return framebuffer(&info);
}
} // namespace vee
