export module vee:shader_module;
import :calls;
import jute;
import hai;
import wagen;

using namespace wagen;

namespace vee {
  export using shader_module = calls::handle<VkShaderModule, &::vkCreateShaderModule, &::vkDestroyShaderModule>;

  export inline auto create_shader_module(const void *data, unsigned size) {
    VkShaderModuleCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pCode = static_cast<const unsigned *>(data);
    info.codeSize = size;
    return shader_module{&info};
  }
  export inline auto create_shader_module(jute::view data) {
    return create_shader_module(data.begin(), data.size());
  }
} // namespace vee
