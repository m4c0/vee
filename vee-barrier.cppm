export module vee:barrier;
import :calls;
import wagen;

namespace vee {
  export enum barrier_type {
    from_compute_to_compute,
    from_fragment_to_fragment,
    from_host_to_transfer,
    from_transfer_to_fragment,
    from_transfer_to_vertex,
    from_pipeline_to_host,
  };
  export inline auto cmd_pipeline_barrier(VkCommandBuffer cb, VkBuffer buf,
                                          barrier_type bt) {
    VkBufferMemoryBarrier imb{};
    imb.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    imb.srcQueueFamilyIndex = vk_queue_family_ignored;
    imb.dstQueueFamilyIndex = vk_queue_family_ignored;
    imb.buffer = buf;
    imb.size = vk_whole_size;
  
    switch (bt) {
    case from_compute_to_compute: {
      imb.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
      imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  
      constexpr const auto stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
      calls::call(vkCmdPipelineBarrier, cb, stage, stage, 0, 0, nullptr, 1, &imb, 0, nullptr);
      break;
    }
    case from_fragment_to_fragment: silog::die("TBD: fragment-to-fragment barrier for images");
    case from_host_to_transfer: {
      imb.srcAccessMask = 0;
      imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  
      constexpr const auto src_stage = VK_PIPELINE_STAGE_HOST_BIT;
      constexpr const auto dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr,
                  1, &imb, 0, nullptr);
      break;
    }
    case from_transfer_to_fragment: {
      imb.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  
      constexpr const auto src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      constexpr const auto dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr,
                  1, &imb, 0, nullptr);
      break;
    }
    case from_transfer_to_vertex: {
      imb.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  
      constexpr const auto src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      constexpr const auto dst_stage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
      calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr,
                  1, &imb, 0, nullptr);
      break;
    }
    case from_pipeline_to_host: {
      imb.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
      imb.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
  
      constexpr const auto src_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      constexpr const auto dst_stage = VK_PIPELINE_STAGE_HOST_BIT;
      calls::call(vkCmdPipelineBarrier, cb, src_stage, dst_stage, 0, 0, nullptr,
                  1, &imb, 0, nullptr);
      break;
    }
    }
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
  
