#include "provider.h"

struct frame_guard {
  std::shared_ptr<async_state> state;
  explicit frame_guard(std::shared_ptr<async_state> state)
      : state(std::move(state)) {
    ++this->state->entered;
  }
  ~frame_guard() { ++state->destroyed; }
};

extern "C" __attribute__((noinline)) void
objc3_native_async_checkpoint(int value, const char *text) {
  if (value != 42 || text[0] != 'p')
    __builtin_trap();
}

int native_leaf(std::shared_ptr<async_state> state, int value) __objc3_async {
  frame_guard guard(state);
  __objc3_await objc3::task_yield;
  if (value < 0)
    throw value;
  return value;
}

int native_parent(std::shared_ptr<async_state> state, int value) __objc3_async {
  frame_guard guard(state);
  ++state->parents;
  defer { ++state->deferred; }
  std::string text = "parent frame survives suspension";
  auto identity = [](int x) { return x; };
  int (^block)(int) = ^(int x) {
    return x;
  };
  int result = __objc3_await native_leaf(state, block(identity(value)));
  if (value == 42)
    objc3_native_async_checkpoint(result, text.c_str());
  ++state->resumed;
  return result + 1;
}

int native_chain(unsigned depth) __objc3_async {
  if (!depth)
    return 0;
  return 1 + __objc3_await native_chain(depth - 1);
}

std::string native_text(std::string text) __objc3_async {
  __objc3_await objc3::task_yield;
  return text + " resumed";
}

const std::string native_const_text() __objc3_async {
  __objc3_await objc3::task_yield;
  return std::string("qualified result");
}

async_aligned native_aligned(int value) __objc3_async {
  alignas(512) unsigned char local[512]{};
  local[0] = 7;
  __objc3_await objc3::task_yield;
  if (reinterpret_cast<uintptr_t>(local) % 512 != 0)
    __builtin_trap();
  return {value + local[0]};
}

void native_void(std::shared_ptr<async_state> state) __objc3_async {
  frame_guard guard(std::move(state));
  __objc3_await objc3::task_yield;
  return;
}
void native_empty() __objc3_async {}
native_callback native_indirect = native_leaf;
int native_overload(sync_callback) { return 1; }
int native_overload(native_callback) { return 2; }
int native_error_kind(first_error_callback) { return 1; }
int native_error_kind(second_error_callback) { return 2; }
int native_error_kind(untyped_error_callback) { return 3; }
int native_error_kind(void (*)()) { return 4; }

#ifdef __OBJC__
extern "C" id class_createInstance(Class, __SIZE_TYPE__)
    __attribute__((ns_returns_retained));
extern "C" id object_dispose(__unsafe_unretained id);
extern "C" void objc_release(__unsafe_unretained id);
static std::atomic<unsigned> object_destructions{0};
__attribute__((objc_root_class))
@interface NativeAsyncValue {
  Class isa;
@public
  int number;
}
+ (id)new;
@end
@implementation NativeAsyncValue
+ (id)new {
  return class_createInstance(self, 0);
}
- (void)dealloc {
  ++object_destructions;
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {
}
#endif
@end

int native_owned(bool fail) __objc3_async {
  NativeAsyncValue *value = [NativeAsyncValue new];
#if !__has_feature(objc_arc)
  struct release_owner {
    NativeAsyncValue *value;
    ~release_owner() { objc_release(value); }
  } owner{value};
#endif
  value->number = 96;
  int (^read)(void) = ^{
    return value->number;
  };
  __objc3_await objc3::task_yield;
  if (fail)
    throw 96;
  return read();
}
unsigned native_object_destructions() { return object_destructions.load(); }
int native_object_error() __objc3_async {
  NativeAsyncValue *value = [NativeAsyncValue new];
#if !__has_feature(objc_arc)
  struct release_owner {
    NativeAsyncValue *value;
    ~release_owner() { objc_release(value); }
  } owner{value};
#endif
  value->number = 73;
  __objc3_await objc3::task_yield;
  @throw value;
}
int native_exception_number(__unsafe_unretained id value) {
  return ((NativeAsyncValue *)value)->number;
}
#endif
