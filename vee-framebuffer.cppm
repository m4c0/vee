export module vee:framebuffer;
import :calls;
import :surface_capabilities;
import wagen;

using namespace wagen;

namespace vee {
export struct fb_params {
  VkPhysicalDevice physical_device;
  VkSurfaceKHR surface;
  VkRenderPass render_pass;
  VkImageView image_buffer;
  VkImageView depth_buffer;
};
export using framebuffer = calls::handle<VkFramebuffer, &::vkCreateFramebuffer,
                                         &::vkDestroyFramebuffer>;
export inline auto create_framebuffer(const fb_params &p) {
  const auto scap = get_surface_capabilities(p.physical_device, p.surface);

  VkFramebufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.width = scap.currentExtent.width;
  info.height = scap.currentExtent.height;
  info.renderPass = p.render_pass;
  info.attachmentCount = 2;
  info.pAttachments = &p.image_buffer;
  info.layers = 1;
  return framebuffer(&info);
}
} // namespace vee
