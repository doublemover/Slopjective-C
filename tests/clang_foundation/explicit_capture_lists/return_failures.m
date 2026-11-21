#include "return_failures.h"
int main(void) {
  capture_return_failure_suite();
  return capture_failures != 0;
}
