module;
#include "vulkan.hpp"
extern "C" char *getenv(const char *);

export module vee:instance;
import :calls;
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
static constexpr const auto enum_instance_layer_props =
    calls::enumerate<&::vkEnumerateInstanceLayerProperties,
                     VkLayerProperties>();
static constexpr const auto enum_instance_ext_props =
    calls::enumerate<&::vkEnumerateInstanceExtensionProperties,
                     VkExtensionProperties>();
static auto get_layers() {
  auto api_dump = is_api_dump_requested();

  vec<const char *> res{2};
  for (auto &lp : enum_instance_layer_props()) {
    auto layer_name = jute::view::unsafe(lp.layerName);
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
  vec<const char *> res{5};
  res.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  res.push_back(VEE_VULKAN_PLATFORM_EXT);

#ifdef __APPLE__
  // TODO: decide if support for other non-conformant Vulkan devices
  res.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  res.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

  for (auto &lp : enum_instance_ext_props(nullptr)) {
    auto name = jute::view::unsafe(lp.extensionName);
    if (name == jute::view{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}) {
      res.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      silog::log(silog::info, "Enabling debug utils");
    }
  }
  return res;
}

export using instance =
    calls::handle<VkInstance, &::vkCreateInstance, &::vkDestroyInstance>;
export inline auto create_instance(const char *app_name) {
  auto layers = get_layers();
  auto extensions = get_extensions();

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = 1;
  app_info.pEngineName = "m4c0/vee";
  app_info.engineVersion = 1;
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.enabledExtensionCount = extensions.size();
  create_info.enabledLayerCount = layers.size();
  create_info.pApplicationInfo = &app_info;
  create_info.ppEnabledExtensionNames = extensions.data();
  create_info.ppEnabledLayerNames = layers.data();

#ifdef __APPLE__
  // TODO: decide if support for other non-conformant Vulkan devices
  create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  instance res{&create_info};
  volkLoadInstance(*res);
  silog::log(silog::info, "Vulkan instance created");
  return res;
}

export inline void initialise() { vee::calls::call(volkInitialize); }
} // namespace vee
