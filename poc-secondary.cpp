#pragma leco app
#pragma leco add_shader "poc-secondary.frag"
#pragma leco add_shader "poc-secondary.vert"

import casein;
import dotz;
import hai;
import silog;
import sires;
import sith;
import traits;
import vee;

struct frame_stuff {
  vee::image_view iv;
  vee::command_buffer cb;
  vee::framebuffer fb;
};

static class thread : public sith::thread {
public:
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

    vee::shader_module vert = vee::create_shader_module(sires::slurp("poc-secondary.vert.spv"));
    vee::shader_module frag = vee::create_shader_module(sires::slurp("poc-secondary.frag.spv"));
    vee::gr_pipeline gp = vee::create_graphics_pipeline({
      .pipeline_layout = *pl,
      .render_pass = *rp,
      .shaders {
        vee::pipeline_vert_stage(*vert, "main"),
        vee::pipeline_frag_stage(*frag, "main"),
      },
      .bindings {
        vee::vertex_input_bind(sizeof(dotz::vec2)),
      },
      .attributes {
        vee::vertex_attribute_vec2(0, 0),
      },
    });

    vee::buffer v_buf = vee::create_buffer(sizeof(dotz::vec2) * 3, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    vee::device_memory v_mem = vee::create_host_buffer_memory(pd, *v_buf);
    vee::bind_buffer_memory(*v_buf, *v_mem);

    vee::semaphore img_available_sema = vee::create_semaphore();
    vee::semaphore rnd_finished_sema = vee::create_semaphore();
    vee::fence f = vee::create_fence_signaled();

    auto vs = static_cast<dotz::vec2 *>(vee::map_memory(*v_mem));
    vs[0] = { -1, -1 };
    vs[1] = { 0, 1 };
    vs[2] = { 1, -1 };
    vee::unmap_memory(*v_mem);

    auto extent = vee::get_surface_capabilities(pd, *s).currentExtent;

    /////////////////////////////////////////////////////////////////
    // Secondary buffer recording
    /////////////////////////////////////////////////////////////////

    vee::render_pass srp = vee::create_render_pass({
      .attachments {{
        vee::create_passthru_colour_attachment(pd, *s),
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

    vee::command_buffer scb = vee::allocate_secondary_command_buffer(*cp);

    vee::begin_cmd_buf_render_pass_continue(scb, *srp);
    vee::cmd_set_scissor(scb, extent);
    vee::cmd_set_viewport(scb, extent);
    vee::cmd_bind_gr_pipeline(scb, *gp);
    vee::cmd_bind_vertex_buffers(scb, 0, *v_buf);
    vee::cmd_draw(scb, 3);
    vee::end_cmd_buf(scb);

    /////////////////////////////////////////////////////////////////

    while (!interrupted()) {
      vee::swapchain swc = vee::create_swapchain(pd, *s);

      auto imgs = vee::get_swapchain_images(*swc);
      auto frms = hai::array<hai::uptr<frame_stuff>> { imgs.size() };
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

      while (!interrupted()) {
        try {
          vee::wait_and_reset_fence(*f);

          auto idx = vee::acquire_next_image(*swc, *img_available_sema);
          auto & frame = frms[idx];

          {
            vee::begin_cmd_buf_one_time_submit(frame->cb);
            vee::cmd_begin_render_pass({
              .command_buffer = frame->cb,
              .render_pass = *rp,
              .framebuffer = *frame->fb,
              .extent = extent,
              .clear_colours = { vee::clear_colour(0.1, 0.2, 0.3, 1.0) },
            }, false);
            vee::cmd_execute_command(frame->cb, scb);
            vee::cmd_end_render_pass(frame->cb);
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
      vee::device_wait_idle();
    }
  }
} t;
static hai::uptr<sith::run_guard> tr {};

struct init {
  init() {
    using namespace casein;
    handle(CREATE_WINDOW, [] { tr.reset(new sith::run_guard(&t)); });
    handle(QUIT, [] { tr.reset(nullptr); });
  }
} i;
