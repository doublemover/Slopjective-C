#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <objc/message.h>
id class_createInstance(Class type, size_t extra) __attribute__((ns_returns_retained));
static int destroyed;
__attribute__((objc_root_class))
@interface NamesDebuggerRoot { Class isa; @public int slot; }
+ (id)new;
+ (int)tag;
- (int)value;
- (int)directValue __attribute__((objc_direct));
+ (int)directTag __attribute__((objc_direct));
@end
@implementation NamesDebuggerRoot
+ (id)new { return class_createInstance(self, 0); }
+ (int)tag { return 42; }
- (int)value { return slot; }
- (int)directValue { return slot + 1; }
+ (int)directTag { return 43; }
- (void)dealloc { ++destroyed; object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
@interface NamesDebuggerRoot (Extension)
- (int)categoryDirect __attribute__((objc_direct));
@end
@implementation NamesDebuggerRoot (Extension)
- (int)categoryDirect { return slot + 2; }
@end
int names_debugger_invoke(id object, SEL selector) {
  return ((int (*)(id, SEL))objc_msg_lookup(object, selector))(object, selector);
}
int names_debugger_value(void) {
  int observed = 0;
  {
    NamesDebuggerRoot *object = [NamesDebuggerRoot new];
    object->slot = 42;
    observed = object->slot;
#if !__has_feature(objc_arc)
    objc_release(object);
#endif
  }
  return observed == 42 && destroyed == 1 ? 42 : 0;
}
