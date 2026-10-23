#include "provider.h"
#include "objc3_task.hpp"
#include <cassert>
#include <utility>

// Independently compiled C++20/23 consumers import the carrier without knowing
// the C provider's frame layout or a C++ promise type.
int main() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(0, &executor) == OBJC3_SERVICE_OK);
  int cleanups = 0;
  objc3_async_frame frame = native_c_chain(7, &cleanups);
  auto pending = objc3::task<int>::from_native(&frame);
  assert(!frame.context);
  auto running = std::move(pending).start(executor);
  running.wait();
  assert(running.result() == 21);
  objc3_async_frame pair_frame = native_c_pair_value({40, 2.0});
  auto pair = objc3::task<native_c_pair>::from_native(&pair_frame).start(executor);
  pair.wait();
  auto value = pair.result();
  assert(value.left == 42 && value.right == 2.0);
  objc3_executor_release(executor);
}
