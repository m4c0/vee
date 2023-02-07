module;
#include "vulkan.hpp"

export module vee:gr_pipeline;
import :calls;

namespace vee {
export using gr_pipeline =
    calls::handle<VkPipeline, &::vkCreateGraphicsPipelines,
                  &::vkDestroyPipeline>;
export inline auto create_graphics_pipeline(const auto in) {
  VkGraphicsPipelineCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  return gr_pipeline(VK_NULL_HANDLE, 1, &info);
}
} // namespace vee
