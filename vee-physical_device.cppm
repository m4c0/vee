module;
#include "vulkan.hpp"

export module vee:physical_device;
import :calls;
import jute;
import silog;

namespace vee {
static constexpr const auto enum_physical_devices =
    calls::enumerate<&::vkEnumeratePhysicalDevices, VkPhysicalDevice>();
static constexpr const auto get_pd_queue_family_props =
    calls::enumerate<&::vkGetPhysicalDeviceQueueFamilyProperties,
                     VkQueueFamilyProperties>();

static bool is_surface_compatible(VkPhysicalDevice pd, VkSurfaceKHR s) {
  uint32_t count{};

  vkGetPhysicalDeviceSurfaceFormatsKHR(pd, s, &count, nullptr);
  if (count == 0)
    return false;

  vkGetPhysicalDeviceSurfacePresentModesKHR(pd, s, &count, nullptr);
  if (count == 0)
    return false;

  VkPhysicalDeviceFeatures f{};
  vkGetPhysicalDeviceFeatures(pd, &f);
  return f.samplerAnisotropy == VK_TRUE;
}

static bool get_queue_family(VkPhysicalDevice pd, VkSurfaceKHR s,
                             unsigned *idx) {
  auto queues = get_pd_queue_family_props(pd);
  for (*idx = 0; *idx < queues.size(); (*idx)++) {
    auto q = (queues.data())[*idx];

    if ((q.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
      continue;
    }
    if ((q.queueFlags & VK_QUEUE_TRANSFER_BIT) == 0) {
      continue;
    }

    VkBool32 surf_support{};
    vkGetPhysicalDeviceSurfaceSupportKHR(pd, *idx, s, &surf_support);
    if (surf_support == VK_TRUE) {
      return true;
    }
  }
  return false;
}

static void log_rejected_device(VkPhysicalDevice pd, jute::view reason) {
  VkPhysicalDeviceProperties props{};
  vkGetPhysicalDeviceProperties(pd, &props);

  using namespace jute::literals;
  auto msg = "Rejecting device ["_s + jute::view::unsafe(props.deviceName) +
             "] - reason: "_s + reason;

  silog::log(silog::warning, msg.cstr().data());
}

export struct physical_device_pair {
  VkPhysicalDevice physical_device;
  unsigned queue_family;
};
export inline auto find_physical_device_with_universal_queue(VkSurfaceKHR s) {
  for (auto pd : enum_physical_devices()) {
    unsigned qf{};
    if (!get_queue_family(pd, s, &qf)) {
      log_rejected_device(pd, "Missing compatible queue family");
      continue;
    }

    if (!is_surface_compatible(pd, s)) {
      log_rejected_device(pd, "Missing compatible surface");
      continue;
    }

    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(pd, &props);

    using namespace jute::literals;
    auto msg = "Using device: "_s + jute::view::unsafe(props.deviceName);
    silog::log(silog::info, msg.cstr().data());

    return physical_device_pair{pd, qf};
  }

  silog::log(silog::error, "No suitable physical devices found");
  return physical_device_pair{};
}
} // namespace vee
