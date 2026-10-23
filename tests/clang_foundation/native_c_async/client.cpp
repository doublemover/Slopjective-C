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
  native_c_wide wide{};
  wide.bytes[255] = 40;
  auto wide_frame = native_c_wide_value(wide);
  auto wide_task = objc3::task<native_c_wide>::from_native(&wide_frame).start(executor);
  wide_task.wait();
  assert(wide_task.result().bytes[255] == 42);
  native_c_complex complex{};
  __real__ complex = 40;
  __imag__ complex = 2;
  auto complex_frame = native_c_complex_value(complex);
  auto complex_task = objc3::task<native_c_complex>::from_native(&complex_frame).start(executor);
  complex_task.wait();
  auto complex_result = complex_task.result();
  assert(__real__ complex_result == 42 && __imag__ complex_result == 2);
  auto empty_frame = native_c_empty(nullptr);
  auto empty = objc3::task<void>::from_native(&empty_frame).start(executor);
  empty.wait();
  empty.result();
  auto roundtrip_frame = native_c_roundtrip(42);
  auto roundtrip = objc3::task<int>::from_native(&roundtrip_frame).start(executor);
  roundtrip.wait();
  assert(roundtrip.result() == 42);
  auto foreign_frame = native_c_roundtrip(-1);
  auto foreign = objc3::task<int>::from_native(&foreign_frame).start(executor);
  foreign.wait();
  bool foreign_caught = false;
  try { (void)foreign.result(); }
  catch (int value) { foreign_caught = value == 42; }
  assert(foreign_caught);
  for (int mode = 0; mode != 5; ++mode) {
    int destroyed = 0;
    auto method_frame = native_c_make_method_frame(mode, &destroyed);
    auto method = objc3::task<int>::from_native(&method_frame).start(executor);
    method.wait();
    assert(method.result() == (mode == 1 || mode == 3 ? 43 : 42));
    // Join frame destruction before the source-owned counter leaves scope.
    auto group = objc3::task_group_handle::create();
    group.add(method);
    group.close();
    group.wait();
    assert(destroyed == (mode == 4 ? 0 : 1));
  }
  {
    int destroyed = 0;
    auto block_frame = native_c_make_block_frame(39, &destroyed);
    auto block = objc3::task<int>::from_native(&block_frame).start(executor);
    block.wait();
    assert(block.result() == 42);
    auto group = objc3::task_group_handle::create();
    group.add(block);
    group.close();
    group.wait();
    assert(destroyed == 1);
  }
  {
    auto nil_frame = native_c_make_nil_frame();
    assert(!nil_frame.context);
    objc3_task_t *nil_task = nullptr;
    assert(objc3_async_frame_start(&nil_frame, executor, nullptr, &nil_task) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
    auto nil_await_frame = native_c_make_nil_await_frame();
    auto nil_await = objc3::task<int>::from_native(&nil_await_frame).start(executor);
    nil_await.wait();
    bool failed = false;
    try { (void)nil_await.result(); }
    catch (const objc3::service_failure &error) {
      failed = error.status() == OBJC3_SERVICE_INVALID_ARGUMENT;
    }
    assert(failed);
  }
  objc3_executor_release(executor);
}
