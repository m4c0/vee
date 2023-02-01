#include "../hai/build.hpp"
#include "../jute/build.hpp"
#include "../silog/build.hpp"
#include "../traits/build.hpp"
#include "ecow.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  auto m = unit::create<mod>("vee");
  m->add_wsdep("hai", hai());
  m->add_wsdep("jute", jute());
  m->add_wsdep("silog", silog());
  m->add_wsdep("traits", traits());
  m->add_include_dir("vulkan-headers/include");

  m->add_unit<>("volk.cpp")->add_include_dir("vulkan-headers/include");

  m->add_part("calls");

  m->add_part("debug_utils_messenger");
  m->add_part("instance");

  auto poc = unit::create<app>("vee-poc");
  poc->add_ref(m);
  poc->add_unit<>("poc");

  auto pf = unit::create<per_feat<seq>>("vee-filter");
  pf->for_feature(android_ndk).add_ref(poc);
  pf->for_feature(objective_c).add_ref(poc);
  pf->for_feature(windows_api).add_ref(poc);

  return run_main(pf, argc, argv);
}
