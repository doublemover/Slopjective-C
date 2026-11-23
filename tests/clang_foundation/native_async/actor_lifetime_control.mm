#include "objc3_runtime_services.h"
#include "objc3_task.hpp"

#include <atomic>
#include <cassert>
#include <cstdio>
#include <memory>
#include <new>

extern "C" id class_createInstance(Class, __SIZE_TYPE__)
    __attribute__((ns_returns_retained));
extern "C" id object_dispose(__unsafe_unretained id);
extern "C" void objc_release(id);
bool native_actor_object_association();

static std::atomic<long> fail_after{-1};
extern "C" void *__real__Znwm(std::size_t);
extern "C" void *__wrap__Znwm(std::size_t size) {
  if (fail_after.load() >= 0 && fail_after.fetch_sub(1) == 0)
    throw std::bad_alloc();
  return __real__Znwm(size);
}

static unsigned destroyed;
__attribute__((objc_root_class))
@interface BindingAllocationControl { Class isa; @public bool retired; }
+ (id)new;
@end
@implementation BindingAllocationControl
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc {
  if (retired) {
    objc3_actor_t *found = nullptr;
    assert(objc3_actor_get_for_object((__bridge const void *)self, &found) ==
           OBJC3_SERVICE_CLOSED && !found);
  }
  ++destroyed;
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

static void binding_allocation_rollback() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  bool failed = false, succeeded = false;
  for (long position = 0; position != 8; ++position) {
    objc3_actor_t *actor = nullptr;
    assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
    BindingAllocationControl *owner = [BindingAllocationControl new];
    void *raw = (__bridge void *)owner;
    const auto before = destroyed;
    fail_after = position;
    const auto status = objc3_actor_bind_object(raw, actor);
    fail_after = -1;
    if (status == OBJC3_SERVICE_OK) succeeded = true;
    else {
      failed = true;
      assert(status == OBJC3_SERVICE_OUT_OF_MEMORY && destroyed == before);
      objc3_actor_t *found = nullptr;
      assert(objc3_actor_get_for_object(raw, &found) == OBJC3_SERVICE_NOT_READY && !found);
      assert(objc3_actor_bind_object(raw, actor) == OBJC3_SERVICE_OK);
    }
    assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
    owner->retired = true;
    assert(objc3_actor_unbind_object(raw) == OBJC3_SERVICE_OK);
    assert(destroyed == before);
#if !__has_feature(objc_arc)
    objc_release(owner);
#endif
    owner = (BindingAllocationControl *)0;
    assert(destroyed == before + 1);
    objc3_actor_release(actor);
  }
  assert(failed && succeeded);
  objc3_executor_release(executor);
}

using ResultBlock = int (^)(void);

static objc3::task<ResultBlock> return_block(std::shared_ptr<int> state) {
  co_return ^{ return *state; };
}

static objc3::task<BindingAllocationControl *> return_object(BindingAllocationControl *value) {
  co_return value;
}

static void task_result_bridges() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  auto pump = [&] {
    uint32_t count = 0;
    assert(objc3_executor_pump(executor, 1, &count) == OBJC3_SERVICE_OK && count == 1);
  };
  std::weak_ptr<int> weak;
  const auto before = destroyed;
  @autoreleasepool {
    auto state = std::make_shared<int>(42);
    weak = state;
    auto block_task = return_block(std::move(state)).start(executor);
    pump();
    ResultBlock block = block_task.result();
    block_task = {};
    assert(!weak.expired() && block() == 42);

    BindingAllocationControl *object = [BindingAllocationControl new];
    auto object_task = return_object(object).start(executor);
    pump();
    auto *extracted = object_task.result();
    assert(extracted == object);
    object_task = {};
#if !__has_feature(objc_arc)
    objc_release(object);
#endif
    object = (BindingAllocationControl *)0;
    assert(destroyed == before && !extracted->retired);
  }
  assert(weak.expired() && destroyed == before + 1);

  auto successful_void = []() -> objc3::task<void> { co_return; };
  auto done = successful_void().start(executor);
  pump();
  done.result();
  const int identity = 73;
  auto failing_void = [](const int *identity) -> objc3::task<void> {
    throw identity;
    co_return;
  };
  auto failed = failing_void(&identity).start(executor);
  pump();
  bool caught = false;
  try { failed.result(); }
  catch (const int *error) { caught = error == &identity; }
  assert(caught);
  objc3_executor_release(executor);
}

int main() {
  @autoreleasepool {
    assert(native_actor_object_association());
    binding_allocation_rollback();
    task_result_bridges();
  }
  std::puts("actor lifetime: all focused checks passed");
}
