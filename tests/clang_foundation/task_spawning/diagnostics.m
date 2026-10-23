#include <objc3/concurrency.h>

__attribute__((objc_root_class))
@interface MutableCapture
@end

// Compile with strict concurrency checking. Both direct and macro calls must
// diagnose the original object capture at the user call/argument site.
void reject_direct(MutableCapture *object) {
  (void)objc3_task_spawn(^void(void) __objc3_async_frame { (void)object; });
}
void reject_macro(MutableCapture *object) {
  (void)OBJC_TASK_SPAWN(^void(void) __objc3_async_frame { (void)object; });
}
void reject_detached_direct(MutableCapture *object) {
  (void)objc3_task_detached(^void(void) __objc3_async_frame { (void)object; });
}
void reject_detached_macro(MutableCapture *object) {
  (void)OBJC_TASK_DETACHED(^void(void) __objc3_async_frame { (void)object; });
}
void unused_child_handles(void) {
  objc3_task_spawn(^void(void) __objc3_async_frame {});
  OBJC_TASK_SPAWN(^void(void) __objc3_async_frame {});
}
