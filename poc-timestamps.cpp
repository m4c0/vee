#pragma leco app
#pragma leco add_shader "poc-timestamps.frag"
#pragma leco add_shader "poc-timestamps.vert"

import casein;
import hai;
import silog;
import sires;
import sith;
import traits;
import vee;
import wagen;

using namespace wagen;

struct frame_stuff {
  vee::image_view iv;
  vee::command_buffer cb;
  vee::framebuffer fb;
};

static _Atomic(bool) g_resized = false;

static struct thread : public sith::thread {
  void run() override {
    vee::instance i = vee::create_instance("vee-poc");
    vee::debug_utils_messenger dbg = vee::create_debug_utils_messenger();
    vee::surface s = vee::create_surface(casein::native_ptr);
    const auto & [pd, qf] = vee::find_physical_device_with_universal_queue(*s);
    vee::device d = vee::create_single_queue_device(pd, qf);

    vee::queue q = vee::get_queue_for_family(qf);

    vee::command_pool cp = vee::create_command_pool(qf);

    vee::render_pass rp = vee::create_render_pass({
      .attachments {{
        vee::create_colour_attachment(pd, *s),
      }},
      .subpasses {{
        vee::create_subpass({
          .colours {{ vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal) }},
        }),
      }},
      .dependencies {{
        vee::create_colour_dependency(),
      }},
    });

    vee::pipeline_layout pl = vee::create_pipeline_layout();
    vee::shader_module vert = vee::create_shader_module(sires::slurp("poc-timestamps.vert.spv"));
    vee::shader_module frag = vee::create_shader_module(sires::slurp("poc-timestamps.frag.spv"));
    vee::gr_pipeline gp = vee::create_graphics_pipeline({
      .pipeline_layout = *pl,
      .render_pass = *rp,
      .back_face_cull = false,
      .shaders {
        vee::pipeline_vert_stage(*vert, "main"),
        vee::pipeline_frag_stage(*frag, "main"),
      },
      .bindings {},
      .attributes {},
    });

    vee::semaphore img_available_sema = vee::create_semaphore();
    vee::semaphore rnd_finished_sema = vee::create_semaphore();
    vee::fence f = vee::create_fence_signaled();

    while (!interrupted()) {
      vee::swapchain swc = vee::create_swapchain(pd, *s);

      auto imgs = vee::get_swapchain_images(*swc);
      auto frms = hai::array<hai::uptr<frame_stuff>> { imgs.size() };
      auto extent = vee::get_surface_capabilities(pd, *s).currentExtent;
      for (auto i = 0; i < imgs.size(); i++) {
        auto iv = vee::create_image_view_for_surface(imgs[i], pd, *s);
        vee::fb_params fp {
          .render_pass = *rp,
          .attachments {{ *iv }},
          .extent = extent,
        };
        frms[i] = hai::uptr { new frame_stuff {
          .iv = traits::move(iv),
          .cb = vee::allocate_primary_command_buffer(*cp),
          .fb = vee::create_framebuffer(fp),
        } };
      }

      // Two metrics per swapchain frame: top and bottom of pipeline
      auto qpool = vee::create_timestamp_query_pool(2 * imgs.size());

      while (!interrupted() && !g_resized) {
        try {
          vee::wait_and_reset_fence(*f);

          auto idx = vee::acquire_next_image(*swc, *img_available_sema);
          auto & frame = frms[idx];

          {
            vee::begin_cmd_buf_one_time_submit(frame->cb);

            // Must reset between each use
            vee::cmd_reset_query_pool(frame->cb, *qpool, 2 * idx, 2);

            vee::cmd_write_timestamp(frame->cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, *qpool, idx * 2);

            vee::cmd_begin_render_pass({
              .command_buffer = frame->cb,
              .render_pass = *rp,
              .framebuffer = *frame->fb,
              .extent = extent,
              .clear_colours = { vee::clear_colour(0.1, 0.2, 0.3, 1.0) },
            }, true);
            vee::cmd_set_scissor(frame->cb, extent);
            vee::cmd_set_viewport(frame->cb, extent);
            vee::cmd_bind_gr_pipeline(frame->cb, *gp);
            vee::cmd_draw(frame->cb, 3);
            vee::cmd_end_render_pass(frame->cb);

            vee::cmd_write_timestamp(frame->cb, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, *qpool, idx * 2 + 1);

            vee::end_cmd_buf(frame->cb);
          }

          vee::queue_submit({
            .queue = q,
            .fence = *f,
            .command_buffer = frame->cb,
            .wait_semaphore = *img_available_sema,
            .signal_semaphore = *rnd_finished_sema,
          });
          vee::queue_present({
            .queue = q,
            .swapchain = *swc,
            .wait_semaphore = *rnd_finished_sema,
            .image_index = idx,
          });
        } catch (vee::out_of_date_error) {
          silog::log(silog::info, "extent is outdated");
          break;
        }
      }
      g_resized = false;
      vee::device_wait_idle();
    }
  }
} t;

static hai::uptr<sith::run_guard> tr {};
extern "C" void casein_init() {
  using namespace casein;
  handle(CREATE_WINDOW, [] { tr.reset(new sith::run_guard(&t)); });
  handle(RESIZE_WINDOW, [] { if (!casein::window_live_resize) g_resized = true; });
  handle(QUIT, [] { tr.reset(nullptr); });
}
