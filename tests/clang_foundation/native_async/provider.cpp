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
int native_actor_owned(objc3_actor_t *first, objc3_actor_t *second,
                       bool fail) __objc3_async {
  NativeAsyncValue *value = [NativeAsyncValue new];
#if !__has_feature(objc_arc)
  struct release_owner {
    NativeAsyncValue *value;
    ~release_owner() { objc_release(value); }
  } owner{value};
#endif
  value->number = 42;
  int (^read)(void) = ^{
    return value->number;
  };
  auto actor_identity = [] {
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts);
    facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK)
      __builtin_trap();
    return facts.actor_identity;
  };
  if (actor_identity() != objc3_actor_identity(first))
    __builtin_trap();
  __objc3_await objc3::hop_to(second);
  if (actor_identity() != objc3_actor_identity(second))
    __builtin_trap();
  __objc3_await objc3::task_yield;
  if (actor_identity() != objc3_actor_identity(second))
    __builtin_trap();
  int result = __objc3_await native_identity(read());
  if (actor_identity() != objc3_actor_identity(second))
    __builtin_trap();
  __objc3_await objc3::hop_to(first);
  if (actor_identity() != objc3_actor_identity(first))
    __builtin_trap();
  if (fail)
    throw result;
  return result;
}

static std::atomic<unsigned> result_destructions{0};
__attribute__((objc_root_class))
@interface NativeAsyncResult<NativeAsyncResultProtocol> {
  Class isa;
@public
  int value;
}
+ (id)new;
@end
@implementation NativeAsyncResult
+ (id)new {
  return class_createInstance(self, 0);
}
- (int)number {
  return value;
}
- (void)dealloc {
  ++result_destructions;
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {
}
#endif
@end

#if !__has_feature(objc_arc)
extern "C" id objc_retain(__unsafe_unretained id);
struct result_owner {
  id value;
  ~result_owner() { objc_release(value); }
};
#endif

NativeAsyncResult *native_result_object(int number, bool fail) __objc3_async {
  NativeAsyncResult *result = [NativeAsyncResult new];
#if !__has_feature(objc_arc)
  result_owner owner{result};
#endif
  result->value = number;
  __objc3_await objc3::task_yield;
  if (fail)
    throw number;
  return result;
}

id<NativeAsyncResultProtocol> native_result_protocol(int number) __objc3_async {
  return __objc3_await native_result_object(number);
}

native_result_reader native_result_block(int number) __objc3_async {
  NativeAsyncResult *result = [NativeAsyncResult new];
#if !__has_feature(objc_arc)
  result_owner owner{result};
#endif
  result->value = number;
  __objc3_await objc3::task_yield;
  // MRC must copy this frame-local Block before destroying its capture storage.
  return ^(int delta) {
    return result->value + delta;
  };
}

NativeAsyncResult *native_result_relay(objc3_actor_t *actor,
                                       int number) __objc3_async {
  NativeAsyncResult *result = __objc3_await native_result_object(number);
#if !__has_feature(objc_arc)
  result_owner owner{objc_retain(result)};
#endif
  if (actor)
    __objc3_await objc3::hop_to(actor);
  __objc3_await objc3::task_yield;
  extern void objc3_native_async_result_checkpoint(int, const char *);
  objc3_native_async_result_checkpoint(result->value, "owned result");
  return result;
}

__attribute__((noinline)) void
objc3_native_async_result_checkpoint(int value, const char *kind) {
  if (value < 0 || kind[0] != 'o')
    __builtin_trap();
}

NativeAsyncResult *native_result_shared(
    objc3::task_handle<NativeAsyncResult *> dependency) __objc3_async {
  NativeAsyncResult *result = __objc3_await dependency;
#if !__has_feature(objc_arc)
  result_owner owner{objc_retain(result)};
#endif
  __objc3_await objc3::task_yield;
  return result;
}

NativeAsyncResult *native_result_lazy(
    objc3::task<NativeAsyncResult *> dependency) __objc3_async {
  NativeAsyncResult *result = __objc3_await std::move(dependency);
#if !__has_feature(objc_arc)
  result_owner owner{objc_retain(result)};
#endif
  __objc3_await objc3::task_yield;
  return result;
}

id native_result_nil() __objc3_async {
  __objc3_await objc3::task_yield;
  return (id)0;
}
native_result_reader native_result_nil_block() __objc3_async {
  __objc3_await objc3::task_yield;
  return (native_result_reader)0;
}
extern "C" Class objc_getClass(const char *);
Class native_result_class() __objc3_async {
  __objc3_await objc3::task_yield;
  return objc_getClass("NativeAsyncResult");
}
int native_result_number(__unsafe_unretained id value) {
  return ((NativeAsyncResult *)value)->value;
}
unsigned native_result_destructions() { return result_destructions.load(); }
#endif
