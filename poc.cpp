#include "vulkan.hpp"

import casein;
import hai;
import vee;

struct device_stuff {
  casein::native_handle_t nptr;

  vee::instance i = vee::create_instance("my-app");
  vee::debug_utils_messenger dbg = vee::create_debug_utils_messenger();
  vee::surface s = vee::create_surface(nptr);
  vee::physical_device_pair pdqf =
      vee::find_physical_device_with_universal_queue(*s);
  vee::device d =
      vee::create_single_queue_device(pdqf.physical_device, pdqf.queue_family);
};

struct extent_stuff {
  VkPhysicalDevice pd;
  VkSurfaceKHR s;
  unsigned qf;

  vee::command_pool cp = vee::create_command_pool(qf);
  vee::render_pass rp = vee::create_render_pass(pd, s);
  vee::swapchain swc = vee::create_swapchain(pd, s);

  vee::image d_img = vee::create_depth_image(pd, s);
  vee::device_memory d_mem = vee::create_local_memory(pd, *d_img);
  decltype(nullptr) d_bind = vee::bind_image_memory(*d_img, *d_mem);
  vee::image_view d_iv = vee::create_depth_image_view(*d_img);
};

struct inflight_stuff {
  VkCommandPool pool;
  VkCommandBuffer cb = vee::allocate_secondary_command_buffer(pool);

  vee::semaphore img_available_sema{};
  vee::semaphore rnd_finished_sema{};
  vee::fence f{};
};
struct inflights {
  unsigned qf;

  vee::command_pool cp = vee::create_command_pool(qf);

  inflight_stuff front{*cp};
  inflight_stuff back{*cp};
};

struct frame_stuff {
  const extent_stuff *xs;
  VkImage img;

  VkCommandBuffer cb = vee::allocate_primary_command_buffer(*xs->cp);

  vee::image_view iv = vee::create_rgba_image_view(img, xs->pd, xs->s);

  vee::fb_params fbp{
      .physical_device = xs->pd,
      .surface = xs->s,
      .render_pass = *xs->rp,
      .image_buffer = *iv,
      .depth_buffer = *xs->d_iv,
  };
  vee::framebuffer fb = vee::create_framebuffer(fbp);
};

/*
void on_window_created() {
  static const auto &[pd, qf] = get_device_stuff().pdqf;
  static auto s = *get_device_stuff().s;

  static auto q = vee::get_queue_for_family(qf);
}
*/

extern "C" void casein_handle(const casein::event &e) {
  static volatile casein::native_handle_t nptr{};
  static hai::uptr<device_stuff> dev{};
  static hai::uptr<extent_stuff> ext{};
  static hai::uptr<inflights> infs{};
  static hai::holder<hai::uptr<frame_stuff>[]> frms{};

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    nptr = e.as<casein::events::create_window>().native_window_handle();
    break;
  case casein::REPAINT:
    // we might receive a frame before vulkan is initialised
    if (nptr && !dev) {
      dev = hai::uptr<device_stuff>::make(nptr);
      const auto &[pd, qf] = dev->pdqf;
      ext = hai::uptr<extent_stuff>::make(pd, *dev->s, qf);
      infs = hai::uptr<inflights>::make(qf);

      auto imgs = vee::get_swapchain_images(*ext->swc);
      frms = decltype(frms)::make(imgs.size());
      for (auto i = 0; i < imgs.size(); i++) {
        (*frms)[i] = hai::uptr<frame_stuff>::make(&*ext, (imgs.data())[i]);
      }
    }
    break;
  case casein::QUIT:
    frms = {};
    infs = {};
    ext = {};
    dev = {};
    break;
  default:
    break;
  }
}
