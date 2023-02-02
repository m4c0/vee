import vee;

int main() {
  auto i = vee::create_instance("my-app");
  auto d = vee::create_debug_utils_messenger();
  // pointer should come from UI (CAMetalLayer, HWND or AWindow)
  auto s = vee::create_surface(nullptr);
  const auto &[pd, qf] = vee::find_physical_device_with_universal_queue(*s);
}
