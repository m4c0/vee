module;
#include "vulkan.hpp"

export module vee:actions;
import :calls;
import :mapmem;

namespace vee {
export inline auto bind_buffer_memory(VkBuffer buf, VkDeviceMemory mem,
                                      VkDeviceSize sz = 0) {
  calls::call(vkBindBufferMemory, buf, mem, sz);
  return nullptr; // Returns "something" to allow binding via RAII
}
export inline auto bind_image_memory(VkImage img, VkDeviceMemory mem,
                                     VkDeviceSize sz = 0) {
  calls::call(vkBindImageMemory, img, mem, sz);
  return nullptr; // Returns "something" to allow binding via RAII
}

export inline auto acquire_next_image(VkSwapchainKHR swc, VkSemaphore sema) {
  return calls::create<unsigned, &::vkAcquireNextImageKHR>(swc, ~0UL, sema,
                                                           VK_NULL_HANDLE);
}

export inline auto allocate_descriptor_set(VkDescriptorPool pool,
                                           VkDescriptorSetLayout layout) {
  VkDescriptorSetAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  info.descriptorPool = pool;
  info.descriptorSetCount = 1;
  info.pSetLayouts = &layout;
  return calls::create<VkDescriptorSet, &::vkAllocateDescriptorSets>(&info);
}

export inline auto allocate_primary_command_buffer(VkCommandPool pool) {
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandBufferCount = 1;
  info.commandPool = pool;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  return calls::create<VkCommandBuffer, &::vkAllocateCommandBuffers>(&info);
}
export inline auto allocate_secondary_command_buffer(VkCommandPool pool) {
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandBufferCount = 1;
  info.commandPool = pool;
  info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  return calls::create<VkCommandBuffer, &::vkAllocateCommandBuffers>(&info);
}

export inline auto begin_cmd_buf_render_pass_continue(VkCommandBuffer cb,
                                                      VkRenderPass rp) {
  VkCommandBufferInheritanceInfo inheritance{};
  inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
  inheritance.renderPass = rp;

  VkCommandBufferBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
  info.pInheritanceInfo = &inheritance;
  calls::call(vkBeginCommandBuffer, cb, &info);
}
export inline auto begin_cmd_buf_one_time_submit(VkCommandBuffer cb) {
  VkCommandBufferBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  calls::call(vkBeginCommandBuffer, cb, &info);
}

export struct render_pass_begin {
  VkCommandBuffer command_buffer;
  VkRenderPass render_pass;
  VkFramebuffer framebuffer;
  VkExtent2D extent;
  VkClearColorValue clear_color{1.0f, 0.0f, 1.0f, 1.0f};
  bool use_secondary_cmd_buf = true;
};
export inline auto cmd_begin_render_pass(const render_pass_begin &rpb) {
  // Using sensible defaults. The magenta color is a visible marker for pixels
  // missing rendering
  VkClearValue values[2];
  values[0].color = rpb.clear_color;
  values[1].depthStencil = {1.0f, 0};

  VkSubpassContents sbc = rpb.use_secondary_cmd_buf
                              ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
                              : VK_SUBPASS_CONTENTS_INLINE;

  VkRenderPassBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  info.renderPass = rpb.render_pass;
  info.framebuffer = rpb.framebuffer;
  info.renderArea.extent = rpb.extent;
  info.clearValueCount = 2;
  info.pClearValues = values;
  calls::call(vkCmdBeginRenderPass, rpb.command_buffer, &info, sbc);
}

export inline auto cmd_bind_descriptor_set(VkCommandBuffer cb,
                                           VkPipelineLayout pl, unsigned idx,
                                           VkDescriptorSet ds) {
  calls::call(vkCmdBindDescriptorSets, cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pl,
              idx, 1, &ds, 0, nullptr);
}

export inline auto cmd_bind_gr_pipeline(VkCommandBuffer cb, VkPipeline p) {
  calls::call(vkCmdBindPipeline, cb, VK_PIPELINE_BIND_POINT_GRAPHICS, p);
}

export inline auto cmd_bind_vertex_buffers(VkCommandBuffer cb, unsigned idx,
                                           VkBuffer buf) {
  VkDeviceSize offs{};
  calls::call(vkCmdBindVertexBuffers, cb, idx, 1, &buf, &offs);
}

export inline auto cmd_copy_buffer_to_image(VkCommandBuffer cb, VkExtent2D ext,
                                            VkBuffer buf, VkImage img) {
  VkBufferImageCopy r{};
  r.bufferOffset = 0;
  r.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  r.imageSubresource.layerCount = 1;
  r.imageExtent.width = ext.width;
  r.imageExtent.height = ext.height;
  r.imageExtent.depth = 1;
  calls::call(vkCmdCopyBufferToImage, cb, buf, img,
              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &r);
}

export inline auto cmd_draw(VkCommandBuffer cb, unsigned vtx,
                            unsigned inst = 1) {
  calls::call(vkCmdDraw, cb, vtx, inst, 0U, 0U);
}

export inline auto cmd_end_render_pass(VkCommandBuffer cb) {
  calls::call(vkCmdEndRenderPass, cb);
}

export inline auto cmd_execute_command(VkCommandBuffer pri_cb,
                                       VkCommandBuffer sec_cb) {
  calls::call(vkCmdExecuteCommands, pri_cb, 1, &sec_cb);
}

export enum barrier_type {
  from_host_to_transfer,
  from_transfer_to_fragment,
};
export inline auto cmd_pipeline_barrier(VkCommandBuffer cb, VkImage img,
                                        barrier_type bt) {
  VkImageMemoryBarrier imb{};
  imb.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imb.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imb.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imb.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imb.subresourceRange.levelCount = 1;
  imb.subresourceRange.layerCount = 1;
  imb.image = img;

  switch (bt) {
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
  }
}

export template <typename Tp>
inline auto cmd_push_fragment_constants(VkCommandBuffer cb, VkPipelineLayout pl,
                                        Tp *v, unsigned offset = 0) {
  calls::call(vkCmdPushConstants, cb, pl, VK_SHADER_STAGE_FRAGMENT_BIT, offset,
              sizeof(Tp), v);
}
export template <typename Tp>
inline auto cmd_push_vertex_constants(VkCommandBuffer cb, VkPipelineLayout pl,
                                      Tp *v, unsigned offset = 0) {
  calls::call(vkCmdPushConstants, cb, pl, VK_SHADER_STAGE_VERTEX_BIT, offset,
              sizeof(Tp), v);
}
export template <typename Tp>
inline auto cmd_push_vert_frag_constants(VkCommandBuffer cb,
                                         VkPipelineLayout pl, Tp *v,
                                         unsigned offset = 0) {
  calls::call(vkCmdPushConstants, cb, pl,
              VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, offset,
              sizeof(Tp), v);
}

export inline auto cmd_set_scissor(VkCommandBuffer cb, VkExtent2D ext) {
  VkRect2D rect{};
  rect.extent = ext;
  calls::call(vkCmdSetScissor, cb, 0, 1, &rect);
}

export inline auto cmd_set_viewport(VkCommandBuffer cb, VkExtent2D ext) {
  VkViewport viewport{};
  viewport.width = ext.width;
  viewport.height = ext.height;
  viewport.maxDepth = 1;
  calls::call(vkCmdSetViewport, cb, 0, 1, &viewport);
}

export inline auto device_wait_idle() { calls::call(vkDeviceWaitIdle); }

export inline auto end_cmd_buf(VkCommandBuffer cb) {
  calls::call(vkEndCommandBuffer, cb);
}

export template <typename Tp>
inline void map_memory(VkDeviceMemory m, auto &&fn) {
  mapmem mm{m};
  fn(static_cast<Tp *>(*mm));
}

export struct present_info {
  VkQueue queue;
  VkSwapchainKHR swapchain;
  VkSemaphore wait_semaphore;
  unsigned image_index;
};
export inline auto queue_present(const present_info &qp) {
  VkPresentInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &qp.wait_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &qp.swapchain;
  info.pImageIndices = &qp.image_index;
  calls::call(vkQueuePresentKHR, qp.queue, &info);
}

export struct submit_info {
  VkQueue queue;
  VkFence fence;
  VkCommandBuffer command_buffer;
  VkSemaphore wait_semaphore;
  VkSemaphore signal_semaphore;
};
export inline auto queue_submit(const submit_info &si) {
  VkPipelineStageFlags stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

  VkSubmitInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  info.commandBufferCount = 1;
  info.pCommandBuffers = &si.command_buffer;
  info.pWaitDstStageMask = &stage;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &si.wait_semaphore;
  info.signalSemaphoreCount = 1;
  info.pSignalSemaphores = &si.signal_semaphore;
  calls::call(vkQueueSubmit, si.queue, 1, &info, si.fence);
}

export inline auto update_descriptor_set_with_storage(VkDescriptorSet set,
                                                      unsigned binding,
                                                      VkBuffer b) {
  VkDescriptorBufferInfo ii{};
  ii.buffer = b;
  ii.offset = 0;
  ii.range = VK_WHOLE_SIZE;

  VkWriteDescriptorSet w{};
  w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  w.dstSet = set;
  w.dstBinding = binding;
  w.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  w.descriptorCount = 1;
  w.pBufferInfo = &ii;
  calls::call(vkUpdateDescriptorSets, 1, &w, 0, nullptr);
}

export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding,
                                         VkImageView iv, VkSampler s) {
  VkDescriptorImageInfo ii{};
  ii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  ii.imageView = iv;
  ii.sampler = s;

  VkWriteDescriptorSet w{};
  w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  w.dstSet = set;
  w.dstBinding = binding;
  w.descriptorCount = 1;
  w.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  w.pImageInfo = &ii;
  calls::call(vkUpdateDescriptorSets, 1, &w, 0, nullptr);
}

export inline auto wait_and_reset_fence(VkFence fence) {
  calls::call(vkWaitForFences, 1, &fence, VK_TRUE, ~0UL);
  calls::call(vkResetFences, 1, &fence);
}
} // namespace vee
