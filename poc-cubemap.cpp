#pragma leco app
#pragma leco add_resource_dir "assets"
#pragma leco add_shader "poc-cubemap.frag"
#pragma leco add_shader "poc-cubemap.vert"
export module poc;

import casein;
import hai;
import silog;
import sires;
import sith;
import stubby;
import sv;
import traits;
import vee;
import wagen;

using namespace wagen;

struct upc {
  float mouse_x;
  float mouse_y;
} pc;

struct frame_stuff {
  vee::image_view iv;
  vee::command_buffer cb;
  vee::framebuffer fb;
};

static _Atomic bool g_resized = false;

static class thread : public sith::thread {
public:
  void run() override {
    vee::instance i = vee::create_instance("vee-poc-cubemap");
    vee::debug_utils_messenger dbg = vee::create_debug_utils_messenger();
    vee::surface s = vee::create_surface(casein::native_ptr);
    const auto & [pd, qf] = vee::find_physical_device_with_universal_queue(*s);

    vee::device d = vee::create_single_queue_device(pd, qf, {
      .samplerAnisotropy = true,
    });

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

    vee::descriptor_set_layout dsl = vee::create_descriptor_set_layout({ vee::dsl_fragment_sampler() });

    vee::pipeline_layout pl = vee::create_pipeline_layout(*dsl, vee::vertex_push_constant_range<upc>());

    vee::shader_module vert = vee::create_shader_module(sires::slurp("poc-cubemap.vert.spv"));
    vee::shader_module frag = vee::create_shader_module(sires::slurp("poc-cubemap.frag.spv"));
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

    vee::descriptor_pool desc_pool = vee::create_descriptor_pool(1, { vee::combined_image_sampler() });
    vee::descriptor_set desc_set = vee::allocate_descriptor_set(*desc_pool, *dsl);

    // Sampler requires different V otherwise poles gets aberrations
    auto smp_info = vee::sampler_create_info().linear();
    smp_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    smp_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    vee::sampler smp = vee::create_sampler(smp_info);

    constexpr const sv files[] {
      "negx.jpg", "negy.jpg", "negz.jpg",
      "posx.jpg", "posy.jpg", "posz.jpg",
    };
    vee::buffer vs_buf {};
    vee::device_memory vs_mem {};
    unsigned img_w, img_h;
    unsigned char * ps;
    for (int i = 0; i < 6; i++) {
      auto img = stbi::load(sires::slurp(files[i]));
      if (i == 0) {
        img_w = img.width;
        img_h = img.height;
        vs_buf = vee::create_buffer(img_w * img_h * 4 * 6, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        vs_mem = vee::create_host_buffer_memory(pd, *vs_buf);
        vee::bind_buffer_memory(*vs_buf, *vs_mem);

        ps = static_cast<unsigned char *>(vee::map_memory(*vs_mem));
      }

      auto img_pxs = img_w * img_h;
      for (auto j = 0; j < img_pxs * 4; j++) *ps = (*img.data)[j];
    }
    vee::unmap_memory(*vs_mem);

    auto t_img_ci = vee::image_create_info(
        { img_w, img_h }, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    t_img_ci.arrayLayers = 6;
    t_img_ci.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    vee::image t_img { &t_img_ci };
    vee::device_memory t_mem = vee::create_local_image_memory(pd, *t_img);
    vee::bind_image_memory(*t_img, *t_mem);

    // TODO: 6-image cube equivalent
    VkImageViewCreateInfo t_iv_info {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = *t_img,
      .viewType = VK_IMAGE_VIEW_TYPE_CUBE,
      .format = VK_FORMAT_R8G8B8A8_SRGB,
      .subresourceRange {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 6,
      },
    };
    vee::image_view t_iv { &t_iv_info };

    vee::update_descriptor_set(desc_set, 0, *t_iv, *smp);

    auto ccb = vee::allocate_primary_command_buffer(*cp);
    vee::begin_cmd_buf_one_time_submit(ccb);

    auto imb = vee::image_memory_barrier(*t_img);
    imb.subresourceRange.layerCount = 6;
    imb.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vee::cmd_pipeline_barrier(ccb, 
        VK_PIPELINE_STAGE_HOST_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        imb);

    hai::array<VkBufferImageCopy> ic { 6 };
    for (auto i = 0; i < 6; i++) {
      ic[i] = vee::vk_buffer_image_copy(VK_IMAGE_ASPECT_COLOR_BIT, {}, { img_w, img_h }); 
      ic[i].imageSubresource.layerCount = 6;
    }
    vee::cmd_copy_buffer_to_image(ccb, *vs_buf, *t_img, ic.begin(), 6);

    imb = vee::image_memory_barrier(*t_img);
    imb.subresourceRange.layerCount = 6;
    imb.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imb.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imb.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vee::cmd_pipeline_barrier(ccb, 
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        imb);

    vee::end_cmd_buf(ccb);
    vee::queue_submit({ .queue = q, .command_buffer = ccb });

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

      while (!interrupted() && !g_resized) {
        try {
          vee::wait_and_reset_fence(*f);

          auto idx = vee::acquire_next_image(*swc, *img_available_sema);
          auto & frame = frms[idx];

          vee::begin_cmd_buf_one_time_submit(frame->cb);
          vee::cmd_begin_render_pass({
            .command_buffer = frame->cb,
            .render_pass = *rp,
            .framebuffer = *frame->fb,
            .extent = extent,
            .clear_colours = { vee::clear_colour(0.1, 0.2, 0.3, 1.0) },
          });

          vee::cmd_set_scissor(frame->cb, extent);
          vee::cmd_set_viewport(frame->cb, extent);
          vee::cmd_bind_descriptor_set(frame->cb, *pl, 0, desc_set);
          vee::cmd_push_vertex_constants(frame->cb, *pl, &pc);
          vee::cmd_bind_gr_pipeline(frame->cb, *gp);
          vee::cmd_draw(frame->cb, 3);

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

struct init {
  init() {
    using namespace casein;
    // Square window to avoid dealing with aspect
    window_size = { 600, 600 };
    handle(CREATE_WINDOW, [] { tr.reset(new sith::run_guard(&t)); });
    handle(MOUSE_MOVE, [] {
      pc.mouse_x = casein::mouse_pos.x;
      pc.mouse_y = casein::mouse_pos.y;
    });
    handle(RESIZE_WINDOW, [] {
      if (!casein::window_live_resize) g_resized = true;
    });
    handle(QUIT, [] { tr.reset(nullptr); });
  }
} i;
