export module vee:instance;
import :vulkan;

namespace vee {
static auto create_instance(const char *app_name) {
  // auto layers = get_layers();
  // auto extensions = get_extensions();

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = 1;
  app_info.pEngineName = "m4c0/vee";
  app_info.engineVersion = 1;
  app_info.apiVersion = vk_api_version;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  create_info.enabledLayerCount = 0;
  create_info.enabledExtensionCount = 0;
  // create_info.enabledLayerCount = layers.size();
  // create_info.ppEnabledLayerNames = layers.data();

  // create_info.enabledExtensionCount = extensions.size();
  // create_info.ppEnabledExtensionNames = extensions.data();

  return calls::create_instance(create_info);
}

export class instance {
public:
  explicit instance(const char *app_name) { initialise(); }
};
} // namespace vee
