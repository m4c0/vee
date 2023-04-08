#define VOLK_IMPLEMENTATION
#include "vulkan.hpp"

#ifdef __APPLE__
VkInstance &volkGetLoadedInstance() {
  static VkInstance i;
  return i;
}
VkDevice &volkGetLoadedDevice() {
  static VkDevice d;
  return d;
}
void volkLoadInstance(VkInstance i) { volkGetLoadedInstance() = i; }
void volkLoadDevice(VkDevice d) { volkGetLoadedDevice() = d; }
void volkInitialize() {}
#endif
