#include "ecow.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  auto m = unit::create<mod>("vee");
  m->add_include_dir("vulkan-headers/include");
  m->add_part("vulkan");

  return run_main(m, argc, argv);
}
