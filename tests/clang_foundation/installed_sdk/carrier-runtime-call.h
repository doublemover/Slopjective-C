#pragma once
#include "carrier-call.h"
typedef CallStrong (^RuntimeStrongBlock)(CallStrong);
RuntimeStrongBlock runtime_make_block(int key);
@protocol RuntimeCarrierProtocol
- (CallStrong)echo:(CallStrong)value;
- (CallWeak)echoWeak:(CallWeak)value;
@end
__attribute__((objc_root_class))
@interface RuntimeCalls <RuntimeCarrierProtocol> { Class isa; }
+ (instancetype)new;
+ (CallStrong)classMake:(int)key;
- (CallStrong)make:(int)key;
- (CallStrong)direct:(CallStrong)value __attribute__((objc_direct));
- (CallStrong)echo:(CallStrong)value;
- (CallWeak)echoWeak:(CallWeak)value;
- (CallUnsafe)echoUnsafe:(CallUnsafe)value;
- (CallOutcome)echoOutcome:(CallOutcome)value;
- (CallMixed)echoMixed:(CallMixed)value;
- (CallBlock)echoBlock:(CallBlock)value;
- (void)replace:(CallStrong)value key:(int)key;
- (CallStrong)many:(CallStrong)a b:(CallStrong)b c:(CallStrong)c
                 d:(CallStrong)d e:(CallStrong)e f:(CallStrong)f
                 g:(CallStrong)g last:(int)last;
- (CallStrong)throwNative:(CallStrong)value;
#ifdef __cplusplus
- (CallStrong)throwCpp:(CallStrong)value;
#endif
@end
@interface RuntimeSubclass : RuntimeCalls
- (CallStrong)echo:(CallStrong)value;
@end
