module;
#include "vulkan.hpp"

export module vee:alias;

namespace vee {
export using command_buffer = VkCommandBuffer;
export using descriptor_set = VkDescriptorSet;
export using extent = VkExtent2D;
export using physical_device = VkPhysicalDevice;
export using queue = VkQueue;
} // namespace vee
