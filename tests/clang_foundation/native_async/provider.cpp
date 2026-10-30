#include "provider.h"
#ifdef OBJC3_ACTOR_LIFETIME_INTERNAL_TESTING
#include "runtime/concurrency/service_actor.h"
#include <barrier>
extern "C" void *actor_control_retain(void *) __asm__("objc_retain");
extern "C" void actor_control_release(void *) __asm__("objc_release");
#endif

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

#ifndef OBJC3_IR_ASSURANCE_BOUNDED
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
// Raw aliases deliberately bypass ARC return-value handshakes: this fixture
// must enqueue a real autorelease in both ownership modes, not release a local
// strong reference before suspension and accidentally pass without a pool.
extern "C" void *pool_create_instance(Class, __SIZE_TYPE__)
    __asm__("class_createInstance");
extern "C" void *pool_autorelease(void *) __asm__("objc_autorelease");
__attribute__((objc_root_class))
@interface NativeAsyncPoolWitness {
  Class isa;
@public
  std::atomic<unsigned> *counter;
}
+ (void)temporary:(void *)counter;
@end
@implementation NativeAsyncPoolWitness
+ (void)temporary:(void *)counter {
  void *raw = pool_create_instance(self, 0);
  ((__bridge NativeAsyncPoolWitness *)raw)->counter =
      static_cast<std::atomic<unsigned> *>(counter);
  pool_autorelease(raw);
}
- (void)dealloc {
  ++*counter;
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

int native_pool_slices(std::shared_ptr<std::atomic<unsigned>> destroyed,
                       objc3_actor_t *first, objc3_actor_t *second,
                       objc3::task_handle<int> completed) __objc3_async {
  @autoreleasepool {
    @autoreleasepool {
      NativeAsyncValue *value = [NativeAsyncValue new];
#if !__has_feature(objc_arc)
      struct release_owner {
        NativeAsyncValue *value;
        ~release_owner() { objc_release(value); }
      } owner{value};
#endif
      value->number = 42;
      [NativeAsyncPoolWitness temporary:destroyed.get()];
      __objc3_await objc3::task_yield;
      if (*destroyed != 1 || value->number != 42) __builtin_trap();
      [NativeAsyncPoolWitness temporary:destroyed.get()];
      int result = __objc3_await completed;
      if (*destroyed != 2 || result != 42) __builtin_trap();
      if (first) {
        [NativeAsyncPoolWitness temporary:destroyed.get()];
        __objc3_await objc3::hop_to(second);
        if (*destroyed != 3 || value->number != 42) __builtin_trap();
        [NativeAsyncPoolWitness temporary:destroyed.get()];
        __objc3_await objc3::hop_to(first);
        if (*destroyed != 4 || value->number != 42) __builtin_trap();
      }
      return value->number;
    }
  }
}

static std::atomic<unsigned> actor_owned_creations{0};
unsigned native_actor_owned_creations() { return actor_owned_creations.load(); }
int native_actor_owned(objc3_actor_t *first, objc3_actor_t *second,
                       bool fail) __objc3_async {
  NativeAsyncValue *value = [NativeAsyncValue new];
  ++actor_owned_creations;
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

static std::atomic<unsigned> checked_destructions{0};
__attribute__((objc_root_class))
@interface NativeAsyncFailure<NativeFirstError> { Class isa; }
+ (id)new;
@end
@implementation NativeAsyncFailure
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc { ++checked_destructions; object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
unsigned native_checked_destructions() { return checked_destructions.load(); }

int native_checked_leaf(std::shared_ptr<async_state> state, bool fail, int kind)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  frame_guard guard(state);
  defer { ++state->deferred; }
  ++state->parents;
  NativeAsyncFailure *error = [NativeAsyncFailure new];
#if !__has_feature(objc_arc)
  result_owner owner{error};
#endif
  __objc3_await objc3::task_yield;
  if (kind == 1) throw 73;
  if (kind == 2) @throw error;
  if (fail) __objc3_throw((id<NativeFirstError>)error);
  return 42;
}
int native_checked_parent(std::shared_ptr<async_state> state, bool fail, int kind)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  frame_guard guard(state);
  return __objc3_try(__objc3_await native_checked_leaf(state, fail, kind));
}
int native_checked_recover(std::shared_ptr<async_state> state, bool fail, int kind)
    __objc3_async {
  __objc3_do {
    return __objc3_try(__objc3_await native_checked_parent(state, fail, kind));
  } __objc3_catch(id<NativeFirstError> error) {
    if (!error) __builtin_trap();
    extern void objc3_native_async_error_checkpoint(int, __unsafe_unretained id);
    objc3_native_async_error_checkpoint(96, error);
    // The handler owns the payload through another real suspension.
    __objc3_await objc3::task_yield;
    return 96;
  }
}
int native_checked_optional(std::shared_ptr<async_state> state, bool fail)
    __objc3_async {
  auto result = __objc3_try_optional(
      __objc3_await native_checked_leaf(state, fail));
  return __objc3_match(result) { case __objc3_case_some(let value) => value; case __objc3_case_none => 0; };
}
int native_checked_force(std::shared_ptr<async_state> state) __objc3_async {
  return __objc3_try_force(__objc3_await native_checked_leaf(state, false));
}
static int checked_sync_failure(id<NativeFirstError> error, bool fail)
    __objc3_throws(id<NativeFirstError>) {
  if (fail) __objc3_throw(error);
  return 42;
}
int native_checked_sync(std::shared_ptr<async_state> state, bool fail)
    __objc3_async {
  frame_guard guard(state);
  NativeAsyncFailure *error = [NativeAsyncFailure new];
#if !__has_feature(objc_arc)
  result_owner owner{error};
#endif
  __objc3_await objc3::task_yield;
  __objc3_do { return __objc3_try(checked_sync_failure(error, fail)); }
  __objc3_catch(id<NativeFirstError> caught) { return caught ? 96 : 0; }
}
int native_checked_cleanup(std::shared_ptr<async_state> state)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  struct throwing_cleanup { ~throwing_cleanup() noexcept(false) { throw 91; } };
  throwing_cleanup cleanup;
  return __objc3_try(__objc3_await native_checked_leaf(state, true));
}
int native_checked_native_handlers(std::shared_ptr<async_state> state)
    __objc3_async {
  __objc3_do {
    @try {
      try {
        return __objc3_try(__objc3_await native_checked_leaf(state, true));
      } catch (...) { return -1; }
    } @catch (id) { return -2; }
  } __objc3_catch(id<NativeFirstError> error) { return error ? 96 : 0; }
}

__attribute__((noinline)) void
objc3_native_async_error_checkpoint(int value, __unsafe_unretained id error) {
  if (value != 96 || !error) __builtin_trap();
}

int native_checked_cleanup_caught(std::shared_ptr<async_state> state)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  try {
    struct cleanup { ~cleanup() noexcept(false) { throw 91; } };
    cleanup owned;
    return __objc3_try(__objc3_await native_checked_leaf(state, true));
  } catch (int value) { return value == 91 ? 42 : 0; }
}

NativeAsyncResult *native_checked_result_object(std::shared_ptr<async_state> state,
                                               bool fail)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  (void)__objc3_try(__objc3_await native_checked_leaf(std::move(state), fail));
  return __objc3_await native_result_object(42);
}
native_result_reader native_checked_result_block(std::shared_ptr<async_state> state,
                                                 bool fail)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  (void)__objc3_try(__objc3_await native_checked_leaf(std::move(state), fail));
  return __objc3_await native_result_block(42);
}
void native_checked_result_void(std::shared_ptr<async_state> state, bool fail)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  (void)__objc3_try(__objc3_await native_checked_leaf(std::move(state), fail));
  return;
}
int native_checked_result_kinds(std::shared_ptr<async_state> state, unsigned kind)
    __objc3_async {
  __objc3_do {
    NativeAsyncResult *object = __objc3_try(
        __objc3_await native_checked_result_object(state, kind == 1));
#if !__has_feature(objc_arc)
    result_owner owner{objc_retain(object)};
#endif
    native_result_reader block = __objc3_try(
        __objc3_await native_checked_result_block(state, kind == 2));
    int value = native_result_number(object) + block(0);
    __objc3_try(__objc3_await native_checked_result_void(state, kind == 3));
    return value == 84 ? 42 : 0;
  } __objc3_catch(id<NativeFirstError> error) {
    __objc3_await objc3::task_yield;
    return error ? 96 : 0;
  }
}
int native_checked_untyped(std::shared_ptr<async_state> state)
    __objc3_async __objc3_throws {
  frame_guard guard(state);
  NativeAsyncFailure *error = [NativeAsyncFailure new];
#if !__has_feature(objc_arc)
  result_owner owner{error};
#endif
  __objc3_await objc3::task_yield;
  __objc3_throw((id<Error>)error);
}
int native_checked_untyped_recover(std::shared_ptr<async_state> state)
    __objc3_async {
  __objc3_do {
    return __objc3_try(__objc3_await native_checked_untyped(std::move(state)));
  } __objc3_catch(id<Error> error) {
    __objc3_await objc3::task_yield;
    return error ? 96 : 0;
  }
}
int native_checked_indirect(std::shared_ptr<async_state> state) __objc3_async {
  using callback = int (*)(std::shared_ptr<async_state>, bool, int)
      __objc3_async __objc3_throws(id<NativeFirstError>);
  callback function = native_checked_leaf;
  __objc3_do {
    return __objc3_try(__objc3_await function(std::move(state), true, 0));
  } __objc3_catch(id<NativeFirstError> error) { return error ? 96 : 0; }
}
int native_checked_hop(objc3_actor_t *first, objc3_actor_t *second,
                        std::shared_ptr<async_state> state) __objc3_async {
  __objc3_await objc3::hop_to(second);
  int value = __objc3_await native_checked_recover(std::move(state), true);
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
      facts.actor_identity != objc3_actor_identity(second)) __builtin_trap();
  __objc3_await objc3::hop_to(first);
  return value;
}

extern "C" __attribute__((noinline)) void
objc3_native_async_member_checkpoint(int result, int bias) {
  if (result != 42 || bias != 40) __builtin_trap();
}
int native_async_worker::value(std::shared_ptr<async_state> state, int n) const
    __objc3_async {
  frame_guard guard(state);
  int result = bias + __objc3_await native_leaf(state, n);
  if (n == 2) objc3_native_async_member_checkpoint(result, bias);
  return result;
}
int native_async_worker::checked(std::shared_ptr<async_state> state, bool fail) const
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  frame_guard guard(state);
  return __objc3_try(__objc3_await native_checked_leaf(state, fail));
}
int native_async_worker::constant(int n) __objc3_async {
  __objc3_await objc3::task_yield;
  return n;
}
int native_async_worker::operator()(int n) const __objc3_async {
  __objc3_await objc3::task_yield;
  return bias + n;
}
int native_async_worker::operator[](int n) const __objc3_async { return bias + n; }
int native_async_worker::operator-() const __objc3_async { return -bias; }
int native_async_worker::qualified() & __objc3_async { return bias; }
int native_async_worker::qualified() && __objc3_async { return bias + 1; }
int operator+(const native_async_worker &worker, int n) __objc3_async {
  __objc3_await objc3::task_yield;
  return worker.bias + n;
}
int native_member_apply(const native_async_worker &worker,
                        native_member_callback member,
                        std::shared_ptr<async_state> state, int n) __objc3_async {
  return __objc3_await (worker.*member)(std::move(state), n);
}
int native_async_worker::hopped(objc3_actor_t *actor,
                                std::shared_ptr<async_state> state) const __objc3_async {
  __objc3_await objc3::hop_to(actor);
  int result = __objc3_await value(std::move(state), 2);
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
      facts.actor_identity != objc3_actor_identity(actor)) __builtin_trap();
  return result;
}

async_checked_value native_checked_value(std::shared_ptr<async_state> state,
                                        bool fail, int kind)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  int value = __objc3_try(__objc3_await native_checked_leaf(state, fail, kind));
  return async_checked_value(std::move(state), value);
}
async_checked_value native_checked_value_relay(std::shared_ptr<async_state> state,
                                              bool fail)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  return __objc3_try(__objc3_await native_checked_value(std::move(state), fail));
}
std::string native_checked_text(std::shared_ptr<async_state> state, bool fail)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  (void)__objc3_try(__objc3_await native_checked_leaf(std::move(state), fail));
  return std::string(256, 'v');
}
async_checked_complex native_checked_complex(std::shared_ptr<async_state> state,
                                             bool fail)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  (void)__objc3_try(__objc3_await native_checked_leaf(std::move(state), fail));
  async_checked_complex result;
  __real__ result = 40; __imag__ result = 2;
  return result;
}
async_checked_data native_checked_data(std::shared_ptr<async_state> state, bool fail)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  (void)__objc3_try(__objc3_await native_checked_leaf(std::move(state), fail));
  return &async_checked_target::value;
}
async_checked_method native_checked_method(std::shared_ptr<async_state> state,
                                            bool fail)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  (void)__objc3_try(__objc3_await native_checked_leaf(std::move(state), fail));
  return &async_checked_target::add;
}
__attribute__((noinline))
void objc3_native_async_value_checkpoint(int value, int size) {
  asm volatile("" : : "r"(value), "r"(size) : "memory");
}
int native_checked_values(std::shared_ptr<async_state> state, unsigned kind)
    __objc3_async {
  __objc3_do {
    auto value = __objc3_try(__objc3_await native_checked_value_relay(state, kind == 1));
    auto text = __objc3_try(__objc3_await native_checked_text(state, kind == 2));
    auto number = __objc3_try(__objc3_await native_checked_complex(state, kind == 3));
    auto data = __objc3_try(__objc3_await native_checked_data(state, kind == 4));
    auto method = __objc3_try(__objc3_await native_checked_method(state, kind == 5));
    async_checked_target target;
    __objc3_await objc3::task_yield;
    objc3_native_async_value_checkpoint(value.value, (int)text.size());
    return value.value == 42 && text == std::string(256, 'v') &&
           __real__ number + __imag__ number == 42 &&
           target.*data == 40 && (target.*method)(2) == 42 ? 42 : 0;
  } __objc3_catch(id<NativeFirstError> error) {
    __objc3_await objc3::task_yield;
    return error ? 96 : 0;
  }
}
int native_checked_value_optional(std::shared_ptr<async_state> state, bool fail)
    __objc3_async {
  auto value = __objc3_try_optional(__objc3_await native_checked_value(state, fail));
  auto text = __objc3_try_optional(__objc3_await native_checked_text(state, fail));
  auto number = __objc3_try_optional(__objc3_await native_checked_complex(state, fail));
  auto data = __objc3_try_optional(__objc3_await native_checked_data(state, fail));
  auto method = __objc3_try_optional(__objc3_await native_checked_method(state, fail));
  __objc3_await objc3::task_yield;
  if (fail) return !__objc3_has_value(value) && !__objc3_has_value(text) &&
      !__objc3_has_value(number) && !__objc3_has_value(data) &&
      !__objc3_has_value(method) ? 96 : 0;
  return __objc3_has_value(value) && __objc3_has_value(text) &&
      __objc3_has_value(number) && __objc3_has_value(data) &&
      __objc3_has_value(method) && state->values > 0 ? 42 : 0;
}
int native_checked_value_discard(std::shared_ptr<async_state> state, bool fail)
    __objc3_async {
  (void)__objc3_try_optional(__objc3_await native_checked_value(state, fail));
  (void)__objc3_try_force(__objc3_await native_checked_value(state, false));
  return 42;
}

#ifdef __OBJC__
static std::atomic<unsigned> method_destructions{0};
extern "C" __attribute__((noinline)) void
objc3_native_async_objc_method_checkpoint(int result, __unsafe_unretained id receiver) {
  if (result != 42 || !receiver) __builtin_trap();
}
@implementation NativeAsyncMethod
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc { ++method_destructions; object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
+ (int)constant:(int)value __objc3_async {
  __objc3_await objc3::task_yield;
  return value;
}
- (int)value:(std::shared_ptr<async_state>)state mode:(int)mode __objc3_async {
  frame_guard guard(state);
  __objc3_await objc3::task_yield;
  if (mode == 1) throw 73;
  if (mode == 2) @throw self;
  int result = bias + __objc3_await [self direct:2];
  if (mode == 3) objc3_native_async_objc_method_checkpoint(result, self);
  if (!_cmd) __builtin_trap();
  return result;
}
- (int)checked:(std::shared_ptr<async_state>)state fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  int result = __objc3_try(__objc3_await native_checked_leaf(state, false));
  if (fail) {
    NativeAsyncFailure *error = [NativeAsyncFailure new];
#if !__has_feature(objc_arc)
    result_owner owner{error};
#endif
    __objc3_await objc3::task_yield;
    __objc3_throw((id<NativeFirstError>)error);
  }
  return result + bias;
}
- (int)recover:(std::shared_ptr<async_state>)state fail:(bool)fail __objc3_async {
  __objc3_do {
    return __objc3_try(__objc3_await [self checked:state fail:fail]);
  } __objc3_catch(id<NativeFirstError> error) {
    __objc3_await objc3::task_yield;
    return error ? bias + 56 : 0;
  }
}
- (async_checked_value)record:(std::shared_ptr<async_state>)state fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  return __objc3_try(__objc3_await native_checked_value(state, fail));
}
- (NativeAsyncResult *)object:(int)value __objc3_async {
  return __objc3_await native_result_object(bias + value);
}
- (native_result_reader)reader:(int)value __objc3_async {
  return __objc3_await native_result_block(bias + value);
}
- (int)direct:(int)value __objc3_async __attribute__((objc_direct)) {
  __objc3_await objc3::task_yield;
  return value;
}
- (void)empty __objc3_async {}
- (int)hopped:(objc3_actor_t *)actor state:(std::shared_ptr<async_state>)state __objc3_async {
  __objc3_await objc3::hop_to(actor);
  int result = __objc3_await [self value:state mode:0];
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  if (objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
      facts.actor_identity != objc3_actor_identity(actor)) __builtin_trap();
  return result;
}
@end
@implementation NativeAsyncMethodChild
- (int)value:(std::shared_ptr<async_state>)state mode:(int)mode __objc3_async {
  return 1 + __objc3_await [super value:state mode:mode];
}
@end
unsigned native_method_destructions() { return method_destructions.load(); }
int native_method_consume(id<NativeAsyncMethodProtocol> receiver,
                           std::shared_ptr<async_state> state, bool fail) __objc3_async {
  __objc3_do {
    return __objc3_try(__objc3_await [receiver checked:state fail:fail]);
  } __objc3_catch(id<NativeFirstError> error) {
    __objc3_await objc3::task_yield;
    return error ? 96 : 0;
  }
}
#endif

#ifdef __OBJC__
extern "C" __attribute__((noinline)) void
objc3_native_async_block_checkpoint(int result, int bias, int progress) {
  if (result != 42 || bias != 40 || progress != 1) __builtin_trap();
}
objc3::task<int> native_block_lazy(std::shared_ptr<async_state> state, int mode) {
  NativeAsyncValue *value = [NativeAsyncValue new];
#if !__has_feature(objc_arc)
  result_owner owner{value};
#endif
  value->number = 40;
  std::string text = "captured Block survives its caller";
  __block int progress = 0;
  int bias = 40;
  int (^read)(int) = ^int(int delta) { return value->number + delta; };
  auto __unsafe_unretained block = ^int(int delta) __objc3_async {
    frame_guard guard(state);
    __objc3_await objc3::task_yield;
    ++progress;
    if (text != "captured Block survives its caller" || progress != 1)
      __builtin_trap();
    if (mode == 1) throw 109;
    if (mode == 2) @throw value;
    int result = __objc3_await native_leaf(state, read(delta));
    objc3_native_async_block_checkpoint(result, bias, progress);
    return result;
  };
  // The caller's Block, C++ captures and __block storage leave scope before
  // the returned lazy frame can resume, in ARC and MRC alike.
  return block(2);
}
objc3::task<async_checked_value>
native_block_checked(std::shared_ptr<async_state> state, bool fail) {
  auto __unsafe_unretained block = ^async_checked_value() __objc3_async __objc3_throws(id<NativeFirstError>) {
    frame_guard guard(state);
    __objc3_await objc3::task_yield;
    if (fail) {
      NativeAsyncFailure *error = [NativeAsyncFailure new];
#if !__has_feature(objc_arc)
      result_owner owner{error};
#endif
      __objc3_throw((id<NativeFirstError>)error);
    }
    return __objc3_try(__objc3_await native_checked_value(state, false));
  };
  return block();
}
int native_block_recover(std::shared_ptr<async_state> state, bool fail) __objc3_async {
  auto __unsafe_unretained block = ^int() __objc3_async {
    __objc3_do {
      auto checked = ^async_checked_value() __objc3_async __objc3_throws(id<NativeFirstError>) {
        return __objc3_try(__objc3_await native_checked_value(state, fail));
      };
      auto value = __objc3_try(__objc3_await checked());
      return value.value;
    } __objc3_catch(id<NativeFirstError> error) {
      __objc3_await objc3::task_yield;
      return error ? 96 : 0;
    }
  };
  return __objc3_await block();
}
native_async_block native_block_factory(int bias) __objc3_async {
  __objc3_await objc3::task_yield;
  return ^int(int delta) __objc3_async {
    __objc3_await objc3::task_yield;
    return bias + delta;
  };
}
objc3::task<int> native_block_hopped(objc3_actor_t *actor,
                                    std::shared_ptr<async_state> state) {
  std::string captured = "owned across actor hop";
  auto __unsafe_unretained block = ^int() __objc3_async {
    frame_guard guard(state);
    __objc3_await objc3::hop_to(actor);
    int result = __objc3_await native_leaf(state, 42);
    objc3_current_actor_facts facts{};
    facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    if (captured != "owned across actor hop" ||
        objc3_actor_current_facts(&facts) != OBJC3_SERVICE_OK ||
        facts.actor_identity != objc3_actor_identity(actor)) __builtin_trap();
    return result;
  };
  return block();
}
struct native_throwing_block_capture {
  std::shared_ptr<async_state> state;
  std::shared_ptr<bool> fail;
  native_throwing_block_capture(std::shared_ptr<async_state> s, std::shared_ptr<bool> f)
      : state(std::move(s)), fail(std::move(f)) { ++state->values; }
  native_throwing_block_capture(const native_throwing_block_capture &other)
      : state(other.state), fail(other.fail) {
    if (*fail) throw 109;
    ++state->values;
  }
  ~native_throwing_block_capture() { --state->values; }
};
bool native_block_copy_failure(std::shared_ptr<async_state> state) {
  auto fail = std::make_shared<bool>(false);
  native_throwing_block_capture capture(state, fail);
  auto __unsafe_unretained block = ^int() __objc3_async {
    __objc3_await objc3::task_yield;
    return capture.state ? 42 : 0;
  };
  *fail = true;
  try { auto lazy = block(); }
  catch (int value) { return value == 109; }
  return false;
}
#endif
#ifdef __OBJC__
static objc3::task<int> invoke_noescape(
    __attribute__((noescape)) native_async_block block) {
  return block(2);
}
@interface NativeAsyncMethod (AsyncBlocks)
+ (objc3::task<int>)invokeBlock:(native_async_block)__attribute__((noescape))block;
@end
@implementation NativeAsyncMethod (AsyncBlocks)
+ (objc3::task<int>)invokeBlock:(native_async_block)__attribute__((noescape))block {
  return block(2);
}
@end
objc3::task<int> native_block_noescape(std::shared_ptr<async_state> state, bool method) {
  __block int bias = 40;
  if (method)
    return [NativeAsyncMethod invokeBlock:^int(int delta) __objc3_async {
      frame_guard guard(state);
      __objc3_await objc3::task_yield;
      return ++bias + delta - 1;
    }];
  return invoke_noescape(^int(int delta) __objc3_async {
    frame_guard guard(state);
    __objc3_await objc3::task_yield;
    return ++bias + delta - 1;
  });
}
#endif

#ifdef __OBJC__
// Real object/actor lifetime witnesses run in every native ARC/MRC provider.
#include <cassert>
#include <thread>

namespace {
struct lexical_pool_task {
  struct promise_type {
    std::exception_ptr error;
    lexical_pool_task get_return_object() noexcept {
      return lexical_pool_task(std::coroutine_handle<promise_type>::from_promise(*this));
    }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() noexcept {}
    void unhandled_exception() noexcept { error = std::current_exception(); }
  };
  std::coroutine_handle<promise_type> handle;
  explicit lexical_pool_task(std::coroutine_handle<promise_type> handle) : handle(handle) {}
  lexical_pool_task(const lexical_pool_task &) = delete;
  lexical_pool_task(lexical_pool_task &&other) noexcept
      : handle(std::exchange(other.handle, {})) {}
  ~lexical_pool_task() { if (handle) handle.destroy(); }
};
struct pool_veto_suspend {
  bool await_ready() const noexcept { return false; }
  bool await_suspend(std::coroutine_handle<>) const noexcept { return false; }
  void await_resume() const noexcept {}
};
struct pool_throw_suspend {
  bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<>) const { throw 73; }
  void await_resume() const noexcept {}
};
lexical_pool_task lexical_pool_flow(std::atomic<unsigned> *destroyed) {
  @autoreleasepool {
    [NativeAsyncPoolWitness temporary:destroyed];
    @autoreleasepool {
      [NativeAsyncPoolWitness temporary:destroyed];
      co_await std::suspend_always{};
      assert(*destroyed == 2);
      [NativeAsyncPoolWitness temporary:destroyed];
      co_await std::suspend_never{};
      assert(*destroyed == 2); // A ready await retains the active lexical pools.
      co_await pool_veto_suspend{};
      assert(*destroyed == 3); // An extra drain on a vetoed suspension is safe.
      [NativeAsyncPoolWitness temporary:destroyed];
      try { co_await pool_throw_suspend{}; }
      catch (int error) {
        assert(error == 73 && *destroyed == 4);
        [NativeAsyncPoolWitness temporary:destroyed];
      }
    }
    assert(*destroyed == 5); // The catch ran inside a restored inner pool.
    [NativeAsyncPoolWitness temporary:destroyed];
  }
  assert(*destroyed == 6);
  co_return;
}
lexical_pool_task lexical_pool_abandon(std::atomic<unsigned> *destroyed) {
  @autoreleasepool { @autoreleasepool { @autoreleasepool {
  @autoreleasepool { @autoreleasepool { @autoreleasepool {
    [NativeAsyncPoolWitness temporary:destroyed];
    co_await std::suspend_always{};
    throw 99;
  } } } } } }
  co_return;
}
} // namespace

bool native_lexical_pool_witnesses() {
  std::atomic<unsigned> destroyed{0};
  {
    auto task = lexical_pool_flow(&destroyed);
    task.handle.resume();
    assert(destroyed == 2 && !task.handle.done());
    std::thread resume([&] { task.handle.resume(); });
    resume.join();
    assert(task.handle.done() && !task.handle.promise().error && destroyed == 6);
  }
  for (bool resume : {false, true}) {
    destroyed = 0;
    auto task = lexical_pool_abandon(&destroyed);
    task.handle.resume();
    assert(destroyed == 1 && !task.handle.done());
    std::thread finish([&] {
      if (resume) task.handle.resume();
      else { task.handle.destroy(); task.handle = {}; }
    });
    finish.join();
    assert(destroyed == 1);
    if (resume) {
      assert(task.handle.done() && task.handle.promise().error);
      try { std::rethrow_exception(task.handle.promise().error); }
      catch (int error) { assert(error == 99); }
    }
  }
  return true;
}

static std::atomic<unsigned> actor_association_destructions{0};
__attribute__((objc_root_class))
@interface RuntimeActorAssociationValue {
  Class isa;
@public
  int number;
  bool expectRetired;
}
+ (id)new;
@end
@implementation RuntimeActorAssociationValue
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc {
  if (expectRetired) {
    objc3_actor_t *found = nullptr;
    assert(objc3_actor_get_for_object((__bridge const void *)self, &found) ==
           OBJC3_SERVICE_CLOSED && !found);
  }
  ++actor_association_destructions;
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

struct ActorObjectJob {
  void *object;
  objc3_actor_t *actor;
  unsigned ran{0}, cleaned{0};
};
static void actor_object_job(void *opaque) {
  auto &job = *static_cast<ActorObjectJob *>(opaque);
  objc3_actor_t *found = nullptr;
  assert(objc3_actor_get_for_object(job.object, &found) == OBJC3_SERVICE_OK);
  assert(objc3_actor_identity(found) == objc3_actor_identity(job.actor));
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == objc3_actor_identity(found));
  auto *value = (__bridge RuntimeActorAssociationValue *)job.object;
  value->number = 42;
  ++job.ran;
  objc3_actor_release(found);
}
static void actor_object_cleanup(void *opaque) {
  ++static_cast<ActorObjectJob *>(opaque)->cleaned;
}

// A deterministic allocator fixture for the pinned non-GC GNUstep runtime.
// class_createInstance supplies the initial allocation and ARC class policy.
// Its allocation has one intptr_t reference-count prefix (gc_none.c); reuse
// resets that prefix only after the previous lifetime's native hidden-class
// destructor has run. The last lifetime uses ordinary object_dispose.
extern "C" Class object_getClass(id);
extern "C" Class object_setClass(id, Class);
extern "C" __SIZE_TYPE__ class_getInstanceSize(Class);
extern "C" SEL sel_registerName(const char *);
extern "C" void (*class_getMethodImplementation(Class, SEL))(void);
static void *actor_reuse_storage;
static unsigned actor_reuse_destroyed;
__attribute__((objc_root_class))
@interface RuntimeActorReuseValue {
  Class isa;
@public
  bool preserveStorage;
  bool expectRetired;
}
+ (id)new;
@end
@implementation RuntimeActorReuseValue
+ (id)new {
  if (!actor_reuse_storage) return class_createInstance(self, 0);
  void *storage = std::exchange(actor_reuse_storage, nullptr);
  auto *allocation = static_cast<unsigned char *>(storage) - sizeof(__INTPTR_TYPE__);
  __builtin_memset(allocation, 0, sizeof(__INTPTR_TYPE__) + class_getInstanceSize(self));
  object_setClass((__bridge id)storage, self);
#if __has_feature(objc_arc)
  return (__bridge_transfer id)storage;
#else
  return (id)storage;
#endif
}
- (void)dealloc {
  if (expectRetired) {
    objc3_actor_t *found = nullptr;
    assert(objc3_actor_get_for_object((__bridge const void *)self, &found) == OBJC3_SERVICE_CLOSED && !found);
  }
  ++actor_reuse_destroyed;
  if (preserveStorage) {
    auto selector = sel_registerName(".cxx_destruct");
    // object_getClass deliberately hides GNUstep association subclasses.
    // The actual isa owns the .cxx_destruct routine that disposes their state.
    __unsafe_unretained Class actual_class;
    __builtin_memcpy((void *)&actual_class, (__bridge const void *)self, sizeof(actual_class));
    auto destructor = class_getMethodImplementation(actual_class, selector);
    assert(destructor);
    reinterpret_cast<void (*)(__unsafe_unretained id, SEL)>(destructor)(self, selector);
    __builtin_memcpy((void *)&actual_class, (__bridge const void *)self, sizeof(actual_class));
    assert(actual_class == object_getClass(self));
    actor_reuse_storage = (__bridge void *)self;
  } else object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
static void actor_object_lifetime_reuse(objc3_executor_t *executor) {
  const auto before = actor_reuse_destroyed;
  objc3_actor_t *old = nullptr;
  assert(objc3_actor_create(executor, &old) == OBJC3_SERVICE_OK);
  const auto old_identity = objc3_actor_identity(old);
  __attribute__((objc_precise_lifetime)) RuntimeActorReuseValue *second = (RuntimeActorReuseValue *)0;
  objc3_actor_t *new_actors[8]{};
  @autoreleasepool {
    RuntimeActorReuseValue *first = [RuntimeActorReuseValue new];
    first->preserveStorage = true;
    first->expectRetired = true;
    void *address = (__bridge void *)first;
    assert(objc3_actor_bind_object(address, old) == OBJC3_SERVICE_OK);
#ifdef OBJC3_ACTOR_LIFETIME_INTERNAL_TESTING
    const auto old_generation = objc3::services::actor_object_lifetime(address);
    assert(old_generation != 0);
#endif
    assert(objc3_actor_close(old) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(old) == OBJC3_SERVICE_OK);
    assert(objc3_actor_unbind_object(address) == OBJC3_SERVICE_OK);
    objc3_actor_t *found = nullptr;
    assert(objc3_actor_get_or_create_for_object(address, executor, &found) == OBJC3_SERVICE_CLOSED && !found);
#if !__has_feature(objc_arc)
    objc_release(first);
#endif
    first = (RuntimeActorReuseValue *)0;
    assert(actor_reuse_destroyed == before + 1 && actor_reuse_storage == address);
    second = [RuntimeActorReuseValue new];
    assert((__bridge void *)second == address);
    std::thread entrants[8];
    for (unsigned i = 0; i != 8; ++i)
      entrants[i] = std::thread([&, i] {
        @autoreleasepool {
          assert(objc3_actor_get_or_create_for_object(address, executor, &new_actors[i]) == OBJC3_SERVICE_OK);
        }
      });
    for (auto &entrant : entrants) entrant.join();
    for (auto *actor : new_actors)
      assert(actor == new_actors[0] && objc3_actor_identity(actor) != old_identity);
    // Publish B's explicit generation while A's identity can still have an
    // autorelease lease. A's delayed destructor must not erase this binding.
    assert(objc3_actor_bind_object(address, new_actors[0]) == OBJC3_SERVICE_OK);
    assert(objc3_actor_close(new_actors[0]) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(new_actors[0]) == OBJC3_SERVICE_OK);
    second->expectRetired = true;
    assert(objc3_actor_unbind_object(address) == OBJC3_SERVICE_OK);
#ifdef OBJC3_ACTOR_LIFETIME_INTERNAL_TESTING
    assert(objc3::services::actor_object_lifetime(address) != old_generation);
    // Deterministically deliver A's old teardown callback after B retired.
    // This supplements the real association/autorelease destruction path.
    objc3::services::forget_retired_actor_object(address, old_generation);
    objc3_actor_t *retired = nullptr;
    assert(objc3_actor_get_for_object(address, &retired) == OBJC3_SERVICE_CLOSED && !retired);
#endif
  }
  objc3_actor_t *found = nullptr;
  void *address = (__bridge void *)second;
  // A's delayed cleanup must preserve B's distinct retired generation too.
  assert(objc3_actor_get_for_object(address, &found) == OBJC3_SERVICE_CLOSED && !found);
  assert(objc3_actor_get_or_create_for_object(address, executor, &found) == OBJC3_SERVICE_CLOSED && !found);
  assert(objc3_actor_unbind_object(address) == OBJC3_SERVICE_CLOSED);
#if !__has_feature(objc_arc)
  objc_release(second);
#endif
  second = (RuntimeActorReuseValue *)0;
  assert(actor_reuse_destroyed == before + 2 && !actor_reuse_storage);
  for (auto *actor : new_actors) objc3_actor_release(actor);
  objc3_actor_release(old);
}

bool native_actor_object_association() {
  const auto before = actor_association_destructions.load();
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  objc3_actor_t *first = nullptr, *second = nullptr;
  assert(objc3_actor_create(executor, &first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(executor, &second) == OBJC3_SERVICE_OK);
  RuntimeActorAssociationValue *owner = [RuntimeActorAssociationValue new];
  void *raw = (__bridge void *)owner;
  objc3_actor_t *found = nullptr;
  assert(objc3_actor_get_for_object(raw, &found) == OBJC3_SERVICE_NOT_READY && !found);
  assert(objc3_actor_bind_object(raw, first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_bind_object(raw, first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_bind_object(raw, second) == OBJC3_SERVICE_INVALID_ARGUMENT);
  owner->expectRetired = true;
#if !__has_feature(objc_arc)
  objc_release(owner);
#endif
  owner = (RuntimeActorAssociationValue *)0;
  assert(actor_association_destructions == before); // Only binding pins object.
  assert(objc3_actor_get_for_object(raw, &found) == OBJC3_SERVICE_OK && found == first);
  objc3_actor_release(first); // Binding/lookup independently retain the actor.
  assert(objc3_actor_unbind_object(raw) == OBJC3_SERVICE_NOT_READY);
  ActorObjectJob job{raw, found};
  objc3_actor_hop_facts hop{};
  hop.struct_size = sizeof(hop);
  hop.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_enqueue(found, actor_object_job, &job, actor_object_cleanup,
                              &hop) == OBJC3_SERVICE_OK);
  assert(objc3_actor_close(found) == OBJC3_SERVICE_OK);
  assert(objc3_actor_unbind_object(raw) == OBJC3_SERVICE_NOT_READY);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(job.ran == 1 && job.cleaned == 1);
  assert(objc3_actor_unbind_object(raw) == OBJC3_SERVICE_OK);
  assert(actor_association_destructions == before + 1); // Dealloc reenters lookup.
  // raw no longer denotes a live receiver after unbind releases its final pin.
  objc3_actor_release(found);
  assert(objc3_actor_close(second) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(second) == OBJC3_SERVICE_OK);
  objc3_actor_release(second);

  RuntimeActorAssociationValue *raced = [RuntimeActorAssociationValue new];
  raw = (__bridge void *)raced;
  objc3_actor_t *candidates[8]{};
  objc3_service_status outcomes[8]{};
  std::thread registrations[8];
#ifdef OBJC3_ACTOR_LIFETIME_INTERNAL_TESTING
  std::barrier first_binding(12);
  std::atomic<bool> registrations_done{false};
  std::thread retainers[4];
  for (auto &retainer : retainers) retainer = std::thread([&] {
    first_binding.arrive_and_wait();
    do {
      auto *pin = actor_control_retain(raw);
      assert(pin == raw);
      actor_control_release(pin);
    } while (!registrations_done.load());
  });
#endif
  for (unsigned index = 0; index != 8; ++index) {
    assert(objc3_actor_create(executor, &candidates[index]) == OBJC3_SERVICE_OK);
    registrations[index] = std::thread([&, index] {
#ifdef OBJC3_ACTOR_LIFETIME_INTERNAL_TESTING
      first_binding.arrive_and_wait();
#endif
      outcomes[index] = objc3_actor_bind_object(raw, candidates[index]);
    });
  }
  unsigned winners = 0;
  objc3_actor_t *winner = nullptr, *replacement = nullptr;
  for (unsigned index = 0; index != 8; ++index) {
    registrations[index].join();
    if (outcomes[index] == OBJC3_SERVICE_OK) {
      ++winners;
      winner = candidates[index];
    } else {
      assert(outcomes[index] == OBJC3_SERVICE_INVALID_ARGUMENT);
      replacement = candidates[index];
    }
  }
#ifdef OBJC3_ACTOR_LIFETIME_INTERNAL_TESTING
  registrations_done = true;
  for (auto &retainer : retainers) retainer.join();
#endif
  assert(winners == 1 && replacement);
  assert(objc3_actor_get_for_object(raw, &found) == OBJC3_SERVICE_OK && found == winner);
  assert(objc3_actor_close(winner) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(winner) == OBJC3_SERVICE_OK);
  assert(objc3_actor_unbind_object(raw) == OBJC3_SERVICE_OK);
  assert(objc3_actor_bind_object(raw, winner) == OBJC3_SERVICE_CLOSED);
  assert(objc3_actor_bind_object(raw, replacement) == OBJC3_SERVICE_OK);
  // The retained old lookup cannot admit work after rebinding the live object.
  assert(objc3_actor_enqueue(found, actor_object_job, &job, actor_object_cleanup,
                              &hop) == OBJC3_SERVICE_CLOSED);
  objc3_actor_release(found);
  assert(objc3_actor_get_for_object(raw, &found) == OBJC3_SERVICE_OK && found == replacement);
  objc3_actor_release(found);
  assert(objc3_actor_close(replacement) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(replacement) == OBJC3_SERVICE_OK);
  raced->expectRetired = true;
  assert(objc3_actor_unbind_object(raw) == OBJC3_SERVICE_OK);
  assert(objc3_actor_unbind_object(raw) == OBJC3_SERVICE_CLOSED);
  assert(objc3_actor_get_or_create_for_object(raw, executor, &found) == OBJC3_SERVICE_CLOSED && !found);
#if !__has_feature(objc_arc)
  objc_release(raced);
#endif
  raced = (RuntimeActorAssociationValue *)0;
  assert(actor_association_destructions == before + 2);
  for (auto *candidate : candidates) {
    assert(objc3_actor_close(candidate) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(candidate) == OBJC3_SERVICE_OK);
    objc3_actor_release(candidate);
  }
  actor_object_lifetime_reuse(executor);
  objc3_executor_release(executor);
  return true;
}
#endif
#endif // !OBJC3_IR_ASSURANCE_BOUNDED
