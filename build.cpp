#include "build.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  auto m = vee();

  auto poc = unit::create<app>("vee-poc");
  poc->add_wsdep("casein", casein());
  poc->add_wsdep("hai", hai());
  poc->add_wsdep("sires", sires());
  poc->add_wsdep("traits", traits());
  poc->add_ref(m);
  poc->add_unit<>("poc")->add_include_dir("vulkan-headers/include");
  poc->add_unit<spirv>("poc.frag");
  poc->add_unit<spirv>("poc.vert");
  poc->add_resource("poc.frag.spv");
  poc->add_resource("poc.vert.spv");

  auto pf = unit::create<per_feat<seq>>("vee-filter");
  pf->for_feature(android_ndk).add_ref(poc);
  pf->for_feature(objective_c).add_ref(poc);
  pf->for_feature(windows_api).add_ref(poc);

  return run_main(pf, argc, argv);
}
