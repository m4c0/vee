#pragma leco app
#pragma leco add_shader "poc-bitonic.comp"

import rng;
import silog;
import sires;
import vee;

int main() try {
  auto i = vee::create_instance("vee-poc-bitonic");
  auto dbg = vee::create_debug_utils_messenger();
  const auto & [pd, qf] = vee::find_physical_device_with_universal_queue(nullptr);
  auto d = vee::create_single_queue_device(pd, qf);
  auto q = vee::get_queue_for_family(qf);

  constexpr const auto elems = 16;
  constexpr const auto buf_sz = elems * sizeof(unsigned);

  auto dsl = vee::create_descriptor_set_layout({
    vee::dsl_compute_storage(),
    vee::dsl_compute_storage(),
  });
  auto pl = vee::create_pipeline_layout(*dsl);

  auto dpool = vee::create_descriptor_pool(2, { vee::storage_buffer(4) });

  auto ds0 = vee::allocate_descriptor_set(*dpool, *dsl);
  auto ds1 = vee::allocate_descriptor_set(*dpool, *dsl);

  auto mem0 = vee::create_host_memory(pd, buf_sz);
  auto buf0 = vee::create_buffer(buf_sz, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  vee::bind_buffer_memory(*buf0, *mem0, 0);

  auto mem1 = vee::create_host_memory(pd, buf_sz);
  auto buf1 = vee::create_buffer(buf_sz, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  vee::bind_buffer_memory(*buf1, *mem1, 0);

  // We "ping-pong" between bitonic stages
  vee::update_descriptor_set(ds0, 0, *buf0);
  vee::update_descriptor_set(ds0, 1, *buf1);
  vee::update_descriptor_set(ds1, 0, *buf1);
  vee::update_descriptor_set(ds1, 1, *buf0);

  auto kern = vee::create_shader_module(sires::slurp("poc-bitonic.comp.spv"));
  auto p = vee::create_compute_pipeline(*pl, *kern, "main");

  auto cp = vee::create_command_pool(qf);
  auto cb = vee::allocate_primary_command_buffer(*cp);
  auto f = vee::create_fence_reset();

  {
    auto p = static_cast<unsigned *>(vee::map_memory(*mem0));
    for (auto i = 0; i < elems; i++) p[i] = i;
    for (auto i = 0; i < elems; i++) {
      for (auto j = i + 1; j < elems; j++) {
        if (rng::randf() >= 0.5) continue;
        auto tmp = p[i];
        p[i] = p[j];
        p[j] = tmp;
      }
    }
    vee::unmap_memory(*mem0);
  }

  {
    vee::begin_cmd_buf_one_time_submit(cb);
    vee::cmd_bind_c_pipeline(cb, *p);
    vee::cmd_bind_c_descriptor_set(cb, *pl, 0, ds0);
    vee::cmd_dispatch(cb, elems / 2, 1, 1);
    vee::end_cmd_buf(cb);
  }
  vee::queue_submit({
    .queue = q,
    .fence = *f,
    .command_buffer = cb
  });
  vee::device_wait_idle();

  {
    auto p = static_cast<unsigned *>(vee::map_memory(*mem1));
    for (auto i = 0; i < elems; i++) {
      silog::log(silog::info, ">>>>> %d", p[i]);
    }
    vee::unmap_memory(*mem1);
  }
} catch (...) {
  return 1;
}
