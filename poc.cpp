#include "vulkan.hpp"

import casein;
import vee;

static auto &current_nptr() {
  static casein::native_handle_t i{};
  return i;
}

struct dev_stuff {
  vee::instance i = vee::create_instance("my-app");
  vee::debug_utils_messenger dbg = vee::create_debug_utils_messenger();
  vee::surface s = vee::create_surface(current_nptr());
  vee::physical_device_pair pdqf =
      vee::find_physical_device_with_universal_queue(*s);
  vee::device d =
      vee::create_single_queue_device(pdqf.physical_device, pdqf.queue_family);
};
static dev_stuff &get_dev_stuff() {
  static dev_stuff x{};
  return x;
}

struct ext_stuff {
  VkPhysicalDevice pd;
  VkSurfaceKHR s;

  vee::swapchain swc = vee::create_swapchain(pd, s);

  vee::image d_img = vee::create_depth_image(pd, s);
  vee::device_memory d_mem = vee::create_local_memory(pd, *d_img);
  decltype(nullptr) d_bind = vee::bind_image_memory(*d_img, *d_mem);
  vee::image_view d_iv = vee::create_depth_image_view(*d_img);
};
static ext_stuff &get_ext_stuff() {
  static const auto &[pd, qf] = get_dev_stuff().pdqf;
  static auto *s = *get_dev_stuff().s;

  static ext_stuff x{pd, s};
  return x;
}

void on_window_created() {
  static const auto &[pd, qf] = get_dev_stuff().pdqf;
  static auto *s = *get_dev_stuff().s;

  static auto q = vee::get_queue_for_family(qf);
  static auto rp = vee::create_render_pass(pd, s);
  static auto cp = vee::create_command_pool(qf);
}
void on_paint() {
  // we might receive a frame before vulkan is initialised
  if (!current_nptr())
    return;

  get_ext_stuff();
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
