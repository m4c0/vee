export module vee:mapmem;
import :calls;
import wagen;

// clang <=16 has a bug on Windows when we instantiate a exported symbol which
// relies on non-exported symbols. i.e. it blows up when it tries to instantiate
// `calls::create` inside any form of template method/class
export namespace vee::memimpl {
[[nodiscard]] void *map(VkDeviceMemory m) {
  return calls::create<void *, &::vkMapMemory>(m, 0, vk_whole_size, 0);
}
void unmap(VkDeviceMemory m) { calls::call(vkUnmapMemory, m); }
} // namespace vee::memimpl

namespace vee {
export class mapmem {
  VkDeviceMemory m_dm{};
  void *m_ptr{};

public:
  mapmem() = default;
  explicit mapmem(VkDeviceMemory m) : m_dm{m}, m_ptr{memimpl::map(m)} {}
  ~mapmem() {
    if (m_ptr != nullptr)
      memimpl::unmap(m_dm);
  }

  mapmem(const mapmem &) = delete;
  mapmem &operator=(const mapmem &) = delete;

  mapmem(mapmem &&o) : m_dm{o.m_dm}, m_ptr{o.m_ptr} { o.m_ptr = nullptr; }
  mapmem &operator=(mapmem &&o) = delete;

  [[nodiscard]] constexpr auto *operator*() noexcept { return m_ptr; }
};
} // namespace vee
