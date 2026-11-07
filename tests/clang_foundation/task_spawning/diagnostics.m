#include <objc3/concurrency.h>

__attribute__((objc_root_class))
@interface MutableCapture
@end

// Compile with strict concurrency checking. Both direct and macro calls must
// diagnose the original object capture at the user call/argument site.
void reject_direct(MutableCapture *object) { // expected-note {{concurrent capture of 'object' is declared here}}
  (void)objc3_task_spawn(^void(void) __objc3_async_frame { (void)object; }); // expected-error {{non-Sendable type}}
}
void reject_macro(MutableCapture *object) { // expected-note {{concurrent capture of 'object' is declared here}}
  (void)OBJC_TASK_SPAWN(^void(void) __objc3_async_frame { (void)object; }); // expected-error {{non-Sendable type}}
}
void reject_detached_direct(MutableCapture *object) { // expected-note {{concurrent capture of 'object' is declared here}}
  (void)objc3_task_detached(^void(void) __objc3_async_frame { (void)object; }); // expected-error {{non-Sendable type}}
}
void reject_detached_macro(MutableCapture *object) { // expected-note {{concurrent capture of 'object' is declared here}}
  (void)OBJC_TASK_DETACHED(^void(void) __objc3_async_frame { (void)object; }); // expected-error {{non-Sendable type}}
}
void unused_child_handles(void) {
  objc3_task_spawn(^void(void) __objc3_async_frame {}); // expected-warning {{child task handle is unused}}
  OBJC_TASK_SPAWN(^void(void) __objc3_async_frame {}); // expected-warning {{child task handle is unused}}
}

// A group result is copied into independent Optional storage. Resource aliases
// and nontrivial enclosing records cannot acquire an implicit duplicate owner.
#ifndef __cplusplus
void group_next_close_handle(int);
typedef int GroupNextOwnedHandle __attribute__((
    objc_requires_cleanup(group_next_close_handle),
    objc_resource_family(GroupNextHandles),objc_invalid(-1)));
void reject_group_next_resource(void) {
  (void)__builtin_objc3_async_group_next_plan((Optional(GroupNextOwnedHandle) *)0); // expected-error {{requires supported plain storage or an ARC managed carrier}}
}
#if __has_feature(objc_arc)
struct GroupNextOwnedRecord { id value; };
void reject_group_next_record(void) {
  (void)__builtin_objc3_async_group_next_plan((Optional(struct GroupNextOwnedRecord) *)0); // expected-error {{not supported in an Objective-C 3 value carrier}}
}
void reject_group_next_unowned(void) {
  (void)__builtin_objc3_async_group_next_plan((Optional(__unsafe_unretained id) *)0); // expected-error {{requires supported plain storage or an ARC managed carrier}}
}
#endif
#else
void reject_group_next_cpp_plan(void) {
  (void)__builtin_objc3_async_group_next_plan((Optional(int) *)0); // expected-error {{native group-next plan requires a pointer}}
}
#endif
