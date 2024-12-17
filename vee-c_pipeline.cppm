export module vee:c_pipeline;
import :calls;
import hai;
import traits;
import wagen;

using namespace wagen;

namespace vee {
  export using c_pipeline = calls::handle<VkPipeline, &::vkCreateComputePipelines, &::vkDestroyPipeline>;

  export inline auto create_compute_pipeline(VkPipelineLayout layout, VkShaderModule s, const char * fn) {
    VkPipelineShaderStageCreateInfo stage{};
    stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage.module = s;
    stage.pName = fn;

    VkComputePipelineCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.layout = layout;
    info.stage = stage;
    return c_pipeline(vk_null_handle, 1, &info);
  }
}
