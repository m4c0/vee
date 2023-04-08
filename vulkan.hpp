#pragma once
#ifdef __APPLE__
#define VK_USE_PLATFORM_METAL_EXT
#define VEE_VULKAN_PLATFORM_EXT VK_EXT_METAL_SURFACE_EXTENSION_NAME
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#define VEE_VULKAN_PLATFORM_EXT VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#define VEE_VULKAN_PLATFORM_EXT VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#else
#error Unsupported platform
#endif

#ifdef __APPLE__
#include "vulkan-headers/include/vulkan/vulkan_core.h"
#include "vulkan-headers/include/vulkan/vulkan_metal.h"

VkInstance &volkGetLoadedInstance();
VkDevice &volkGetLoadedDevice();
void volkLoadInstance(VkInstance i);
void volkLoadDevice(VkDevice d);
void volkInitialize();

#else
#include "volk/volk.h"
#endif
