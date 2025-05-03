#pragma leco app
#pragma leco add_shader "poc.frag"
#pragma leco add_shader "poc.vert"
export module poc;

import casein;
import hai;
import silog;
import sires;
import sith;
import traits;
import vee;

struct point {
  float x;
  float y;
  float z;
  float w;
};
struct rgba {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};
struct upc {
  float mouse_x;
  float mouse_y;
  float factor;
  float vert_scale;
};

struct frame_stuff {
  vee::image_view iv;
  vee::command_buffer cb;
  vee::framebuffer fb;
};

static upc pc {};

static void mouse_move() {
  pc.mouse_x = casein::mouse_pos.x;
  pc.mouse_y = casein::mouse_pos.y;
}

static volatile bool gv_resized {};
static void resize_window() {
  pc.factor = casein::screen_scale_factor;

  if (!casein::window_live_resize) gv_resized = true;
}

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
    vee::render_pass rp = vee::create_render_pass(pd, *s);

    vee::descriptor_set_layout dsl = vee::create_descriptor_set_layout({ vee::dsl_fragment_sampler() });

    vee::pipeline_layout pl = vee::create_pipeline_layout(*dsl, vee::vert_frag_push_constant_range<upc>());

    float k = 1;
    vee::shader_module vert = vee::create_shader_module_from_resource("poc.vert.spv");
    vee::shader_module frag = vee::create_shader_module_from_resource("poc.frag.spv");
    vee::gr_pipeline gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = *rp,
        .shaders {
            vee::pipeline_vert_stage(*vert, "main"),
            vee::pipeline_frag_stage(*frag, "main", vee::specialisation_info<float>(&k)),
        },
        .bindings {
            vee::vertex_input_bind(sizeof(point)),
        },
        .attributes {
            vee::vertex_attribute_vec2(0, 0),
        },
    });

    vee::descriptor_pool desc_pool = vee::create_descriptor_pool(1, { vee::combined_image_sampler() });
    vee::descriptor_set desc_set = vee::allocate_descriptor_set(*desc_pool, *dsl);

    vee::sampler smp = vee::create_sampler(vee::nearest_sampler);

    vee::buffer ts_buf = vee::create_transfer_src_buffer(16 * 16 * sizeof(float));
    vee::device_memory ts_mem = vee::create_host_buffer_memory(pd, *ts_buf);
    vee::bind_buffer_memory(*ts_buf, *ts_mem);

    vee::image t_img = vee::create_image({ 16, 16 }, VK_FORMAT_R8G8B8A8_SRGB);
    vee::device_memory t_mem = vee::create_local_image_memory(pd, *t_img);
    vee::bind_image_memory(*t_img, *t_mem);
    vee::image_view t_iv = vee::create_image_view(*t_img, VK_FORMAT_R8G8B8A8_SRGB);

    vee::buffer v_buf = vee::create_vertex_buffer(sizeof(point) * 3);
    vee::device_memory v_mem = vee::create_host_buffer_memory(pd, *v_buf);
    vee::bind_buffer_memory(*v_buf, *v_mem);

    vee::update_descriptor_set(desc_set, 0, *t_iv, *smp);

    vee::command_buffer cb = vee::allocate_secondary_command_buffer(*cp);

    vee::semaphore img_available_sema = vee::create_semaphore();
    vee::semaphore rnd_finished_sema = vee::create_semaphore();
    vee::fence f = vee::create_fence_signaled();

    auto vs = static_cast<point *>(vee::map_memory(*v_mem));
    vs[0] = { -1, -1 };
    vs[1] = { 0, 1 };
    vs[2] = { 1, -1 };
    vee::unmap_memory(*v_mem);

    auto ps = static_cast<rgba *>(vee::map_memory(*ts_mem));
    for (auto i = 0; i < 16 * 16; i++) {
      ps[i] = { 255, 255, 255, static_cast<unsigned char>(i) };
    }
    vee::unmap_memory(*ts_mem);

    pc.vert_scale = 0.8;

    while (!interrupted()) {
      vee::swapchain swc = vee::create_swapchain(pd, *s);

      vee::image d_img = vee::create_depth_image(pd, *s);
      vee::device_memory d_mem = vee::create_local_image_memory(pd, *d_img);
      vee::bind_image_memory(*d_img, *d_mem);
      vee::image_view d_iv = vee::create_depth_image_view(*d_img);

      auto imgs = vee::get_swapchain_images(*swc);
      auto frms = hai::array<hai::uptr<frame_stuff>> { imgs.size() };
      auto extent = vee::get_surface_capabilities(pd, *s).currentExtent;
      for (auto i = 0; i < imgs.size(); i++) {
        auto iv = vee::create_image_view_for_surface(imgs[i], pd, *s);
        vee::fb_params fp {
          .render_pass = *rp,
          .attachments {{ *iv, *d_iv }},
          .extent = extent,
        };
        frms[i] = hai::uptr { new frame_stuff {
            .iv = traits::move(iv),
            .cb = vee::allocate_primary_command_buffer(*cp),
            .fb = vee::create_framebuffer(fp),
        } };
      }

      while (!interrupted() && !gv_resized) {
        try {
          vee::wait_and_reset_fence(*f);

          auto idx = vee::acquire_next_image(*swc, *img_available_sema);
          auto & frame = frms[idx];

          {
            vee::begin_cmd_buf_render_pass_continue(cb, *rp);
            vee::cmd_set_scissor(cb, extent);
            vee::cmd_set_viewport(cb, extent);
            vee::cmd_push_vert_frag_constants(cb, *pl, &pc);
            vee::cmd_bind_descriptor_set(cb, *pl, 0, desc_set);
            vee::cmd_bind_gr_pipeline(cb, *gp);
            vee::cmd_bind_vertex_buffers(cb, 0, *v_buf);
            vee::cmd_draw(cb, 3);
            vee::end_cmd_buf(cb);
          }
          {
            vee::begin_cmd_buf_one_time_submit(frame->cb);
            vee::cmd_pipeline_barrier(frame->cb, *t_img, vee::from_host_to_transfer);
            vee::cmd_copy_buffer_to_image(frame->cb, { 16, 16 }, *ts_buf, *t_img);
            vee::cmd_pipeline_barrier(frame->cb, *t_img, vee::from_transfer_to_fragment);
            vee::cmd_begin_render_pass({
                .command_buffer = frame->cb,
                .render_pass = *rp,
                .framebuffer = *frame->fb,
                .extent = extent,
                .clear_colours = { vee::clear_colour(0.1, 0.2, 0.3, 1.0) },
                .use_secondary_cmd_buf = true,
            });
            vee::cmd_execute_command(frame->cb, cb);
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
      gv_resized = false;
      vee::device_wait_idle();
    }
  }
} t;
static hai::uptr<sith::run_guard> tr {};

struct init {
  init() {
    using namespace casein;
    handle(CREATE_WINDOW, [] { tr.reset(new sith::run_guard(&t)); });
    handle(MOUSE_MOVE, &mouse_move);
    handle(RESIZE_WINDOW, &resize_window);
    handle(QUIT, [] { tr.reset(nullptr); });
  }
} i;
