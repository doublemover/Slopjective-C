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

objc3_async_frame native_module_frame(int value) {
  return native_module_value(value).release_native();
}
objc3_async_frame native_module_object_frame(int value) {
  return native_module_object(value).release_native();
}
objc3_async_frame native_module_block_frame(int value) {
  return native_module_block(value).release_native();
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

struct NativeModuleWorker {
  int bias;
  int value(int) const __objc3_async;
  int checked(int, bool) const __objc3_async __objc3_throws(id<NativeModuleError>);
  int operator()(int) const __objc3_async;
};
int NativeModuleWorker::value(int n) const __objc3_async {
  return bias + __objc3_await native_module_value(n - 1);
}
int NativeModuleWorker::checked(int n, bool fail) const
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  return __objc3_try(__objc3_await native_module_checked(n, fail));
}
int NativeModuleWorker::operator()(int n) const __objc3_async {
  return __objc3_await value(n);
}

struct NativeModuleValue { int number; long padding[5]; };
using native_module_complex = __complex__ double;
using native_module_data = int NativeModuleValue::*;
NativeModuleValue native_module_record(int n, bool fail)
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  int value = __objc3_try(__objc3_await native_module_checked(n, fail));
  return NativeModuleValue{value, {1, 2, 3, 4, 5}};
}
native_module_complex native_module_complex_value(int n, bool fail)
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  int value = __objc3_try(__objc3_await native_module_checked(n, fail));
  native_module_complex result;
  __real__ result = value; __imag__ result = 2;
  return result;
}
native_module_data native_module_data_value(bool fail)
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  (void)__objc3_try(__objc3_await native_module_checked(42, fail));
  return &NativeModuleValue::number;
}

@protocol NativeModuleMethodProtocol
- (int)checked:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>);
@end
__attribute__((objc_root_class))
@interface NativeModuleMethods <NativeModuleMethodProtocol> { Class isa; }
+ (id)new;
- (int)checked:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>);
- (NativeModuleValue)record:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>);
- (NativeModuleResult *)object:(int)value __objc3_async;
- (int)direct:(int)value __objc3_async __attribute__((objc_direct));
@end
@implementation NativeModuleMethods
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc { object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
- (int)checked:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  return __objc3_try(__objc3_await native_module_checked(value, fail));
}
- (NativeModuleValue)record:(int)value fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeModuleError>) {
  return __objc3_try(__objc3_await native_module_record(value, fail));
}
- (NativeModuleResult *)object:(int)value __objc3_async {
  return __objc3_await native_module_object(value);
}
- (int)direct:(int)value __objc3_async __attribute__((objc_direct)) {
  __objc3_await objc3::task_yield;
  return value;
}
@end

using native_module_async_block = int (^)(int) __objc3_async;
using native_module_checked_block = NativeModuleValue (^)(int, bool)
    __objc3_async __objc3_throws(id<NativeModuleError>);
native_module_async_block native_module_async_factory(int bias) __objc3_async {
  __objc3_await objc3::task_yield;
  return ^int(int delta) __objc3_async {
    return bias + __objc3_await native_module_value(delta - 1);
  };
}
native_module_checked_block native_module_checked_factory() __objc3_async {
  __objc3_await objc3::task_yield;
  return ^NativeModuleValue(int n, bool fail) __objc3_async __objc3_throws(id<NativeModuleError>) {
    return __objc3_try(__objc3_await native_module_record(n, fail));
  };
}
