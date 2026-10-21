#pragma once
#include "carrier-throws-call.h"
typedef Optional(CallStrong) NestedStrong;
typedef Optional(CallWeak) NestedWeak;
typedef Optional(CallBlock) NestedBlock;
typedef Result(CallStrong, CallWeak) NestedMixed;
typedef Optional(NestedMixed) NestedDeep;
typedef Optional(Optional(int)) NestedPlain;
typedef Result(NestedPlain, CallStrong) NestedPlainMixed;
NestedStrong nested_make(int key);
NestedStrong nested_identity(NestedStrong value);
NestedMixed nested_mixed(NestedMixed value);
NestedDeep nested_deep(NestedDeep value);
NestedStrong nested_throwing(NestedStrong value, int key, int fail)
    throws(id<Error>);
NestedStrong nested_forward(NestedStrong value, int key, int fail)
    throws(id<Error>);
int nested_state(NestedStrong value);
int nested_weak_state(NestedWeak value);
__attribute__((objc_root_class))
@interface NestedCalls { Class isa; }
+ (id)new;
- (NestedStrong)echo:(NestedStrong)value;
- (NestedMixed)mixed:(NestedMixed)value;
- (NestedStrong)direct:(NestedStrong)value __attribute__((objc_direct));
- (NestedStrong)throwing:(NestedStrong)value
                     key:(int)key
                    fail:(int)fail throws(id<Error>);
- (NestedStrong)native:(NestedStrong)value;
@end
#ifdef __cplusplus
struct NestedBase {
  virtual NestedStrong apply(NestedStrong value) = 0;
  virtual ~NestedBase() = default;
};
struct NestedPadding { virtual ~NestedPadding() = default; long padding = 42; };
struct NestedDerived : NestedPadding, NestedBase {
  NestedStrong apply(NestedStrong value) override;
};
NestedStrong nested_cleanup(int key, int fail) throws(id<Error>);
#endif
