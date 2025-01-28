export module vee:alias;
import wagen;

using namespace wagen;

export namespace vee {
  using command_buffer = VkCommandBuffer;
  using descriptor_set = VkDescriptorSet;
  using extent = VkExtent2D;
  using offset = VkOffset2D;
  using physical_device = VkPhysicalDevice;
  using queue = VkQueue;
  using rect = VkRect2D;
  
  enum image_format {
    image_format_r8 = VK_FORMAT_R8_UNORM,
    image_format_srgba = VK_FORMAT_R8G8B8A8_SRGB,
    image_format_rgba_uint = VK_FORMAT_R8G8B8A8_UINT,
    image_format_rgba_unorm = VK_FORMAT_R8G8B8A8_UNORM,
    image_format_rgba_uscaled = VK_FORMAT_R8G8B8A8_USCALED,
    image_format_yuv420p = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
  };
} // namespace vee
