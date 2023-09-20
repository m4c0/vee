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
  VkExtent2D extent;
};
export using framebuffer = calls::handle<VkFramebuffer, &::vkCreateFramebuffer,
                                         &::vkDestroyFramebuffer>;
export inline auto create_framebuffer(const fb_params &p) {
  VkFramebufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.renderPass = p.render_pass;
  info.attachmentCount = 2;
  info.pAttachments = &p.image_buffer;
  info.layers = 1;

  if (p.surface != nullptr) {
    const auto scap = get_surface_capabilities(p.physical_device, p.surface);
    info.width = scap.currentExtent.width;
    info.height = scap.currentExtent.height;
  } else {
    info.width = p.extent.width;
    info.height = p.extent.height;
  }

  return framebuffer(&info);
}
} // namespace vee
