#pragma once
#include "carrier-throws-call.h"
int direct_initialized(void);
__attribute__((objc_root_class))
@interface DirectCalls { Class isa; }
+ (id)new;
+ (CallStrong)echo:(CallStrong)value
              fail:(int)fail throws(id<Error>)__attribute__((objc_direct));
- (CallStrong)echo:(CallStrong)value
               key:(int)key
              fail:(int)fail throws(id<Error>)__attribute__((objc_direct));
- (CallWeak)weak:(CallWeak)value
            fail:(int)fail throws(id<Error>)__attribute__((objc_direct));
- (CallMixed)mixed:(CallMixed)value
              fail:(int)fail throws(id<Error>)__attribute__((objc_direct));
- (CallBlock)block:(CallBlock)value
              fail:(int)fail throws(id<Error>)__attribute__((objc_direct));
- (int)command __attribute__((objc_direct));
- (CallStrong)native:(CallStrong)value throws(id<Error>)
                         __attribute__((objc_direct));
#ifdef __cplusplus
- (CallStrong)cleanup:(int)key throws(id<Error>)__attribute__((objc_direct));
#endif
@end
