#include "../silog/build.hpp"
#include "ecow.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  auto m = unit::create<mod>("vee");
  m->add_wsdep("silog", silog());
  m->add_include_dir("vulkan-headers/include");
  m->add_part("vulkan");
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
