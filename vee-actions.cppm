export module vee:actions;
import :calls;
import dotz;
import hai;
import silog;
import wagen;

using namespace wagen;

namespace vee {
export inline auto bind_buffer_memory(VkBuffer buf, VkDeviceMemory mem,
                                      VkDeviceSize offs = 0) {
  calls::call(vkBindBufferMemory, buf, mem, offs);
  return nullptr; // Returns "something" to allow binding via RAII
}
export inline auto bind_image_memory(VkImage img, VkDeviceMemory mem,
                                     VkDeviceSize offs = 0) {
  calls::call(vkBindImageMemory, img, mem, offs);
  return nullptr; // Returns "something" to allow binding via RAII
}

export inline auto acquire_next_image(VkSwapchainKHR swc, VkSemaphore sema) {
  return calls::create<unsigned, &::vkAcquireNextImageKHR>(swc, ~0UL, sema,
                                                           vk_null_handle);
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
export inline auto begin_cmd_buf_sim_use(VkCommandBuffer cb) {
  VkCommandBufferBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  calls::call(vkBeginCommandBuffer, cb, &info);
}

export inline auto clear_colour(float r, float g, float b, float a) {
  return VkClearColorValue {{ r, g, b, a }};
}
export inline auto clear_colour(dotz::vec4 c) {
  return VkClearColorValue {{ c.x, c.y, c.z, c.w }};
}
export struct render_pass_begin {
  VkCommandBuffer command_buffer;
  VkRenderPass render_pass;
  VkFramebuffer framebuffer;
  VkExtent2D extent;
  // Using sensible defaults. The magenta color is a visible marker for pixels
  // missing rendering
  hai::view<VkClearColorValue> clear_colours { clear_colour(1, 0, 1, 1) };
  bool use_secondary_cmd_buf = false;
  bool clear_depth = true;
};
static hai::array<VkClearValue> clear_values(const render_pass_begin & rpb) {
  auto dsz = rpb.clear_depth ? 1 : 0;
  auto csz = rpb.clear_colours.size() + dsz;
  if (csz == 0) return {};

  hai::array<VkClearValue> values { csz };
  for (auto i = 0; i < rpb.clear_colours.size(); i++) {
    values[i].color = rpb.clear_colours[i];
  }
  if (rpb.clear_depth) values[values.size() - 1].depthStencil = {1.0f, 0};
  return values;
}
export inline auto cmd_begin_render_pass(const render_pass_begin &rpb) {
  auto values = clear_values(rpb);

  VkSubpassContents sbc = rpb.use_secondary_cmd_buf
                              ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
                              : VK_SUBPASS_CONTENTS_INLINE;

  VkRenderPassBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  info.renderPass = rpb.render_pass;
  info.framebuffer = rpb.framebuffer;
  info.renderArea.extent = rpb.extent;
  info.clearValueCount = values.size();
  info.pClearValues = values.begin();
  calls::call(vkCmdBeginRenderPass, rpb.command_buffer, &info, sbc);
}

export inline void cmd_next_subpass(VkCommandBuffer cb, bool use_secondaries = false) {
  VkSubpassContents sbc = use_secondaries
                              ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
                              : VK_SUBPASS_CONTENTS_INLINE;
  calls::call(vkCmdNextSubpass, cb, sbc);
}

export inline auto cmd_bind_descriptor_set(VkCommandBuffer cb,
                                           VkPipelineLayout pl, unsigned idx,
                                           VkDescriptorSet ds) {
  calls::call(vkCmdBindDescriptorSets, cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pl,
              idx, 1, &ds, 0, nullptr);
}
export inline auto cmd_bind_c_descriptor_set(
    VkCommandBuffer cb, VkPipelineLayout pl, unsigned idx, VkDescriptorSet ds) {
  calls::call(vkCmdBindDescriptorSets, cb, VK_PIPELINE_BIND_POINT_COMPUTE, pl, idx, 1, &ds, 0, nullptr);
}

export inline auto cmd_bind_c_pipeline(VkCommandBuffer cb, VkPipeline p) {
  calls::call(vkCmdBindPipeline, cb, VK_PIPELINE_BIND_POINT_COMPUTE, p);
}
export inline auto cmd_bind_gr_pipeline(VkCommandBuffer cb, VkPipeline p) {
  calls::call(vkCmdBindPipeline, cb, VK_PIPELINE_BIND_POINT_GRAPHICS, p);
}

export inline auto cmd_bind_index_buffer_u16(VkCommandBuffer cb, VkBuffer buf, VkDeviceSize ofs = 0) {
  calls::call(vkCmdBindIndexBuffer, cb, buf, ofs, VK_INDEX_TYPE_UINT16);
}
export inline auto cmd_bind_index_buffer_u32(VkCommandBuffer cb, VkBuffer buf, VkDeviceSize ofs = 0) {
  calls::call(vkCmdBindIndexBuffer, cb, buf, ofs, VK_INDEX_TYPE_UINT32);
}
export inline auto cmd_bind_vertex_buffers(VkCommandBuffer cb, unsigned idx,
                                           VkBuffer buf, VkDeviceSize offs) {
  calls::call(vkCmdBindVertexBuffers, cb, idx, 1, &buf, &offs);
}
export inline auto cmd_bind_vertex_buffers(VkCommandBuffer cb, unsigned idx,
                                           VkBuffer buf) {
  cmd_bind_vertex_buffers(cb, idx, buf, {});
}

export inline auto cmd_copy_buffer(VkCommandBuffer cb, VkBuffer src,
                                   VkBuffer dst, unsigned sz) {
  VkBufferCopy r{};
  r.size = sz;
  calls::call(vkCmdCopyBuffer, cb, src, dst, 1, &r);
}

constexpr auto vk_buffer_image_copy(VkImageAspectFlags aspect, VkOffset2D ofs, VkExtent2D ext) {
  VkBufferImageCopy r{};
  r.bufferOffset = 0;
  r.imageSubresource.aspectMask = aspect;
  r.imageSubresource.layerCount = 1;
  r.imageOffset.x = ofs.x;
  r.imageOffset.y = ofs.y;
  r.imageExtent.width = ext.width;
  r.imageExtent.height = ext.height;
  r.imageExtent.depth = 1;
  return r;
}
export inline auto cmd_copy_buffer_to_image(VkCommandBuffer cb, VkExtent2D ext, VkBuffer buf, VkImage img) {
  auto r = vk_buffer_image_copy(VK_IMAGE_ASPECT_COLOR_BIT, {}, ext);
  calls::call(vkCmdCopyBufferToImage, cb, buf, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &r);
}
export inline auto cmd_copy_image_to_buffer(VkCommandBuffer cb, VkOffset2D ofs, VkExtent2D ext, VkImage img, VkBuffer buf) {
  auto r = vk_buffer_image_copy(VK_IMAGE_ASPECT_COLOR_BIT, ofs, ext);
  calls::call(vkCmdCopyImageToBuffer, cb, img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buf, 1, &r);
}
export inline auto cmd_copy_image_to_buffer(VkCommandBuffer cb, VkExtent2D ext, VkImage img, VkBuffer buf) {
  auto r = vk_buffer_image_copy(VK_IMAGE_ASPECT_COLOR_BIT, {}, ext);
  calls::call(vkCmdCopyImageToBuffer, cb, img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buf, 1, &r);
}
export inline auto cmd_copy_yuv420p_buffers_to_image(VkCommandBuffer cb,
                                                     VkExtent2D ext, VkBuffer y,
                                                     VkBuffer u, VkBuffer v,
                                                     VkImage img) {
  auto yy = vk_buffer_image_copy(VK_IMAGE_ASPECT_PLANE_0_BIT, {}, ext);
  calls::call(vkCmdCopyBufferToImage, cb, y, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &yy);

  VkExtent2D half_ext{ext.width / 2, ext.height / 2};

  auto uu = vk_buffer_image_copy(VK_IMAGE_ASPECT_PLANE_1_BIT, {}, half_ext);
  calls::call(vkCmdCopyBufferToImage, cb, u, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &uu);

  auto vv = vk_buffer_image_copy(VK_IMAGE_ASPECT_PLANE_2_BIT, {}, half_ext);
  calls::call(vkCmdCopyBufferToImage, cb, v, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vv);
}

export inline auto cmd_dispatch(VkCommandBuffer cb, unsigned count_x, unsigned count_y, unsigned count_z) {
  calls::call(vkCmdDispatch, cb, count_x, count_y, count_z);
}

export struct draw_params {
  unsigned vcount;
  unsigned icount = 1;
  unsigned first_v = 0;
  unsigned first_i = 0;
};
export inline auto cmd_draw(VkCommandBuffer cb, const draw_params & p) {
  calls::call(vkCmdDraw, cb, p.vcount, p.icount, p.first_v, p.first_i);
}
export inline auto cmd_draw(VkCommandBuffer cb, unsigned vtx, unsigned inst = 1,
                            unsigned first_inst = 0) {
  calls::call(vkCmdDraw, cb, vtx, inst, 0U, first_inst);
}

export struct draw_indexed_params {
  unsigned xcount;
  unsigned icount = 1;
  unsigned first_x = 0;
  int voffs = 0;
  unsigned first_i = 0;
};
export inline auto cmd_draw_indexed(VkCommandBuffer cb, const draw_indexed_params & p) {
  calls::call(vkCmdDrawIndexed, cb, p.xcount, p.icount, p.first_x, p.voffs, p.first_i);
}
export inline auto cmd_draw_indexed(VkCommandBuffer cb, unsigned xcount, unsigned icount = 1) {
  cmd_draw_indexed(cb, { .xcount = xcount, .icount = icount });
}

export inline auto cmd_end_render_pass(VkCommandBuffer cb) {
  calls::call(vkCmdEndRenderPass, cb);
}

export inline auto cmd_execute_command(VkCommandBuffer pri_cb,
                                       VkCommandBuffer sec_cb) {
  calls::call(vkCmdExecuteCommands, pri_cb, 1, &sec_cb);
}

export inline auto cmd_fill_buffer(VkCommandBuffer cb, VkBuffer buf, unsigned data) {
  // Remember: offset/size must be multiples of two
  calls::call(vkCmdFillBuffer, cb, buf, 0, vk_whole_size, data);
}

export template <typename Tp>
inline auto cmd_push_compute_constants(VkCommandBuffer cb, VkPipelineLayout pl, Tp *v, unsigned offset = 0) {
  calls::call(vkCmdPushConstants, cb, pl, VK_SHADER_STAGE_COMPUTE_BIT, offset, sizeof(Tp), v);
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

export inline auto cmd_set_scissor(VkCommandBuffer cb, VkRect2D r) {
  calls::call(vkCmdSetScissor, cb, 0, 1, &r);
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

export inline void *map_memory(VkDeviceMemory m) {
  return calls::create<void *, &::vkMapMemory>(m, 0, vk_whole_size, 0);
}
export inline void unmap_memory(VkDeviceMemory m) {
  return calls::call(vkUnmapMemory, m);
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
  if (si.wait_semaphore != nullptr) {
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &si.wait_semaphore;
  }
  if (si.signal_semaphore != nullptr) {
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &si.signal_semaphore;
  }
  calls::call(vkQueueSubmit, si.queue, 1, &info, si.fence);
}

export constexpr auto descriptor_buffer_info(VkBuffer b) {
  return VkDescriptorBufferInfo {
    .buffer = b,
    .offset = 0,
    .range = vk_whole_size,
  };
}
export constexpr auto descriptor_image_info(VkImageView i) {
  return VkDescriptorImageInfo {
    .imageView = i,
    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
  };
}
export constexpr auto descriptor_image_info(VkImageView i, VkSampler s) {
  return VkDescriptorImageInfo {
    .sampler = s,
    .imageView = i,
    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
  };
}
export constexpr auto write_descriptor_set(VkWriteDescriptorSet w) {
  w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  return w;
}
export inline auto update_descriptor_set(const VkWriteDescriptorSet & w) {
  calls::call(vkUpdateDescriptorSets, 1, &w, 0, nullptr);
}
export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, VkBuffer b) {
  auto ii = descriptor_buffer_info(b);
  auto w = write_descriptor_set({
    .dstSet = set,
    .dstBinding = binding,
    .descriptorCount = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    .pBufferInfo = &ii,
  });
  calls::call(vkUpdateDescriptorSets, 1, &w, 0, nullptr);
}
export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, const hai::array<VkImageView> & ivs, VkSampler s) {
  hai::array<VkDescriptorImageInfo> iis { ivs.size() };
  for (auto i = 0; i < ivs.size(); i++) {
    iis[i] = descriptor_image_info(ivs[i], s);
  }
  auto w = write_descriptor_set({
    .dstSet = set,
    .dstBinding = binding,
    .descriptorCount = iis.size(),
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .pImageInfo = iis.begin(),
  });
  calls::call(vkUpdateDescriptorSets, 1, &w, 0, nullptr);
}
export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, unsigned first, const hai::array<VkImageView> & ivs) {
  hai::array<VkDescriptorImageInfo> iis { ivs.size() };
  for (auto i = 0; i < ivs.size(); i++) {
    iis[i] = descriptor_image_info(ivs[i]);
  }
  auto w = write_descriptor_set({
    .dstSet = set,
    .dstBinding = binding,
    .dstArrayElement = first,
    .descriptorCount = iis.size(),
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .pImageInfo = iis.begin(),
  });
  calls::call(vkUpdateDescriptorSets, 1, &w, 0, nullptr);
}
export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, const hai::array<VkImageView> & ivs) {
  return update_descriptor_set(set, binding, 0, ivs);
}
export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, VkImageView iv, VkSampler s) {
  auto ii = descriptor_image_info(iv, s);
  auto w = write_descriptor_set({
    .dstSet = set,
    .dstBinding = binding,
    .descriptorCount = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .pImageInfo = &ii,
  });
  calls::call(vkUpdateDescriptorSets, 1, &w, 0, nullptr);
}
export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, VkImageView iv) {
  return update_descriptor_set(set, binding, iv, nullptr);
}

export inline auto update_descriptor_set_for_attachment(VkDescriptorSet set, unsigned binding, VkImageView iv) {
  auto ii = descriptor_image_info(iv, nullptr);
  vee::update_descriptor_set(vee::write_descriptor_set({
    .dstSet = set,
    .dstBinding = binding,
    .descriptorCount = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    .pImageInfo = &ii,
  }));
}
} // namespace vee
