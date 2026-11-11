#include <objc3/concurrency.h>
#include <cassert>
#include <memory>
#include "cxx_frame_borrows.h"

int main() {
  objc3_executor_t *source = nullptr;
  assert(objc3_executor_create(1, &source) == OBJC3_SERVICE_OK);
  cxx_frame_borrow_contract(source);
  objc3_executor_release(source);
}
