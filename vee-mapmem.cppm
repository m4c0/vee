export module vee:mapmem;
import :actions;
import wagen;

namespace vee {
export class mapmem {
  VkDeviceMemory m_dm{};
  void *m_ptr{};

public:
  mapmem() = default;
  explicit mapmem(VkDeviceMemory m) : m_dm{m}, m_ptr{map_memory(m)} {}
  ~mapmem() {
    if (m_ptr != nullptr)
      unmap_memory(m_dm);
  }

  mapmem(const mapmem &) = delete;
  mapmem &operator=(const mapmem &) = delete;

  mapmem(mapmem &&o) : m_dm{o.m_dm}, m_ptr{o.m_ptr} { o.m_ptr = nullptr; }
  mapmem &operator=(mapmem &&o) = delete;

  [[nodiscard]] constexpr auto *operator*() noexcept { return m_ptr; }
};
} // namespace vee
