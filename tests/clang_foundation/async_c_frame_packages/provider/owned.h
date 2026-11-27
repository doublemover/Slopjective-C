#pragma once
#include <objc/runtime.h>

id class_createInstance(Class type, size_t extra) __attribute__((ns_returns_retained));
#if !__has_feature(objc_arc)
#include <objc/objc-arc.h>
#endif

__attribute__((objc_root_class))
@interface CFrameOwned {
  Class isa;
  int identity;
}
+ (id)new;
- (void)setIdentity:(int)value;
- (int)identity;
- (id)carry:(__attribute__((ns_consumed)) id)argument async;
@end

static int ownership_destroys;
static int ownership_enters;
static id owned_child(id value) async { return value; }

@implementation CFrameOwned
+ (id)new { return class_createInstance(self, 0); }
- (void)setIdentity:(int)value { identity = value; }
- (int)identity { return identity; }
- (void)dealloc {
  ++ownership_destroys;
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
- (id)carry:(__attribute__((ns_consumed)) id)argument async {
#if !__has_feature(objc_arc)
  defer { objc_autorelease(argument); }
#endif
  ++ownership_enters;
  id result = await owned_child(argument);
  assert([self identity] == 17 && [(CFrameOwned *)result identity] == 42);
  return result;
}
@end

// Each route releases the caller's ownership before the native frame runs.
static void exercise_owned_frame(int phase) {
  int destroyed = ownership_destroys, entered = ownership_enters;
  @autoreleasepool {
    CFrameOwned *receiver = [CFrameOwned new];
    CFrameOwned *argument = [CFrameOwned new];
    [receiver setIdentity:17];
    [argument setIdentity:42];
    const void *identity = (__bridge const void *)argument;
#if !__has_feature(objc_arc)
    objc_retain(argument);
#endif
    objc3_async_frame frame = [receiver carry:argument];
#if !__has_feature(objc_arc)
    objc_release(receiver);
    objc_release(argument);
#endif
    receiver = 0;
    argument = 0;
    assert(ownership_destroys == destroyed && ownership_enters == entered);
    objc3_executor_t *executor = 0;
    objc3_task_t *task = 0;
    assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
    assert(objc3_async_frame_start(&frame, executor, 0, &task) == OBJC3_SERVICE_OK);
    unsigned count = 0;
    if (phase != 0) {
      assert(objc3_executor_pump(executor, 1, &count) == OBJC3_SERVICE_OK && count == 1);
      assert(ownership_destroys == destroyed && ownership_enters == entered + 1);
      objc3_completion suspended = {0};
      assert(objc3_task_get_completion(task, &suspended) == OBJC3_SERVICE_NOT_READY);
    }
    if (phase != 2)
      objc3_task_request_cancel(task);
    assert(objc3_executor_pump(executor, 100, &count) == OBJC3_SERVICE_OK);
    objc3_completion completion = {0};
    assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
    if (phase == 2) {
      assert(completion.kind == OBJC3_COMPLETION_SUCCESS && completion.has_payload);
      const void *returned;
      memcpy(&returned, completion.payload, sizeof(returned));
      assert(returned == identity);
    } else {
      assert(completion.kind == OBJC3_COMPLETION_CANCELLED && !completion.has_payload);
    }
    objc3_task_release(task);
    objc3_executor_release(executor);
  }
  assert(ownership_destroys == destroyed + 2);
  assert(ownership_enters == entered + (phase != 0));
}

int c_frame_owned_qualification(void) {
  for (int phase = 0; phase < 3; ++phase)
    exercise_owned_frame(phase);
  return ownership_destroys;
}
