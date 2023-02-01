module;
#include "vulkan.hpp"

export module vee:debug_utils_messenger;
import :calls;
import jute;
import silog;

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

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
               VkDebugUtilsMessageTypeFlagsEXT type,
               const VkDebugUtilsMessengerCallbackDataEXT *data,
               [[maybe_unused]] void *user_data) {
  silog::log(level_for_severity(severity), message(type, data).cstr().data());
  return VK_FALSE;
}

namespace vee {
export inline auto create_debug_utils_messenger() {
  using res_t =
      calls::handle<VkDebugUtilsMessengerEXT, &::vkCreateDebugUtilsMessengerEXT,
                    &::vkDestroyDebugUtilsMessengerEXT>;

  if (vkCreateDebugUtilsMessengerEXT == nullptr)
    return res_t{};

  VkDebugUtilsMessengerCreateInfoEXT info{};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.pfnUserCallback = debug_callback;
  info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

  return res_t{&info};
}
} // namespace vee
