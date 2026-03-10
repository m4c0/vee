export module vee:render_pass;
import :calls;
import :image;
import :surface_format;
import hai;
import wagen;

using namespace wagen;

namespace vee {
  export inline constexpr auto attachment_description() {
    return VkAttachmentDescription {
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    };
  }
  export [[nodiscard]] constexpr auto colour_attachment(VkFormat fmt, VkImageLayout final_layout) {
    return VkAttachmentDescription {
      .format         = fmt,
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .finalLayout    = final_layout,
    };
  }
  export [[nodiscard]] constexpr auto colour_attachment(VkPhysicalDevice pd, VkSurfaceKHR s) {
    return colour_attachment(find_best_surface_format(pd, s).format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  }

  /// Creates a "pass-through" colour attachment. That attachment will support
  /// loading and store, without changing the layout. This is suitable for a
  /// secondary command buffer or an intermediate render pass.
  export [[nodiscard]] constexpr auto passthru_colour_attachment(VkPhysicalDevice pd, VkSurfaceKHR s) {
    return VkAttachmentDescription {
      .format         = find_best_surface_format(pd, s).format,
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD,
      .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
  }

  export inline constexpr auto depth_attachment() {
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
  export inline constexpr auto depth_stencil_attachment() {
    return VkAttachmentDescription {
      .format = VK_FORMAT_D32_SFLOAT_S8_UINT,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
  }

  export inline constexpr auto attachment_ref(unsigned att, VkImageLayout il) {
    VkAttachmentReference ref{};
    ref.attachment = att;
    ref.layout = il;
    return ref;
  }

  export struct subpass_description {
    hai::array<VkAttachmentReference> colours;
    hai::array<VkAttachmentReference> inputs;
    VkAttachmentReference depth_stencil;
    hai::array<VkAttachmentReference> resolves;
    hai::array<unsigned> preserves;
  };
  export inline constexpr auto subpass(const subpass_description & d) {
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = d.colours.size();
    subpass.pColorAttachments = d.colours.begin();
    if (d.depth_stencil.layout) subpass.pDepthStencilAttachment = &d.depth_stencil;
    subpass.inputAttachmentCount = d.inputs.size();
    subpass.pInputAttachments = d.inputs.begin();
    if (d.resolves.size()) subpass.pResolveAttachments = d.resolves.begin();
    subpass.preserveAttachmentCount = d.preserves.size();
    subpass.pPreserveAttachments = d.preserves.begin();
    return subpass;
  }

  export constexpr const auto subpass_external = vk_subpass_external;
  export enum pipeline_stage_flags {
    pipeline_stage_bottom_of_pipe          = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    pipeline_stage_color_attachment_output = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    pipeline_stage_early_fragment_tests    = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    pipeline_stage_fragment_shader         = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    pipeline_stage_late_fragment_tests     = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
  };
  export enum access_flags {
    access_color_attachment_read          = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
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
  export inline constexpr auto dependency(const subpass_dependency & d) {
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
  export inline constexpr auto colour_dependency() {
    return dependency({
      .src_subpass = vk_subpass_external,
      .src_stage_mask = pipeline_stage_color_attachment_output,
      .dst_stage_mask = pipeline_stage_color_attachment_output,
      .dst_access_mask = access_color_attachment_write,
    });
  }
  export inline constexpr auto depth_dependency() {
    return dependency({
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

  export [[deprecated]] inline auto create_render_pass(VkFormat fmt, VkImageLayout final_layout) {
    return create_render_pass({
      .attachments {{
        colour_attachment(fmt, final_layout),
        depth_attachment(),
      }},
      .subpasses {{
        subpass({
          .colours {{ attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }},
          .depth_stencil = attachment_ref(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),
        }),
      }},
      .dependencies {{
        colour_dependency(),
        depth_dependency(),
      }},
    });
  }
  export [[deprecated]] inline auto create_render_pass(VkPhysicalDevice pd, VkSurfaceKHR s) {
    return create_render_pass(find_best_surface_format(pd, s).format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  }
  export [[deprecated]] inline auto create_depthless_render_pass(VkPhysicalDevice pd, VkSurfaceKHR s) {
    return create_render_pass({
      .attachments {{ colour_attachment(pd, s) }},
      .subpasses {{
        subpass({
          .colours {{ attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }},
        }),
      }},
      .dependencies {{ colour_dependency() }},
    });
  }
} // namespace vee
