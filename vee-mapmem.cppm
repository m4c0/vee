module;
#include "vulkan.hpp"

export module vee:mapmem;
import :calls;

// clang <=16 has a bug on Windows when we instantiate a exported symbol which
// relies on non-exported symbols. i.e. it blows up when it tries to instantiate
// `calls::create` inside any form of template method/class
export namespace vee::memimpl {
[[nodiscard]] void *map(VkDeviceMemory m) {
  return calls::create<void *, &::vkMapMemory>(m, 0, VK_WHOLE_SIZE, 0);
}
void unmap(VkDeviceMemory m) { calls::call(vkUnmapMemory, m); }
} // namespace vee::memimpl

namespace vee {
export template <typename Tp> class mapmem {
  VkDeviceMemory m_dm;
  Tp *m_ptr;

public:
  explicit mapmem(VkDeviceMemory m)
      : m_dm{m}, m_ptr{static_cast<Tp *>(memimpl::map(m))} {}
  ~mapmem() { memimpl::unmap(m_dm); }

  mapmem(const mapmem &) = delete;
  mapmem(mapmem &&) = delete;
  mapmem &operator=(const mapmem &) = delete;
  mapmem &operator=(mapmem &&) = delete;

  [[nodiscard]] constexpr Tp &operator[](unsigned idx) noexcept {
    return m_ptr[idx];
  }
  [[nodiscard]] constexpr Tp *begin() noexcept { return m_ptr; }
};
} // namespace vee
