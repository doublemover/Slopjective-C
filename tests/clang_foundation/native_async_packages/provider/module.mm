#include <objc3c/runtime/objc3_task.hpp>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
id class_createInstance(Class, size_t) __attribute__((ns_returns_retained));

@protocol Error @end
@protocol NativeModuleError <Error> @end

__attribute__((objc_root_class))
@interface NativeModuleResult<NativeModuleError> {
  Class isa;
  int value;
}
+ (id)new;
- (void)setNumber:(int)number;
- (int)number;
@end
@implementation NativeModuleResult
+ (id)new {
  return class_createInstance(self, 0);
}
- (void)setNumber:(int)number {
  value = number;
}
- (int)number {
  return value;
}
- (void)dealloc {
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {
}
#endif
@end
using native_module_reader = int (^)(int);

#if !__has_feature(objc_arc)
struct module_result_owner {
  id value;
  ~module_result_owner() { objc_release(value); }
};
#endif

NativeModuleResult *native_module_object(int value) __objc3_async {
  NativeModuleResult *result = [NativeModuleResult new];
#if !__has_feature(objc_arc)
  module_result_owner owner{result};
#endif
  [result setNumber:value];
  __objc3_await objc3::task_yield;
  return result;
}

native_module_reader native_module_block(int value) __objc3_async {
  NativeModuleResult *result = [NativeModuleResult new];
#if !__has_feature(objc_arc)
  module_result_owner owner{result};
#endif
  [result setNumber:value];
  __objc3_await objc3::task_yield;
  return ^(int delta) {
    return [result number] + delta;
  };
}

int native_module_value(int value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value + 1;
}

int native_module_checked(int value, bool fail)
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  NativeModuleResult *error = [NativeModuleResult new];
#if !__has_feature(objc_arc)
  module_result_owner owner{error};
#endif
  [error setNumber:value];
  __objc3_await objc3::task_yield;
  if (fail) __objc3_throw((id<NativeModuleError>)error);
  return value;
}
