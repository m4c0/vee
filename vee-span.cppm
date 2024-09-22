export module vee:span;
import hai;

namespace vee {
export template <typename Tp> class span {
  hai::array<Tp> m_data{};

public:
  constexpr span() = default;

  constexpr span(auto... args) : m_data{hai::array<Tp>::make(args...)} {}

  [[nodiscard]] constexpr auto *data() const { return m_data.begin(); }
  [[nodiscard]] constexpr auto &operator[](unsigned idx) {
    return m_data[idx];
  }
  [[nodiscard]] constexpr auto size() const { return m_data.size(); };
};
} // namespace vee
