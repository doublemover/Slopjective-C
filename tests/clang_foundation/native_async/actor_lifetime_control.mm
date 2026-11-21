#include "objc3_runtime_services.h"

#include <atomic>
#include <cassert>
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

int main() {
  @autoreleasepool {
    assert(native_actor_object_association());
    binding_allocation_rollback();
  }
}
