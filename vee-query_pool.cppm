export module vee:query_pool;
import :calls;

using namespace traits::ints;

namespace vee {
  export using query_pool = calls::handle<VkQueryPool, &::vkCreateQueryPool, &::vkDestroyQueryPool>;

  export auto create_timestamp_query_pool(unsigned pool_size) {
    VkQueryPoolCreateInfo info {
      .sType      = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
      .queryType  = VK_QUERY_TYPE_TIMESTAMP,
      .queryCount = pool_size,
    };
    return query_pool { &info };
  }

  export void get_query_pool_results(VkQueryPool qp, unsigned first, uint64_t * data, unsigned sz) {
    calls::call(vkGetQueryPoolResults, qp, first, sz, sz * sizeof(uint64_t), data, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);
  }

  export void reset_query_pool(VkQueryPool qp, unsigned first, unsigned count) {
    calls::call(vkResetQueryPool, qp, first, count);
  }

  export void cmd_copy_query_pool_results(VkCommandBuffer cb, VkQueryPool qp, unsigned first, unsigned count, VkBuffer buf, VkDeviceSize ofs = 0) {
    calls::call(vkCmdCopyQueryPoolResults, cb, qp, first, count, buf, ofs, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);
  }

  export void cmd_reset_query_pool(VkCommandBuffer cb, VkQueryPool qp, unsigned first, unsigned count) {
    calls::call(vkCmdResetQueryPool, cb, qp, first, count);
  };

  export void cmd_write_timestamp(VkCommandBuffer cb, VkPipelineStageFlagBits stages, VkQueryPool qp, unsigned query) {
    calls::call(vkCmdWriteTimestamp, cb, stages, qp, query);
  };
}
