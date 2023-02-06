module;
#include "vulkan.hpp"

export module vee:calls;
import hai;
import silog;
import traits;

namespace vee {
struct api_failure {};
struct out_of_date_error {};

inline constexpr const auto message_for_result(VkResult res) {
  switch (res) {
  case VK_SUCCESS:
    return "Vulkan API failure: VK_SUCCESS";
  case VK_NOT_READY:
    return "Vulkan API failure: VK_NOT_READY";
  case VK_TIMEOUT:
    return "Vulkan API failure: VK_TIMEOUT";
  case VK_EVENT_SET:
    return "Vulkan API failure: VK_EVENT_SET";
  case VK_EVENT_RESET:
    return "Vulkan API failure: VK_EVENT_RESET";
  case VK_INCOMPLETE:
    return "Vulkan API failure: VK_INCOMPLETE";
  case VK_ERROR_OUT_OF_HOST_MEMORY:
    return "Vulkan API failure: VK_ERROR_OUT_OF_HOST_MEMORY";
  case VK_ERROR_OUT_OF_DEVICE_MEMORY:
    return "Vulkan API failure: VK_ERROR_OUT_OF_DEVICE_MEMORY";
  case VK_ERROR_INITIALIZATION_FAILED:
    return "Vulkan API failure: VK_ERROR_INITIALIZATION_FAILED";
  case VK_ERROR_DEVICE_LOST:
    return "Vulkan API failure: VK_ERROR_DEVICE_LOST";
  case VK_ERROR_MEMORY_MAP_FAILED:
    return "Vulkan API failure: VK_ERROR_MEMORY_MAP_FAILED";
  case VK_ERROR_LAYER_NOT_PRESENT:
    return "Vulkan API failure: VK_ERROR_LAYER_NOT_PRESENT";
  case VK_ERROR_EXTENSION_NOT_PRESENT:
    return "Vulkan API failure: VK_ERROR_EXTENSION_NOT_PRESENT";
  case VK_ERROR_FEATURE_NOT_PRESENT:
    return "Vulkan API failure: VK_ERROR_FEATURE_NOT_PRESENT";
  case VK_ERROR_INCOMPATIBLE_DRIVER:
    return "Vulkan API failure: VK_ERROR_INCOMPATIBLE_DRIVER";
  case VK_ERROR_TOO_MANY_OBJECTS:
    return "Vulkan API failure: VK_ERROR_TOO_MANY_OBJECTS";
  case VK_ERROR_FORMAT_NOT_SUPPORTED:
    return "Vulkan API failure: VK_ERROR_FORMAT_NOT_SUPPORTED";
  case VK_ERROR_FRAGMENTED_POOL:
    return "Vulkan API failure: VK_ERROR_FRAGMENTED_POOL";
  case VK_ERROR_UNKNOWN:
    return "Vulkan API failure: VK_ERROR_UNKNOWN";
  case VK_ERROR_OUT_OF_POOL_MEMORY:
    return "Vulkan API failure: VK_ERROR_OUT_OF_POOL_MEMORY";
  case VK_ERROR_INVALID_EXTERNAL_HANDLE:
    return "Vulkan API failure: VK_ERROR_INVALID_EXTERNAL_HANDLE";
  case VK_ERROR_FRAGMENTATION:
    return "Vulkan API failure: VK_ERROR_FRAGMENTATION";
  case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
    return "Vulkan API failure: VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
  case VK_PIPELINE_COMPILE_REQUIRED:
    return "Vulkan API failure: VK_PIPELINE_COMPILE_REQUIRED";
  case VK_ERROR_SURFACE_LOST_KHR:
    return "Vulkan API failure: VK_ERROR_SURFACE_LOST_KHR";
  case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
    return "Vulkan API failure: VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
  case VK_SUBOPTIMAL_KHR:
    return "Vulkan API failure: VK_SUBOPTIMAL_KHR";
  case VK_ERROR_OUT_OF_DATE_KHR:
    return "Vulkan API failure: VK_ERROR_OUT_OF_DATE_KHR";
  case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
    return "Vulkan API failure: VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
  case VK_ERROR_VALIDATION_FAILED_EXT:
    return "Vulkan API failure: VK_ERROR_VALIDATION_FAILED_EXT";
  case VK_ERROR_INVALID_SHADER_NV:
    return "Vulkan API failure: VK_ERROR_INVALID_SHADER_NV";
  case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
    return "Vulkan API failure: "
           "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
  case VK_ERROR_NOT_PERMITTED_KHR:
    return "Vulkan API failure: VK_ERROR_NOT_PERMITTED_KHR";
  case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
    return "Vulkan API failure: VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
  case VK_THREAD_IDLE_KHR:
    return "Vulkan API failure: VK_THREAD_IDLE_KHR";
  case VK_THREAD_DONE_KHR:
    return "Vulkan API failure: VK_THREAD_DONE_KHR";
  case VK_OPERATION_DEFERRED_KHR:
    return "Vulkan API failure: VK_OPERATION_DEFERRED_KHR";
  case VK_OPERATION_NOT_DEFERRED_KHR:
    return "Vulkan API failure: VK_OPERATION_NOT_DEFERRED_KHR";
  case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
    return "Vulkan API failure: VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
  }
}
} // namespace vee

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
  switch (auto res = fn(args...)) {
  case VK_SUCCESS:
  case VK_SUBOPTIMAL_KHR:
    break;
  case VK_ERROR_OUT_OF_DATE_KHR:
    throw out_of_date_error{};
  default:
    silog::log(silog::error, message_for_result(res));
    throw api_failure{};
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
