export module vee:render_pass;
import :calls;
import :image;
import :surface_format;
import hai;
import wagen;

using namespace wagen;

namespace vee {
  export enum image_layout {
    image_layout_undefined                        = VK_IMAGE_LAYOUT_UNDEFINED,
    image_layout_attachment_optimal               = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
    image_layout_color_attachment_optimal         = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    image_layout_depth_stencil_attachment_optimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    image_layout_present_src_khr                  = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    image_layout_read_only_optimal                = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    image_layout_shader_read_only_optimal         = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    image_layout_transfer_src_optimal             = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
  };

  export enum attachment_load_op {
    attachment_load_op_clear     = VK_ATTACHMENT_LOAD_OP_CLEAR,
    attachment_load_op_dont_care = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    attachment_load_op_load      = VK_ATTACHMENT_LOAD_OP_LOAD,
  };
  export enum attachment_store_op {
    attachment_store_op_dont_care = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    attachment_store_op_store     = VK_ATTACHMENT_STORE_OP_STORE,
  };
  export struct attachment_description {
    VkFormat format;
    attachment_load_op load_op   = attachment_load_op_clear;
    attachment_store_op store_op = attachment_store_op_store;
    image_layout initial_layout;
    image_layout final_layout;
  };
  export [[nodiscard]] constexpr auto create_colour_attachment(const attachment_description & d) {
    VkAttachmentDescription res{};
    res.format = static_cast<VkFormat>(d.format);
    res.samples = VK_SAMPLE_COUNT_1_BIT;
    res.loadOp = static_cast<VkAttachmentLoadOp>(d.load_op);
    res.storeOp = static_cast<VkAttachmentStoreOp>(d.store_op);
    res.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    res.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    res.initialLayout = static_cast<VkImageLayout>(d.initial_layout);
    res.finalLayout = static_cast<VkImageLayout>(d.final_layout);
    return res;
  }
  export [[nodiscard]] constexpr auto create_colour_attachment(VkFormat fmt, vee::image_layout il) {
    return create_colour_attachment({
      .format = fmt,
      .load_op = attachment_load_op_clear,
      .store_op = attachment_store_op_store,
      .final_layout = il,
    });
  }
  export [[nodiscard]] constexpr auto create_colour_attachment(VkPhysicalDevice pd, VkSurfaceKHR s) {
    return create_colour_attachment({
      .format = find_best_surface_format(pd, s).format,
      .load_op = attachment_load_op_clear,
      .store_op = attachment_store_op_store,
      .final_layout = image_layout_present_src_khr,
    });
  }

  static constexpr auto create_depth_attachment() {
    VkAttachmentDescription res{};
    res.format = VK_FORMAT_D32_SFLOAT;
    res.samples = VK_SAMPLE_COUNT_1_BIT;
    res.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    res.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    res.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    res.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    res.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    res.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    return res;
  }

  static constexpr auto create_attachment_ref(unsigned att, VkImageLayout il) {
    VkAttachmentReference ref{};
    ref.attachment = att;
    ref.layout = il;
    return ref;
  }
  export inline constexpr auto create_attachment_ref(unsigned att, vee::image_layout il) {
    return create_attachment_ref(att, static_cast<VkImageLayout>(il));
  }

  export struct subpass_description {
    hai::array<VkAttachmentReference> colours;
    hai::array<VkAttachmentReference> inputs;
    VkAttachmentReference depth_stencil;
  };
  export inline constexpr auto create_subpass(const subpass_description & d) {
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = d.colours.size();
    subpass.pColorAttachments = d.colours.begin();
    if (d.depth_stencil.layout) subpass.pDepthStencilAttachment = &d.depth_stencil;
    subpass.inputAttachmentCount = d.inputs.size();
    subpass.pInputAttachments = d.inputs.begin();
    return subpass;
  }

  export constexpr const auto subpass_external = vk_subpass_external;
  export enum pipeline_stage_flags {
    pipeline_stage_color_attachment_output = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    pipeline_stage_early_fragment_tests    = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    pipeline_stage_fragment_shader         = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    pipeline_stage_late_fragment_tests     = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
  };
  export enum access_flags {
    access_color_attachment_write         = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    access_depth_stencil_attachment_read  = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    access_depth_stencil_attachment_write = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    access_input_attachment_read          = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
  };
  export enum dependency_flags {
    dependency_by_region = VK_DEPENDENCY_BY_REGION_BIT,
  };
  export struct subpass_dependency {
    unsigned src_subpass;
    unsigned src_stage_mask;
    unsigned src_access_mask;
    unsigned dst_subpass;
    unsigned dst_stage_mask;
    unsigned dst_access_mask;
    unsigned dependency;
  };
  export inline constexpr auto create_dependency(const subpass_dependency & d) {
    VkSubpassDependency dep {};
    dep.srcSubpass = d.src_subpass;
    dep.dstSubpass = d.dst_subpass;
    dep.srcStageMask = d.src_stage_mask;
    dep.dstStageMask = d.dst_stage_mask;
    dep.srcAccessMask = d.src_access_mask;
    dep.dstAccessMask = d.dst_access_mask;
    dep.dependencyFlags = d.dependency;
    return dep;
  }
  export inline constexpr auto create_colour_dependency() {
    return create_dependency({
      .src_subpass = vk_subpass_external,
      .src_stage_mask = pipeline_stage_color_attachment_output,
      .dst_stage_mask = pipeline_stage_color_attachment_output,
      .dst_access_mask = access_color_attachment_write,
    });
  }
  static constexpr auto create_depth_dependency() {
    return create_dependency({
      .src_subpass = vk_subpass_external,
      .src_stage_mask = pipeline_stage_early_fragment_tests
                      | pipeline_stage_late_fragment_tests,
      .src_access_mask = access_depth_stencil_attachment_write,
      .dst_stage_mask = pipeline_stage_early_fragment_tests
                      | pipeline_stage_late_fragment_tests,
      .dst_access_mask = access_depth_stencil_attachment_read
                       | access_depth_stencil_attachment_write,
    });
  }

  export using render_pass = calls::handle<VkRenderPass, &::vkCreateRenderPass, &::vkDestroyRenderPass>;
  
  export struct create_render_pass_params {
    hai::array<VkAttachmentDescription> attachments;
    hai::array<VkSubpassDescription> subpasses;
    hai::array<VkSubpassDependency> dependencies;
  };
  export inline auto create_render_pass(const create_render_pass_params & p) {
    VkRenderPassCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = p.attachments.size();
    info.pAttachments = p.attachments.begin();
    info.subpassCount = p.subpasses.size();
    info.pSubpasses = p.subpasses.begin();
    info.dependencyCount = p.dependencies.size();
    info.pDependencies = p.dependencies.begin();
    return render_pass { &info };
  }

  export inline auto create_render_pass(VkFormat fmt, vee::image_layout il) {
    return create_render_pass({
      .attachments {{
        create_colour_attachment(fmt, il),
        create_depth_attachment(),
      }},
      .subpasses {{
        create_subpass({
          .colours {{ create_attachment_ref(0, image_layout_color_attachment_optimal) }},
          .depth_stencil = create_attachment_ref(1, image_layout_depth_stencil_attachment_optimal),
        }),
      }},
      .dependencies {{
        create_colour_dependency(),
        create_depth_dependency(),
      }},
    });
  }
  export inline auto create_render_pass(VkPhysicalDevice pd, VkSurfaceKHR s) {
    return create_render_pass({
      .attachments {{
        create_colour_attachment(pd, s),
        create_depth_attachment(),
      }},
      .subpasses {{
        create_subpass({
          .colours {{ create_attachment_ref(0, image_layout_color_attachment_optimal) }},
          .depth_stencil = create_attachment_ref(1, image_layout_depth_stencil_attachment_optimal),
        }),
      }},
      .dependencies {{
        create_colour_dependency(),
        create_depth_dependency(),
      }},
    });
  }
} // namespace vee
