export module vee:buffer;
import :calls;
import wagen;

using namespace wagen;

namespace vee {
export using buffer =
    calls::handle<VkBuffer, &::vkCreateBuffer, &::vkDestroyBuffer>;

export enum buffer_usage {
  storage_buffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
  transfer_dst_buffer = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
  transfer_src_buffer = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  vertex_buffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
};

export inline auto create_buffer(unsigned size, buffer_usage usage,
                                 auto... usages) {
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.usage = (usages | ... | usage);
  info.size = size;
  return buffer{&info};
}

export inline auto create_vertex_buffer(unsigned size) {
  return create_buffer(size, vertex_buffer);
}

export inline auto create_storage_buffer(unsigned size) {
  return create_buffer(size, storage_buffer);
}

export inline auto create_transfer_dst_buffer(unsigned size) {
  return create_buffer(size, transfer_dst_buffer);
}

export inline auto create_transfer_src_buffer(unsigned size) {
  return create_buffer(size, transfer_src_buffer);
}
} // namespace vee
