import vee;

int main() {
  auto i = vee::create_instance("my-app");
  auto dbg = vee::create_debug_utils_messenger();
  // pointer should come from UI (CAMetalLayer, HWND or AWindow)
  auto s = vee::create_surface(nullptr);
  const auto &[pd, qf] = vee::find_physical_device_with_universal_queue(*s);
  auto d = vee::create_single_queue_device(pd, qf);

  auto q = vee::get_queue_for_family(qf);

  auto sfmt = vee::find_best_surface_format(pd, *s);
  auto rp = vee::create_render_pass(&sfmt);
}
