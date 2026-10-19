#include "carrier-runtime-call.h"
#include <objc/runtime.h>
RuntimeStrongBlock runtime_make_block(int key) {
  id object = call_object(key);
  return ^CallStrong(CallStrong value) {
    if (__objc3_has_value(value)) return value;
    return __objc3_some(id, object);
  };
}
@implementation RuntimeCalls
+ (instancetype)new { return class_createInstance(self, 0); }
+ (CallStrong)classMake:(int)key { return call_make(key); }
- (void)dealloc { object_dispose(self); }
- (CallStrong)make:(int)key { return call_make(key); }
- (CallStrong)direct:(CallStrong)value { return value; }
- (CallStrong)echo:(CallStrong)value { return value; }
- (CallWeak)echoWeak:(CallWeak)value { return value; }
- (CallUnsafe)echoUnsafe:(CallUnsafe)value { return value; }
- (CallOutcome)echoOutcome:(CallOutcome)value { return value; }
- (CallMixed)echoMixed:(CallMixed)value { return value; }
- (CallBlock)echoBlock:(CallBlock)value { return value; }
- (void)replace:(CallStrong)value key:(int)key { value = call_make(key); }
- (CallStrong)many:(CallStrong)a b:(CallStrong)b c:(CallStrong)c
                 d:(CallStrong)d e:(CallStrong)e f:(CallStrong)f
                 g:(CallStrong)g last:(int)last {
  if (last != 7 || !__objc3_has_value(a) || !__objc3_has_value(b) ||
      !__objc3_has_value(c) || !__objc3_has_value(d) ||
      !__objc3_has_value(e) || !__objc3_has_value(f)) __builtin_trap();
  return g;
}
- (CallStrong)throwNative:(CallStrong)value {
  value = call_make(30);
  @throw call_object(31);
}
#ifdef __cplusplus
- (CallStrong)throwCpp:(CallStrong)value {
  value = call_make(32);
  throw 42;
}
#endif
@end
@implementation RuntimeSubclass
- (CallStrong)echo:(CallStrong)value { return [super echo:value]; }
@end
