export module vee:calls;
import :vulkan;

namespace vee {
template <typename T> struct last_arg;
template <typename Ret, typename Arg, typename Last>
struct last_arg<Ret (*&)(Arg, const VkAllocationCallbacks *, Last *)> {
  using type = Last;
};

template <typename Fn, typename Arg, typename Ret = typename last_arg<Fn>::type>
inline auto create(Fn &&fn, const Arg &arg) -> Ret {
  Ret out{};
  if (fn(arg, nullptr, &out) != VK_SUCCESS) {
  }
  return out;
}
} // namespace vee
