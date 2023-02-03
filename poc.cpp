#include "vulkan.hpp"

import casein;
import vee;

struct stuff {
  VkSurfaceKHR surface;
  VkPhysicalDevice phys_device;
  unsigned q_family;
};

static auto &current_nptr() {
  static casein::native_handle_t i{};
  return i;
}

static stuff get_dev_stuff() {
  if (!current_nptr())
    return {};

  static auto i = vee::create_instance("my-app");
  static auto dbg = vee::create_debug_utils_messenger();
  static auto s = vee::create_surface(current_nptr());
  static const auto &[pd, qf] =
      vee::find_physical_device_with_universal_queue(*s);

  // available everywhere as a volk global
  static auto d = vee::create_single_queue_device(pd, qf);

  return {*s, pd, qf};
}

void on_window_created() {
  static const auto &[s, pd, qf] = get_dev_stuff();

  static auto q = vee::get_queue_for_family(qf);
  static auto rp = vee::create_render_pass(pd, s);
  static auto cp = vee::create_command_pool(qf);
}
void on_paint() {
  static const auto &[s, pd, qf] = get_dev_stuff();
  // we might receive a frame before vulkan is initialised
  if (!s)
    return;

  static auto swc = vee::create_swapchain(pd, s);

  static auto d_img = vee::create_depth_image(pd, s);
  static auto d_mem = vee::create_local_memory(pd, *d_img);
  static auto d_bind = vee::bind_image_memory(*d_img, *d_mem);
}

extern "C" void casein_handle(const casein::event &e) {
  switch (e.type()) {
  case casein::CREATE_WINDOW:
    current_nptr() =
        e.as<casein::events::create_window>().native_window_handle();
    on_window_created();
    break;
  case casein::REPAINT:
    on_paint();
    break;
  case casein::QUIT:
    // SDL_Quit, release shenanigans, etc
    break;
  default:
    break;
  }
}
