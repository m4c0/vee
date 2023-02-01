module;
extern "C" char *getenv(const char *);

export module vee:instance;
import :vulkan;
import hai;
import jute;
import silog;

namespace vee {
template <typename Tp> class vec {
  hai::holder<Tp[]> m_data;
  unsigned m_max;
  unsigned m_size{};

public:
  constexpr explicit vec(unsigned max)
      : m_data{hai::holder<Tp[]>::make(max)}, m_max{max} {}

  constexpr void push_back(Tp v) {
    if (m_size >= m_max) {
      silog::log(silog::error, "Vector overflow");
      return;
    }
    (*m_data)[m_size++] = v;
  }

  [[nodiscard]] constexpr auto data() const noexcept { return *m_data; }
  [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
};

static bool is_api_dump_requested() {
  const char *env = getenv("VEE_VULKAN_API_DUMP");
  if (env != nullptr) {
    silog::log(silog::info, "API dump requested");
    return true;
  }
  return false;
}
static auto get_layers() {
  auto api_dump = is_api_dump_requested();

  vec<const char *> res{2};
  for (auto &lp : vk::enum_instance_layer_props()) {
    jute::view layer_name{lp.layerName};
    if (layer_name == "VK_LAYER_KHRONOS_validation") {
      res.push_back("VK_LAYER_KHRONOS_validation");
      silog::log(silog::info, "Enabling validation layer");
    } else if (api_dump && layer_name == "VK_LAYER_LUNARG_api_dump") {
      res.push_back("VK_LAYER_LUNARG_api_dump");
      silog::log(silog::info, "Enabling API dump");
    }
  }
  return res;
}

static auto get_extensions() {
  vec<const char *> res{3};
  res.push_back(vk::khr_surf_ext_name.data());
  res.push_back(vk::plat_surf_ext_name.data());

  for (auto &lp : vk::enum_instance_ext_props(nullptr)) {
    jute::view name{lp.extensionName};
    if (name == vk::debug_utils_ext_name) {
      res.push_back(vk::debug_utils_ext_name.data());
      silog::log(silog::info, "Enabling debug utils");
    }
  }
  return res;
}

export inline auto create_instance(const char *app_name) {
  auto layers = get_layers();
  auto extensions = get_extensions();

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = 1;
  app_info.pEngineName = "m4c0/vee";
  app_info.engineVersion = 1;
  app_info.apiVersion = vk::api_version;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  create_info.enabledLayerCount = layers.size();
  create_info.ppEnabledLayerNames = layers.data();

  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = extensions.data();

  return objects::instance(&create_info);
}
} // namespace vee
