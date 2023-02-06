#include "vulkan.hpp"

import casein;
import hai;
import traits;
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

  VkQueue q = vee::get_queue_for_family(pdqf.queue_family);
};

struct extent_stuff {
  VkPhysicalDevice pd;
  VkSurfaceKHR s;
  unsigned qf;

  VkExtent2D extent = vee::get_surface_capabilities(pd, s).currentExtent;

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

  vee::semaphore img_available_sema = vee::create_semaphore();
  vee::semaphore rnd_finished_sema = vee::create_semaphore();
  vee::fence f = vee::create_fence_signaled();
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

inline void flip(inflights &i) {
  auto tmp = traits::move(i.front);
  i.front = traits::move(i.back);
  i.back = traits::move(tmp);
}

enum states {
  waiting_nptr,
  setup_stuff,
  ready_to_paint,
  done,
};

extern "C" void casein_handle(const casein::event &e) {
  static volatile casein::native_handle_t nptr{};
  static hai::uptr<device_stuff> dev{};
  static hai::uptr<extent_stuff> ext{};
  static hai::uptr<inflights> infs{};
  static hai::holder<hai::uptr<frame_stuff>[]> frms{};
  static states state = waiting_nptr;

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    switch (state) {
    case waiting_nptr:
      vee::initialise();
      nptr = e.as<casein::events::create_window>().native_window_handle();
      dev = hai::uptr<device_stuff>::make(nptr);
      break;
    default:
      break;
    }
    state = setup_stuff;
    break;
  case casein::REPAINT:
    switch (state) {
    case setup_stuff: {
      const auto &[pd, qf] = dev->pdqf;
      ext = hai::uptr<extent_stuff>::make(pd, *dev->s, qf);
      infs = hai::uptr<inflights>::make(qf);

      auto imgs = vee::get_swapchain_images(*ext->swc);
      frms = decltype(frms)::make(imgs.size());
      for (auto i = 0; i < imgs.size(); i++) {
        (*frms)[i] = hai::uptr<frame_stuff>::make(&*ext, (imgs.data())[i]);
      }

      state = ready_to_paint;
      break;
    }
    case ready_to_paint: {
      try {
        flip(*infs);

        auto &inf = infs->back;
        vee::wait_and_reset_fence(*inf.f);

        auto idx = vee::acquire_next_image(*ext->swc, *inf.img_available_sema);
        auto &frame = (*frms)[idx];

        {
          vee::begin_cmd_buf_render_pass_continue(inf.cb, *ext->rp);
          vee::end_cmd_buf(inf.cb);
        }
        {
          vee::begin_cmd_buf_one_time_submit(frame->cb);
          vee::cmd_begin_render_pass({
              .command_buffer = frame->cb,
              .render_pass = *ext->rp,
              .framebuffer = *frame->fb,
              .extent = ext->extent,
          });
          vee::cmd_execute_command(frame->cb, inf.cb);
          vee::cmd_end_render_pass(frame->cb);
          vee::end_cmd_buf(frame->cb);
        }

        vee::queue_submit({
            .queue = dev->q,
            .fence = *inf.f,
            .command_buffer = frame->cb,
            .wait_semaphore = *inf.img_available_sema,
            .signal_semaphore = *inf.rnd_finished_sema,
        });
        vee::queue_present({
            .queue = dev->q,
            .swapchain = *ext->swc,
            .wait_semaphore = *inf.rnd_finished_sema,
            .image_index = idx,
        });
      } catch (vee::out_of_date_error) {
        state = setup_stuff;
        vee::device_wait_idle();
      }
      break;
    }
    default:
      break;
    }
    break;
  case casein::QUIT:
    vee::device_wait_idle();
    frms = {};
    infs = {};
    ext = {};
    dev = {};
    state = done;
    break;
  default:
    break;
  }
}
