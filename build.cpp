#include "ecow.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  auto m = unit::create<mod>("vee");

  return run_main(m, argc, argv);
}
