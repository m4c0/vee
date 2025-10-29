export module vee:update_descriptor_set;
import :calls;

namespace vee {
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
  export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, unsigned first, VkImageView iv, VkSampler s) {
    auto ii = descriptor_image_info(iv, s);
    auto w = write_descriptor_set({
      .dstSet = set,
      .dstBinding = binding,
      .dstArrayElement = first,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .pImageInfo = &ii,
    });
    calls::call(vkUpdateDescriptorSets, 1, &w, 0, nullptr);
  }
  export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, VkImageView iv, VkSampler s) {
    return update_descriptor_set(set, binding, 0, iv, s);
  }
  export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, unsigned first, VkImageView iv) {
    return update_descriptor_set(set, binding, first, iv, nullptr);
  }
  export inline auto update_descriptor_set(VkDescriptorSet set, unsigned binding, VkImageView iv) {
    return update_descriptor_set(set, binding, 0, iv, nullptr);
  }
  
  export inline auto update_descriptor_set_for_uniform(VkDescriptorSet set, unsigned binding, VkBuffer buf) {
    auto bi = vee::descriptor_buffer_info(buf);
    vee::update_descriptor_set(vee::write_descriptor_set({
      .dstSet = set,
      .dstBinding = binding,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = &bi,
    }));
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
}
