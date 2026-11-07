#ifndef GUARD_RUNTIME_PCH
#include "runtime.h"
#endif
int main(void) {
  guard_plain_suite();
  return guard_failures != 0;
}
