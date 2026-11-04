#include <cassert>
#include <cstdio>

int never_called();
int recovery(int divisor) {
  if (!divisor)
    return 42;
  return 84 / divisor;
}

int main(int argc, char **) {
  // Two real control-flow paths have the same semantic result. Coverage must
  // show the previously unexecuted recovery arm after the second process.
  const int value = recovery(argc == 1 ? 2 : 0);
  assert(value == 42);
  std::printf("{\"value\":%d}\n", value);
#ifdef WRONG_BINARY
  std::puts("deliberately different image");
#endif
}
