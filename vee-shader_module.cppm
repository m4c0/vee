export module vee:shader_module;
import :calls;
import jute;
import hai;
import silog;
import sires;
import wagen;

using namespace wagen;

namespace vee {
export using shader_module =
    calls::handle<VkShaderModule, &::vkCreateShaderModule,
                  &::vkDestroyShaderModule>;

export inline auto create_shader_module(const void *data, unsigned size) {
  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.pCode = static_cast<const unsigned *>(data);
  info.codeSize = size;
  return shader_module{&info};
}

struct resource_unavailable {};
export inline shader_module create_shader_module_from_resource(jute::view res) {
  return sires::slurp(res)
      .map([](auto &data) {
        return create_shader_module(data.begin(), data.size());
      })
      .take([res](auto e) {
        silog::log(silog::error, "Failed to load shader [%s]: %s",
                   res.cstr().data(), e.cstr().data());
        throw resource_unavailable{};
      });
}
} // namespace vee
