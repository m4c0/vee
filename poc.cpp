import casein;
import vee;

void on_window_created(auto ptr) {
  static auto i = vee::create_instance("my-app");
  static auto dbg = vee::create_debug_utils_messenger();
  // pointer should come from UI (CAMetalLayer, HWND or AWindow)
  static auto s = vee::create_surface(ptr);
  static const auto &[pd, qf] =
      vee::find_physical_device_with_universal_queue(*s);
  static auto d = vee::create_single_queue_device(pd, qf);

  static auto q = vee::get_queue_for_family(qf);

  static auto sfmt = vee::find_best_surface_format(pd, *s);
  static auto rp = vee::create_render_pass(&sfmt);
}

extern "C" void casein_handle(const casein::event &e) {
  switch (e.type()) {
  case casein::CREATE_WINDOW:
    // You can fetch the native handle (HWND, NSWindow, etc) like this:
    on_window_created(
        e.as<casein::events::create_window>().native_window_handle());
    break;
  case casein::QUIT:
    // SDL_Quit, release shenanigans, etc
    break;
  default:
    break;
  }
}
