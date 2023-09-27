export module vee:instance;
import :calls;
import hai;
import jute;
import silog;
import wagen;

using namespace wagen;
extern "C" char *getenv(const char *);

namespace vee {
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

  hai::varray<const char *> res{2};
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
  hai::varray<const char *> res{5};
  res.push_back(vk_khr_surface_extension_name);
  res.push_back(vk_vulkan_platform_ext);

  for (auto &lp : enum_instance_ext_props(nullptr)) {
    auto name = jute::view::unsafe(lp.extensionName);
    if (name == jute::view::unsafe(vk_ext_debug_utils_extension_name)) {
      res.push_back(vk_ext_debug_utils_extension_name);
      silog::log(silog::info, "Enabling debug utils");
    }
#ifdef LECO_TARGET_MACOSX
    if (name ==
        jute::view::unsafe(vk_khr_portability_enumeration_extension_name)) {
      res.push_back(vk_khr_portability_enumeration_extension_name);
      silog::log(silog::info, "Enabling portability extension");
    }
#endif
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
  app_info.apiVersion = vk_api_version_1_0;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.enabledExtensionCount = extensions.size();
  create_info.enabledLayerCount = layers.size();
  create_info.pApplicationInfo = &app_info;
  create_info.ppEnabledExtensionNames = extensions.begin();
  create_info.ppEnabledLayerNames = layers.begin();

#ifdef __APPLE__
  // TODO: decide if support for other non-conformant Vulkan devices
  create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  instance res{&create_info};
  wagen::instance() = *res;
  silog::log(silog::info, "Vulkan instance created");
  return res;
}

} // namespace vee
