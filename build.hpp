#pragma once

#include "../casein/build.hpp"
#include "../hai/build.hpp"
#include "../jute/build.hpp"
#include "../silog/build.hpp"
#include "../sires/build.hpp"
#include "../traits/build.hpp"
#include "ecow.hpp"

class spirv : public ecow::unit {
  void build_self() const override {
    const auto spv =
        ecow::impl::current_target()->build_path() / (name() + ".spv");

    if (!ecow::impl::must_recompile(name(), spv))
      return;

    std::cerr << "compiling " << name() << std::endl;
    auto cmd =
        std::string{"glslangValidator -V -o "} + spv.string() + " " + name();
    if (std::system(cmd.c_str()) != 0)
      throw std::runtime_error("Failed to compile shader");
  }
  [[nodiscard]] pathset self_objects() const override { return {}; }

public:
  explicit spirv(const std::string &name) : unit{name} {}
};

auto vee() {
  using namespace ecow;

  constexpr const auto setup = [](mod &m) -> mod & {
    m.add_wsdep("hai", hai());
    m.add_wsdep("jute", jute());
    m.add_wsdep("silog", silog());
    m.add_wsdep("sires", sires());
    m.add_wsdep("traits", traits());
    m.add_include_dir("vulkan-headers/include");

    m.add_unit<>("volk.cpp")->add_include_dir("vulkan-headers/include");

    // Used by all
    m.add_part("calls");

    // Used by others
    m.add_part("surface_capabilities");
    m.add_part("surface_format");

    // Everybody else
    m.add_part("actions");
    m.add_part("alias");
    m.add_part("buffer");
    m.add_part("command_pool");
    m.add_part("debug_utils_messenger");
    m.add_part("descriptor_pool");
    m.add_part("descriptor_set_layout");
    m.add_part("device");
    m.add_part("device_memory");
    m.add_part("fence");
    m.add_part("framebuffer");
    m.add_part("gr_pipeline");
    m.add_part("image");
    m.add_part("image_view");
    m.add_part("instance");
    m.add_part("physical_device");
    m.add_part("pipeline_layout");
    m.add_part("render_pass");
    m.add_part("sampler");
    m.add_part("semaphore");
    m.add_part("shader_module");
    m.add_part("swapchain");

    return m;
  };

  auto m = unit::create<per_feat<mod>>("vee");
  setup(m->for_feature(android_ndk)).add_part("android");
  setup(m->for_feature(objective_c)).add_part("metal");
  setup(m->for_feature(windows_api)).add_part("windows");
  return m;
}
