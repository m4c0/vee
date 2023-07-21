module;
#include "vulkan.hpp"

export module vee:alias;

export namespace vee {
using command_buffer = VkCommandBuffer;
using descriptor_set = VkDescriptorSet;
using extent = VkExtent2D;
using physical_device = VkPhysicalDevice;
using queue = VkQueue;
} // namespace vee
