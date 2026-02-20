export module vee:barrier;
import :calls;
import silog;
import wagen;

namespace vee {
  export inline auto memory_barrier(VkAccessFlags src, VkAccessFlags dst) {
    return VkMemoryBarrier {
      .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
      .srcAccessMask = src,
      .dstAccessMask = dst,
    };
  }
  export inline auto cmd_pipeline_barrier(VkCommandBuffer cb, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkMemoryBarrier bmb) {
    calls::call(vkCmdPipelineBarrier, cb, src, dst, 0, 1, &bmb, 0, nullptr, 0, nullptr);
  }

  export inline auto buffer_memory_barrier(VkBuffer buf, VkAccessFlags src, VkAccessFlags dst) {
    return VkBufferMemoryBarrier {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcAccessMask = src,
      .dstAccessMask = dst,
      .srcQueueFamilyIndex = vk_queue_family_ignored,
      .dstQueueFamilyIndex = vk_queue_family_ignored,
      .buffer = buf,
      .size = vk_whole_size,
    };
  }
  export inline auto cmd_pipeline_barrier(VkCommandBuffer cb, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkBufferMemoryBarrier bmb) {
    calls::call(vkCmdPipelineBarrier, cb, src, dst, 0, 0, nullptr, 1, &bmb, 0, nullptr);
  }

  export enum barrier_type {
    from_compute_to_compute,
    from_fragment_to_fragment,
    from_host_to_transfer,
    from_transfer_to_fragment,
    from_transfer_to_vertex,
    from_pipeline_to_host,
  };
  export inline auto cmd_pipeline_barrier(VkCommandBuffer cb, VkBuffer buf, barrier_type bt) {
    switch (bt) {
      case from_compute_to_compute: {
        constexpr const auto stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        cmd_pipeline_barrier(cb, stage, stage, buffer_memory_barrier(buf, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT));
        break;
      }
      case from_fragment_to_fragment:
        silog::die("TBD: fragment-to-fragment barrier for images");
        break;
      case from_host_to_transfer:
        cmd_pipeline_barrier(cb,
            VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            buffer_memory_barrier(buf, 0, VK_ACCESS_TRANSFER_WRITE_BIT));
        break;
      case from_transfer_to_fragment:
        cmd_pipeline_barrier(cb,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            buffer_memory_barrier(buf, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT));
        break;
      case from_transfer_to_vertex:
        cmd_pipeline_barrier(cb,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            buffer_memory_barrier(buf, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT));
        break;
      case from_pipeline_to_host:
        cmd_pipeline_barrier(cb,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_HOST_BIT,
            buffer_memory_barrier(buf, VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_HOST_READ_BIT));
        break;
    }
  }

  export inline auto cmd_pipeline_barrier(VkCommandBuffer cb, VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage, VkImageMemoryBarrier imb) {
    imb.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imb.srcQueueFamilyIndex = vk_queue_family_ignored;
    imb.dstQueueFamilyIndex = vk_queue_family_ignored;
    imb.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imb.subresourceRange.levelCount = 1;
    imb.subresourceRange.layerCount = 1;

    calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &imb);
  }
  export inline auto cmd_pipeline_barrier(VkCommandBuffer cb, VkImage img,
                                          barrier_type bt) {
    VkImageMemoryBarrier imb{};
    imb.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imb.srcQueueFamilyIndex = vk_queue_family_ignored;
    imb.dstQueueFamilyIndex = vk_queue_family_ignored;
    imb.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imb.subresourceRange.levelCount = 1;
    imb.subresourceRange.layerCount = 1;
    imb.image = img;
  
    switch (bt) {
      case from_compute_to_compute: silog::die("TBD: compute-to-compute barrier for images");
      case from_fragment_to_fragment: {
        imb.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        imb.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
  
        constexpr const auto stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        calls::call(vkCmdPipelineBarrier, cb, stage, stage, 0, 0, nullptr, 0, nullptr, 1, &imb);
        break;
      }
    case from_host_to_transfer: {
      imb.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imb.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imb.srcAccessMask = 0;
      imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  
      constexpr const auto src_stage = VK_PIPELINE_STAGE_HOST_BIT;
      constexpr const auto dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr,
                  0, nullptr, 1, &imb);
      break;
    }
    case from_transfer_to_fragment: {
      imb.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imb.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imb.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  
      constexpr const auto src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      constexpr const auto dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr,
                  0, nullptr, 1, &imb);
      break;
    }
    case from_transfer_to_vertex: {
      imb.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imb.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imb.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  
      constexpr const auto src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      constexpr const auto dst_stage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
      calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr,
                  0, nullptr, 1, &imb);
      break;
    }
    case from_pipeline_to_host: {
      imb.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imb.newLayout = VK_IMAGE_LAYOUT_GENERAL;
      imb.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
      imb.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
  
      constexpr const auto src_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      constexpr const auto dst_stage = VK_PIPELINE_STAGE_HOST_BIT;
      calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr,
                  0, nullptr, 1, &imb);
      break;
    }
    }
  }
}
  
