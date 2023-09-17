export module vee:shader_module;
import :calls;
import jute;
import hai;
import sires;
import traits;
import wagen;

using namespace wagen;

namespace vee {
export using shader_module =
    calls::handle<VkShaderModule, &::vkCreateShaderModule,
                  &::vkDestroyShaderModule>;

export inline auto create_shader_module(void *data, unsigned size) {
  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.pCode = static_cast<unsigned *>(data);
  info.codeSize = size;
  return shader_module{&info};
}

inline auto slurp(hai::uptr<yoyo::reader> &rdr) {
  return rdr->size().fmap([&](auto sz) {
    auto buf = hai::holder<char[]>::make(sz);
    return rdr->read(*buf, sz).map(
        [&] { return create_shader_module(*buf, sz); });
  });
}

struct resource_unavailable {};
export inline shader_module create_shader_module_from_resource(jute::view res) {
  return sires::open(res).fmap(slurp).take(
      [](auto) { throw resource_unavailable{}; });
}
} // namespace vee
