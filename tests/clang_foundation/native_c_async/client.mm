#include "provider.h"
#include "objc3_task.hpp"
#include <cassert>
#include <utility>

// This consumer sees logical result/effect signatures with a persistent C-frame
// entry identity. Its own async definitions retain the C++ task entry ABI.
using FrameCallback = int (*)(int, int *) __objc3_async_frame;
using TaskCallback = int (*)(int, int *) __objc3_async;
static_assert(!__is_same(FrameCallback, TaskCallback));
static_assert(__is_same(decltype(native_c_leaf(1, nullptr)), objc3_async_frame));
static_assert(__is_same(decltype([(NativeCAsyncWorker *)0 value:1]), objc3_async_frame));

int mixed(int count, int *cleanups) __objc3_async {
  FrameCallback callback = native_c_leaf;
  int total = __objc3_await native_c_chain(count, cleanups);
  return total + __objc3_await callback(21, cleanups);
}
int method_and_block(NativeCAsyncWorker *worker, NativeCAsyncBlock block) __objc3_async {
  int a = __objc3_await [worker value:2];
  int b = __objc3_await [worker directValue:2];
  int c = block ? __objc3_await block(2) : 0;
  return a + b + c;
}
int recover_frame(id<NativeCError> error) __objc3_async {
  __objc3_do {
    return __objc3_try(__objc3_await native_c_propagate(error));
  } __objc3_catch(id<NativeCError> caught) {
    return caught == error ? __objc3_await native_c_leaf(42, nullptr) : -1;
  }
}
int nil_frame() __objc3_async {
  return __objc3_await [(NativeCAsyncWorker *)0 value:1];
}
struct PoolGuard {
  int &live;
  int &finished;
  PoolGuard(int &live, int &finished) : live(live), finished(finished) { ++live; }
  PoolGuard(const PoolGuard &other) : PoolGuard(other.live, other.finished) {}
  ~PoolGuard() { if (--live == 0) ++finished; }
};
int pool_frames(int *destroyed, int *live, int *finished, bool unwind) __objc3_async {
  @autoreleasepool {
    guard let local = Some(PoolGuard, PoolGuard(*live, *finished)) else { return -1; }
    __objc3_defer { assert(local.live > 0 && local.finished == 0); }
    int answer = __objc3_await native_c_pools(destroyed);
    assert(local.live > 0 && local.finished == 0);
    if (unwind) throw answer;
    return answer;
  }
}
int allocation_error() __objc3_async { return __objc3_await native_c_oom_parent(); }
int foreign_error() __objc3_async { return __objc3_await native_c_roundtrip(-1); }
struct native_c_pair pair_frame() __objc3_async {
  return __objc3_await native_c_pair_value({40, 2.0});
}
void void_frame() __objc3_async { __objc3_await native_c_empty(nullptr); }
id object_frame(id value) __objc3_async { return __objc3_await native_c_object(value, nullptr); }

extern void release_source(void *) __asm__("objc_release");
int main() {
  @autoreleasepool {
    objc3_executor_t *executor = nullptr;
    assert(objc3_executor_create(0, &executor) == OBJC3_SERVICE_OK);
    int cleanups = 0;
    auto value = mixed(7, &cleanups).start(executor);
    value.wait();
    assert(value.result() == 42 && cleanups == 9);
    NativeCAsyncWorker *worker = [NativeCAsyncWorker new];
    worker->number = 40;
    auto methods = method_and_block(worker, nullptr).start(executor);
    methods.wait();
    assert(methods.result() == 85);
    // A descriptor mismatch must destroy the still-lazy native method frame.
    int rejected_destructions = 0;
    bool rejected = false;
    try {
      auto wrong = __objc3_async_from_frame<long>(
          native_c_make_method_frame(0, &rejected_destructions));
      (void)wrong;
    } catch (const objc3::service_failure &error) {
      rejected = error.status() == OBJC3_SERVICE_TYPE_MISMATCH;
    }
    assert(rejected && rejected_destructions == 1);
    auto checked = recover_frame((id<NativeCError>)worker).start(executor);
    checked.wait();
    assert(checked.result() == 42);
    auto object = object_frame(worker).start(executor);
    object.wait();
    assert(object.result() == worker);
    auto pair = pair_frame().start(executor);
    pair.wait();
    assert(pair.result().left == 42);
    auto empty = void_frame().start(executor);
    empty.wait();
    empty.result();
    auto nil_task = nil_frame().start(executor);
    nil_task.wait();
    // An unchecked message returning a zeroable scalar keeps Objective-C nil
    // semantics even when it is awaited through a native task.
    assert(nil_task.result() == 0);
    for (bool unwind : {false, true}) {
      int pool_destructions = 0, guard_live = 0, guard_finished = 0;
      auto pools = pool_frames(&pool_destructions, &guard_live, &guard_finished,
                              unwind).start(executor);
      pools.wait();
      bool caught = false;
      try { assert(pools.result() == 42 && !unwind); }
      catch (int value) { caught = value == 42; }
      assert(caught == unwind && pool_destructions == 6);
      assert(guard_live == 0 && guard_finished == 1);
    }
    int pool_destructions = 0, guard_live = 0, guard_finished = 0;
    objc3_executor_t *manual = nullptr;
    assert(objc3_executor_create(1, &manual) == OBJC3_SERVICE_OK);
    auto cancelled = pool_frames(&pool_destructions, &guard_live, &guard_finished,
                                 false).start(manual);
    uint32_t pumped = 0;
    assert(objc3_executor_pump(manual, 1, &pumped) == OBJC3_SERVICE_OK && pumped == 1);
    assert(guard_live > 0 && guard_finished == 0);
    cancelled.cancel();
    assert(objc3_executor_pump(manual, 10000, &pumped) == OBJC3_SERVICE_OK);
    cancelled.wait();
    bool was_cancelled = false;
    try { (void)cancelled.result(); }
    catch (const objc3::task_cancelled &) { was_cancelled = true; }
    assert(was_cancelled && guard_live == 0 && guard_finished == 1);
    objc3_executor_release(manual);
    auto oom = allocation_error().start(executor);
    oom.wait();
    bool allocation_status = false;
    try { (void)oom.result(); }
    catch (const objc3::service_failure &error) {
      allocation_status = error.status() == OBJC3_SERVICE_OUT_OF_MEMORY;
    }
    assert(allocation_status);
    auto foreign = foreign_error().start(executor);
    foreign.wait();
    bool original = false;
    try { (void)foreign.result(); }
    catch (int error) { original = error == 42; }
    assert(original);
#if !__has_feature(objc_arc)
    release_source((__bridge void *)worker);
#endif
    objc3_executor_release(executor);
  }
}
