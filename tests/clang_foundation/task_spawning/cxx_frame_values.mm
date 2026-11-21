#include <objc3/concurrency.h>
#include <cassert>
#include <initializer_list>
#include <memory>
#include <cstdio>
#include "cxx_frame_values.h"

int main() {
  objc3_executor_t *source = nullptr;
  assert(objc3_executor_create(1, &source) == OBJC3_SERVICE_OK);
  try {
    frame_cxx_value_contract(source);
  } catch (const objc3::service_failure &failure) {
    std::fprintf(stderr, "frame value case %d: service status %d\n",
                 frame_value_case, int(failure.status()));
    throw;
  }
  objc3_executor_release(source);
}
