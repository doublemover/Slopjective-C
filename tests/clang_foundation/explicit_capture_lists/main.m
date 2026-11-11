#ifndef CAPTURE_RUNTIME_PCH
#include "runtime.h"
#endif
int main(void) {
  capture_suite();
  return capture_failures != 0;
}
