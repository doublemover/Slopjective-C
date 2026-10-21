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
