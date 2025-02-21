export module vee:debug_utils_messenger;
import :calls;
import jute;
import silog;
import traits;
import wagen;

static jute::view message_type(VkDebugUtilsMessageTypeFlagsEXT type) {
  switch (type) {
  case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
    return "[vulkan] [general]";
  case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
    return "[vulkan] [performance]";
  case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
    return "[vulkan] [validation]";
  default:
    return "[vulkan] [unknown]";
  }
}

static auto message(VkDebugUtilsMessageTypeFlagsEXT type,
                    const VkDebugUtilsMessengerCallbackDataEXT *data) {
  return message_type(type) + jute::view{" "} +
         jute::view::unsafe(data->pMessage);
}

constexpr inline auto level_for_severity(auto severity) {
  switch (severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    return silog::error;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    return silog::warning;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    return silog::info;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    return silog::debug;
  default:
    return silog::info;
  }
}

static VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                               VkDebugUtilsMessageTypeFlagsEXT type,
                               const VkDebugUtilsMessengerCallbackDataEXT *data,
                               [[maybe_unused]] void *user_data) {
  silog::log(level_for_severity(severity), "%s",
             message(type, data).cstr().data());
  return vk_false;
}

namespace vee {
export using debug_utils_messenger =
    calls::handle<VkDebugUtilsMessengerEXT, &::vkCreateDebugUtilsMessengerEXT,
                  &::vkDestroyDebugUtilsMessengerEXT>;
export inline auto create_debug_utils_messenger() {
#ifndef __APPLE__
  if (vkCreateDebugUtilsMessengerEXT == nullptr)
    return debug_utils_messenger{};
#endif

  VkDebugUtilsMessengerCreateInfoEXT info{};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.pfnUserCallback = callback<debug_callback>;
  info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

  return debug_utils_messenger{&info};
}

export void set_debug_utils_object_name(VkObjectType type, auto handle, const char * name) {
  VkDebugUtilsObjectNameInfoEXT info {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
    .objectType = type,
    .objectHandle = (traits::ints::uint64_t)handle,
    .pObjectName = name,
  };
  calls::call(vkSetDebugUtilsObjectNameEXT, &info);
}
export void set_debug_utils_object_name(VkImage img, const char * name) {
  set_debug_utils_object_name(VK_OBJECT_TYPE_IMAGE, img, name);
}
} // namespace vee
