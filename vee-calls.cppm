export module vee:calls;
import hai;
import silog;

namespace vee::calls {
template <typename Fn, typename... Args>
constexpr void call(Fn &&fn, Args &&...args) {
  if (fn(args...) != 0) {
    silog::log(silog::error, "Vulkan API failure");
    // TODO: throw?
  }
}

template <auto *Fn, typename Ret> consteval auto create() {
  return [](auto &in) {
    Ret out{};
    call(*Fn, &in, nullptr, &out);
    return out;
  };
}
template <auto *Fn> consteval auto destroy() {
  return [](auto ptr) { call(*Fn, ptr, nullptr); };
}
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
