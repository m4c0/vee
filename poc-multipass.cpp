#pragma leco app
#pragma leco add_shader "poc-multipass.frag"
#pragma leco add_shader "poc-multipass.vert"
export module poc;

import casein;
import dotz;
import hai;
import sith;
import traits;
import vee;

// Note: this is just an example of using a render pass with multiple
// subpasses. It lacks some Vulkan infra, like restarting the swapchain when it
// resizes, etc.

static class thread : public sith::thread {
  struct frame_stuff {
    vee::image_view iv;
    vee::command_buffer cb;
    vee::framebuffer fb;
  };

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
        vee::create_colour_attachment({
          .format = vee::find_best_surface_image_format(pd, *s),
          .initial_layout = vee::image_layout_attachment_optimal,
          .final_layout   = vee::image_layout_read_only_optimal,
        }),
      }},
      .subpasses {{ 
        vee::create_subpass({
          .colours {{ vee::create_attachment_ref(0, vee::image_layout_attachment_optimal) }},
        }),
        vee::create_subpass({
          .colours {{ vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal) }},
        }),
      }},
      .dependencies {{ vee::create_colour_dependency() }},
    });

    vee::pipeline_layout pl = vee::create_pipeline_layout();
    vee::shader_module vert = vee::create_shader_module_from_resource("poc-multipass.vert.spv");
    vee::shader_module frag = vee::create_shader_module_from_resource("poc-multipass.frag.spv");
    vee::gr_pipeline gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = *rp,
        .shaders {
            vee::pipeline_vert_stage(*vert, "main"),
            vee::pipeline_frag_stage(*frag, "main"),
        },
        .bindings { vee::vertex_input_bind(sizeof(dotz::vec2)) },
        .attributes { vee::vertex_attribute_vec2(0, 0) },
    });

    vee::buffer v_buf = vee::create_vertex_buffer(sizeof(dotz::vec2) * 6);
    vee::device_memory v_mem = vee::create_host_buffer_memory(pd, *v_buf);
    vee::bind_buffer_memory(*v_buf, *v_mem);

    vee::semaphore img_available_sema = vee::create_semaphore();
    vee::semaphore rnd_finished_sema = vee::create_semaphore();
    vee::fence f = vee::create_fence_signaled();

    auto vs = static_cast<dotz::vec2 *>(vee::map_memory(*v_mem));
    *vs++ = { -1, -1 }; *vs++ = {  1,  1 }; *vs++ = {  1, -1 };
    *vs++ = { -1, -1 }; *vs++ = { -1,  1 }; *vs++ = {  1,  1 };
    vee::unmap_memory(*v_mem);

    while (!interrupted()) {
      vee::swapchain swc = vee::create_swapchain(pd, *s);

      auto imgs = vee::get_swapchain_images(*swc);
      auto frms = hai::array<hai::uptr<frame_stuff>> { imgs.size() };
      for (auto i = 0; i < imgs.size(); i++) {
        auto iv = vee::create_rgba_image_view(imgs[i], pd, *s);
        vee::fb_params fp {
          .physical_device = pd,
          .surface = *s,
          .render_pass = *rp,
          .attachments {{ *iv }},
        };
        frms[i] = hai::uptr { new frame_stuff {
            .iv = traits::move(iv),
            .cb = vee::allocate_primary_command_buffer(*cp),
            .fb = vee::create_framebuffer(fp),
        } };
      }

      vee::extent extent = vee::get_surface_capabilities(pd, *s).currentExtent;
      while (!interrupted()) {
        vee::wait_and_reset_fence(*f);

        auto idx = vee::acquire_next_image(*swc, *img_available_sema);
        auto & frame = frms[idx];

        vee::begin_cmd_buf_one_time_submit(frame->cb);
        vee::cmd_begin_render_pass({
            .command_buffer = frame->cb,
            .render_pass = *rp,
            .framebuffer = *frame->fb,
            .extent = extent,
        });
        vee::cmd_set_scissor(frame->cb, extent);
        vee::cmd_set_viewport(frame->cb, extent);
        vee::cmd_bind_gr_pipeline(frame->cb, *gp);
        vee::cmd_bind_vertex_buffers(frame->cb, 0, *v_buf);
        vee::cmd_draw(frame->cb, 6);
        vee::cmd_next_subpass(frame->cb);
        vee::cmd_draw(frame->cb, 6);
        vee::cmd_end_render_pass(frame->cb);
        vee::end_cmd_buf(frame->cb);

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
