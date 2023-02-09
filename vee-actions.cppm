module;
#include "vulkan.hpp"

export module vee:actions;
import :calls;

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
};
export inline auto cmd_begin_render_pass(const render_pass_begin &rpb) {
  // Using sensible defaults. The magenta color is a visible marker for pixels
  // missing rendering
  VkClearValue values[2];
  values[0].color = {1.0f, 0.0f, 1.0f, 1.0f};
  values[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  info.renderPass = rpb.render_pass;
  info.framebuffer = rpb.framebuffer;
  info.renderArea.extent = rpb.extent;
  info.clearValueCount = 2;
  info.pClearValues = values;
  calls::call(vkCmdBeginRenderPass, rpb.command_buffer, &info,
              VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
}

export inline auto cmd_bind_gr_pipeline(VkCommandBuffer cb, VkPipeline p) {
  calls::call(vkCmdBindPipeline, cb, VK_PIPELINE_BIND_POINT_GRAPHICS, p);
}

export inline auto cmd_bind_vertex_buffers(VkCommandBuffer cb, unsigned idx,
                                           VkBuffer buf) {
  VkDeviceSize offs{};
  calls::call(vkCmdBindVertexBuffers, cb, idx, 1, &buf, &offs);
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

export inline auto device_wait_idle() { calls::call(vkDeviceWaitIdle); }

export inline auto end_cmd_buf(VkCommandBuffer cb) {
  calls::call(vkEndCommandBuffer, cb);
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

export inline auto wait_and_reset_fence(VkFence fence) {
  calls::call(vkWaitForFences, 1, &fence, VK_TRUE, ~0UL);
  calls::call(vkResetFences, 1, &fence);
}
} // namespace vee
