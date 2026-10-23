#ifndef NATIVE_ISOLATION_PROVIDER_H
#define NATIVE_ISOLATION_PROVIDER_H
#include "objc3_runtime_services.h"
@protocol Sendable @end
__attribute__((objc_root_class))
@interface HopObject <Sendable> { Class isa; }
+ (id)new __attribute__((nothrow));
- (int)number __attribute__((nothrow));
@end
actor class HopCounter : HopObject { int value; }
- (int)add:(int)amount
    __attribute__((nothrow, objc_executor(named("native.isolation"))));
- (int)read __attribute__((nothrow));
@end
#ifdef __cplusplus
extern "C" {
#endif
void hop_configure(objc3_executor_t *caller, objc3_executor_t *main,
                   objc3_executor_t *global, objc3_executor_t *named,
                   uint64_t actor_identity);
void *hop_new_counter(void);
int hop_arguments(void);
int hop_entries(void);
int hop_cleanups(void);
int hop_destroyed(void);
int hop_roundtrip(HopCounter *, objc3_executor_t *) __objc3_async_frame;
int hop_nil_roundtrip(void) __objc3_async_frame;
HopObject *hop_owned_roundtrip(void) __objc3_async_frame;
#ifdef __cplusplus
}
#endif
#endif
