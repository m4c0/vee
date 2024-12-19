#pragma leco app
#pragma leco add_shader "poc.comp"

import vee;

int main() try {
  auto i = vee::create_instance("vee-poc-compute");
  auto dbg = vee::create_debug_utils_messenger();
  const auto & [pd, qf] = vee::find_physical_device_with_universal_queue(nullptr);
  auto d = vee::create_single_queue_device(pd, qf);
  auto q = vee::get_queue_for_family(qf);

  constexpr const auto buf_sz = 1024 * 1024 * 4;
  vee::device_memory mem = vee::create_host_buffer_memory(pd, buf_sz * 3);

  auto dsl = vee::create_descriptor_set_layout({
    vee::dsl_compute_storage(),
    vee::dsl_compute_storage(),
    vee::dsl_compute_storage(),
  });
  auto pl = vee::create_pipeline_layout({ *dsl });

  auto dpool = vee::create_descriptor_pool(1, { vee::storage_buffer(3) });

  auto ds = vee::allocate_descriptor_set(*dpool, *dsl);

  auto buf0 = vee::create_buffer(buf_sz, vee::buffer_usage::storage_buffer);
  vee::bind_buffer_memory(*buf0, *mem, 0);
  vee::update_descriptor_set_with_storage(ds, 0, *buf0);

  auto buf1 = vee::create_buffer(buf_sz, vee::buffer_usage::storage_buffer);
  vee::bind_buffer_memory(*buf0, *mem, buf_sz);
  vee::update_descriptor_set_with_storage(ds, 1, *buf1);

  auto buf2 = vee::create_buffer(buf_sz, vee::buffer_usage::storage_buffer);
  vee::bind_buffer_memory(*buf0, *mem, buf_sz * 2);
  vee::update_descriptor_set_with_storage(ds, 2, *buf2);

  auto kern = vee::create_shader_module_from_resource("poc.comp.spv");
  auto p = vee::create_compute_pipeline(*pl, *kern, "main");

  auto cp = vee::create_command_pool(qf);
  auto cb = vee::allocate_primary_command_buffer(*cp);
  auto f = vee::create_fence_signaled();

  {
    auto p = static_cast<float *>(vee::map_memory(*mem));
    for (auto i = 0; i < buf_sz / 4; i++) {
      p[i] = 1;
    }
    vee::unmap_memory(*mem);
  }

  {
    vee::begin_cmd_buf_one_time_submit(cb);
    vee::cmd_bind_c_pipeline(cb, *p);
    vee::cmd_bind_descriptor_set(cb, *pl, 0, ds);
    vee::cmd_dispatch(cb, 1024, 1024, 1);
    vee::end_cmd_buf(cb);
  }
  vee::queue_submit({
    .queue = q,
    .fence = *f,
    .command_buffer = cb
  });
  vee::device_wait_idle();

  {
    auto p = static_cast<float *>(vee::map_memory(*mem));
    for (auto i = 0; i < buf_sz; i++) {
    }
    vee::unmap_memory(*mem);
  }
} catch (...) {
  return 1;
}
