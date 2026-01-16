export module vee:swapchain;
import :calls;
import :surface_capabilities;
import :surface_format;
import wagen;

using namespace wagen;

namespace vee {
static constexpr const auto get_pd_surf_present_modes =
    calls::enumerate<&::vkGetPhysicalDeviceSurfacePresentModesKHR,
                     VkPresentModeKHR>();

static inline constexpr auto get_min_image_count(const VkSurfaceCapabilitiesKHR &cap) {
  auto count = cap.minImageCount + 1;
  if ((cap.maxImageCount > 0) && (count > cap.maxImageCount)) {
    return cap.maxImageCount;
  }
  return count;
}

static inline auto get_present_mode(VkPhysicalDevice pd, VkSurfaceKHR s, bool vsync) {
  if (!vsync) {
    // "immediate" pumps frames as fast as possible, but it might tear
    for (auto mode : get_pd_surf_present_modes(pd, s)) {
      if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) return mode;
    }
  }

  // "mailbox" discards images if CPU is faster than GPU
  for (auto mode : get_pd_surf_present_modes(pd, s)) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) return mode;
  }
  // "fifo" is guaranteed by Vulkan spec
  return VK_PRESENT_MODE_FIFO_KHR;
}

export using swapchain = calls::handle<VkSwapchainKHR, &::vkCreateSwapchainKHR,
                                       &::vkDestroySwapchainKHR>;
export inline auto create_swapchain(VkPhysicalDevice pd, VkSurfaceKHR s, bool vsync = true) {
  const auto cap = get_surface_capabilities(pd, s);
  const auto present_mode = get_present_mode(pd, s, vsync);
  const auto format = find_best_surface_format(pd, s);

  VkSwapchainCreateInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.clipped = vk_true;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // graphics_q = present_q
  info.imageExtent = cap.currentExtent;
  info.presentMode = present_mode;
  info.surface = s;
  info.minImageCount = get_min_image_count(cap);
  info.preTransform = cap.currentTransform;
  info.imageColorSpace = format.colorSpace;
  info.imageFormat = format.format;
  return swapchain(&info);
}

export inline constexpr const auto get_swapchain_images =
    calls::enumerate<&::vkGetSwapchainImagesKHR, VkImage>();
} // namespace vee
