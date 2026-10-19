#pragma once
#include "carrier-call.h"
@protocol Error @end
typedef CallStrong ThrowsFunction(CallStrong, int, int) throws(id<Error>);
typedef CallStrong (^ThrowsBlock)(CallStrong, int, int) throws(id<Error>);
int throws_created(void);
int throws_destroyed(void);
CallStrong throws_value(CallStrong value, int key, int fail) throws(id<Error>);
CallStrong throws_forward(CallStrong value, int key, int fail) throws(id<Error>);
CallWeak throws_weak(CallWeak value, int fail) throws(id<Error>);
CallUnsafe throws_unsafe(CallUnsafe value) throws(id<Error>);
CallOutcome throws_outcome(CallOutcome value, int fail) throws(id<Error>);
CallMixed throws_mixed(CallMixed value, int fail) throws(id<Error>);
CallBlock throws_payload_block(CallBlock value, int fail) throws(id<Error>);
int throws_scalar(CallStrong value, int key, int fail) throws(id<Error>);
ThrowsBlock throws_make_block(id capture);
CallStrong throws_many(CallStrong a, CallStrong b, CallStrong c, CallStrong d,
                       CallStrong e, CallStrong f, CallStrong g, int fail)
    throws(id<Error>);
@protocol ThrowsProtocol
- (CallStrong)value:(CallStrong)value key:(int)key fail:(int)fail throws(id<Error>);
- (CallWeak)weak:(CallWeak)value fail:(int)fail throws(id<Error>);
@end
__attribute__((objc_root_class))
@interface ThrowsCalls <ThrowsProtocol> { Class isa; }
+ (id)new;
+ (CallStrong)make:(int)key fail:(int)fail throws(id<Error>);
- (CallStrong)value:(CallStrong)value key:(int)key fail:(int)fail throws(id<Error>);
- (CallStrong)alternate:(CallStrong)value key:(int)key fail:(int)fail
    throws(id<Error>);
- (CallWeak)weak:(CallWeak)value fail:(int)fail throws(id<Error>);
- (CallStrong)native:(CallStrong)value throws(id<Error>);
@end
@interface ThrowsSubclass : ThrowsCalls
- (CallStrong)value:(CallStrong)value key:(int)key fail:(int)fail throws(id<Error>);
@end
#ifdef __cplusplus
struct ThrowsBase {
  virtual CallStrong apply(CallStrong value, int key, int fail) throws(id<Error>) = 0;
  virtual ~ThrowsBase() = default;
};
struct ThrowsAdjustment { virtual ~ThrowsAdjustment() = default; long padding = 42; };
struct ThrowsDerived : ThrowsAdjustment, ThrowsBase {
  CallStrong apply(CallStrong value, int key, int fail) throws(id<Error>) override;
  static CallStrong make(CallStrong value, int key, int fail) throws(id<Error>);
};
CallStrong throws_return_cleanup(int key, int mode) throws(id<Error>);
CallWeak throws_weak_return_cleanup(id object);
#endif
