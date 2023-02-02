module;
#include "vulkan.hpp"

export module vee:calls;
import hai;
import silog;
import traits;

namespace vee::calls {
template <typename Fn, typename... Args>
  requires requires(Fn fn, Args... args) {
             { fn(args...) } -> traits::same_as<void>;
           }
constexpr void call(Fn &&fn, Args &&...args) {
  fn(args...);
}
template <typename Fn, typename... Args>
  requires requires(Fn fn, Args... args) {
             { fn(args...) } -> traits::not_same_as<void>;
           }
constexpr void call(Fn &&fn, Args &&...args) {
  if (fn(args...) != 0) {
    silog::log(silog::error, "Vulkan API failure");
    // TODO: throw?
  }
}
template <typename Fn, typename... Args>
  requires requires(Fn fn, VkInstance i, Args... args) { fn(i, args...); }
constexpr void call(Fn &&fn, Args &&...args) {
  call(fn, volkGetLoadedInstance(), args...);
}
template <typename Fn, typename... Args>
  requires requires(Fn fn, VkDevice d, Args... args) { fn(d, args...); }
constexpr void call(Fn &&fn, Args &&...args) {
  call(fn, volkGetLoadedDevice(), args...);
}

template <typename Tp, auto *Fn, typename... Args>
constexpr const auto create(Args... in) {
  Tp h{};
  call(*Fn, in..., &h);
  return h;
}

template <auto *DFn> struct h_destroyer {
  constexpr void operator()(auto h) { call(*DFn, h, nullptr); }
};
template <typename Tp, auto *CFn, auto *DFn> class handle {
  hai::value_holder<Tp, h_destroyer<DFn>> m_h{};

public:
  constexpr handle() = default;
  constexpr explicit handle(const auto... in)
      : m_h{create<Tp, CFn>(in..., nullptr)} {}

  [[nodiscard]] constexpr auto operator*() const noexcept { return *m_h; }
};

template <auto *Fn, typename Ret> consteval auto enumerate() {
  class vec {
    hai::holder<Ret[]> m_data;
    unsigned m_size;

  public:
    constexpr explicit vec(unsigned count)
        : m_data{hai::holder<Ret[]>::make(count)}, m_size{count} {}

    [[nodiscard]] constexpr auto data() noexcept { return *m_data; }
    [[nodiscard]] constexpr auto data() const noexcept { return *m_data; }
    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }

    [[nodiscard]] constexpr auto begin() noexcept { return *m_data; }
    [[nodiscard]] constexpr auto begin() const noexcept { return *m_data; }
    [[nodiscard]] constexpr auto end() noexcept { return *m_data + m_size; }
    [[nodiscard]] constexpr auto end() const noexcept {
      return *m_data + m_size;
    }
  };
  return [](auto... args) {
    unsigned count{};
    call(*Fn, args..., &count, nullptr);

    vec res{count};
    call(*Fn, args..., &count, res.data());
    return res;
  };
}
} // namespace vee::calls
