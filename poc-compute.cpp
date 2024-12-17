#pragma leco app
#pragma leco add_shader "poc.comp"

import vee;

int main() try {
  auto i = vee::create_instance("vee-poc-compute");
  auto dbg = vee::create_debug_utils_messenger();
  const auto & [pd, qf] = vee::find_physical_device_with_universal_queue(nullptr);
  auto d = vee::create_single_queue_device(pd, qf);
  auto q = vee::get_queue_for_family(qf);

  auto pl = vee::create_pipeline_layout();

  auto kern = vee::create_shader_module_from_resource("poc.comp.spv");
  auto pipeline = vee::create_compute_pipeline(*pl, *kern, "main");

  auto cp = vee::create_command_pool(qf);
  auto cb = vee::allocate_primary_command_buffer(*cp);
  auto f = vee::create_fence_signaled();

  {
    vee::begin_cmd_buf_one_time_submit(cb);
    vee::cmd_dispatch(cb, 1, 1, 1);
    vee::end_cmd_buf(cb);
  }
  vee::queue_submit({
    .queue = q,
    .fence = *f,
    .command_buffer = cb
  });
} catch (...) {
  return 1;
}
