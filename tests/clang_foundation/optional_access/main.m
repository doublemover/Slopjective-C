#ifndef OPTIONAL_ACCESS_RUNTIME_PCH
#include "runtime.h"
#endif
int main(void) {
  @autoreleasepool { optional_access_suite(); }
  return access_failures != 0 || access_created != access_destroyed;
}
