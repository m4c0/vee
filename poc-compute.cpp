#pragma leco app
#pragma leco add_shader "poc.comp"

import silog;
import sires;
import vee;

int main() try {
  auto i = vee::create_instance("vee-poc-compute");
  auto dbg = vee::create_debug_utils_messenger();
  const auto & [pd, qf] = vee::find_physical_device_with_universal_queue(nullptr);
  auto d = vee::create_single_queue_device(pd, qf);
  auto q = vee::get_queue_for_family(qf);

  constexpr const auto buf_sz = 1024 * 1024 * sizeof(float);
  constexpr const auto mem_sz = buf_sz * 3;
  vee::device_memory mem = vee::create_host_memory(pd, mem_sz);

  auto dsl = vee::create_descriptor_set_layout({
    vee::dsl_compute_storage(),
    vee::dsl_compute_storage(),
    vee::dsl_compute_storage(),
  });
  auto pl = vee::create_pipeline_layout(*dsl);

  auto dpool = vee::create_descriptor_pool(1, { vee::storage_buffer(3) });

  auto ds = vee::allocate_descriptor_set(*dpool, *dsl);

  auto buf0 = vee::create_buffer(buf_sz, vee::buffer_usage::storage_buffer);
  vee::bind_buffer_memory(*buf0, *mem, 0);
  vee::update_descriptor_set(ds, 0, *buf0);

  auto buf1 = vee::create_buffer(buf_sz, vee::buffer_usage::storage_buffer);
  vee::bind_buffer_memory(*buf1, *mem, buf_sz);
  vee::update_descriptor_set(ds, 1, *buf1);

  auto buf2 = vee::create_buffer(buf_sz, vee::buffer_usage::storage_buffer);
  vee::bind_buffer_memory(*buf2, *mem, buf_sz * 2);
  vee::update_descriptor_set(ds, 2, *buf2);

  auto kern = vee::create_shader_module(sires::slurp("poc.comp.spv"));
  auto p = vee::create_compute_pipeline(*pl, *kern, "main");

  auto cp = vee::create_command_pool(qf);
  auto cb = vee::allocate_primary_command_buffer(*cp);
  auto f = vee::create_fence_reset();

  {
    auto p = static_cast<float *>(vee::map_memory(*mem));
    for (auto i = 0; i < mem_sz / 4; i++) {
      p[i] = 1;
    }
    vee::unmap_memory(*mem);
  }

  {
    vee::begin_cmd_buf_one_time_submit(cb);
    vee::cmd_bind_c_pipeline(cb, *p);
    vee::cmd_bind_c_descriptor_set(cb, *pl, 0, ds);
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
    int counts[4] {};
    auto p = static_cast<float *>(vee::map_memory(*mem));
    for (auto i = 0; i < mem_sz / 4; i++) {
      int c = p[i];
      if (c >= 3) c = 3;
      counts[c]++;
    }
    vee::unmap_memory(*mem);

    silog::log(silog::info, ">>>>> 0=%d 1=%d 2=%d others=%d", counts[0], counts[1], counts[2], counts[3]);
  }
} catch (...) {
  return 1;
}
