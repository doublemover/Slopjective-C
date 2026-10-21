#ifndef NATIVE_ASYNC_PROVIDER_H
#define NATIVE_ASYNC_PROVIDER_H
#include "objc3_task.hpp"
#include <atomic>
#include <memory>
#include <string>

struct async_state {
  std::atomic<int> entered{0}, destroyed{0}, resumed{0}, parents{0},
      deferred{0}, values{0}, copies{0}, moves{0};
};
int native_leaf(std::shared_ptr<async_state>, int) __objc3_async;
int native_parent(std::shared_ptr<async_state>, int) __objc3_async;
int native_chain(unsigned) __objc3_async;
std::string native_text(std::string) __objc3_async;
const std::string native_const_text() __objc3_async;
struct alignas(256) async_aligned {
  int value;
};
async_aligned native_aligned(int) __objc3_async;
void native_void(std::shared_ptr<async_state>) __objc3_async;
void native_empty() __objc3_async;
#ifdef __OBJC__
int native_owned(bool) __objc3_async;
int native_object_error() __objc3_async;
int native_actor_owned(objc3_actor_t *, objc3_actor_t *, bool) __objc3_async;
int native_pool_slices(std::shared_ptr<std::atomic<unsigned>>, objc3_actor_t *,
                       objc3_actor_t *, objc3::task_handle<int>) __objc3_async;
int native_exception_number(__unsafe_unretained id);
unsigned native_object_destructions();
@class NativeAsyncResult;
@protocol NativeAsyncResultProtocol
- (int)number;
@end
using native_result_reader = int (^)(int);
NativeAsyncResult *native_result_object(int, bool = false) __objc3_async;
id<NativeAsyncResultProtocol> native_result_protocol(int) __objc3_async;
native_result_reader native_result_block(int) __objc3_async;
NativeAsyncResult *native_result_relay(objc3_actor_t *, int) __objc3_async;
NativeAsyncResult *native_result_shared(
    objc3::task_handle<NativeAsyncResult *>) __objc3_async;
NativeAsyncResult *native_result_lazy(objc3::task<NativeAsyncResult *>) __objc3_async;
id native_result_nil() __objc3_async;
native_result_reader native_result_nil_block() __objc3_async;
Class native_result_class() __objc3_async;
int native_result_number(__unsafe_unretained id);
unsigned native_result_destructions();
#endif

using native_callback = int (*)(std::shared_ptr<async_state>,
                                int) __objc3_async;
extern native_callback native_indirect;
using sync_callback = int (*)(std::shared_ptr<async_state>, int);
int native_overload(sync_callback);
int native_overload(native_callback);
@protocol Error
@end
@protocol NativeFirstError <Error>
@end
@protocol NativeSecondError <Error>
@end
using first_error_callback = void (*)() __objc3_throws(id<NativeFirstError>);
using second_error_callback = void (*)() __objc3_throws(id<NativeSecondError>);
using untyped_error_callback = void (*)() __objc3_throws;
int native_error_kind(first_error_callback);
int native_error_kind(second_error_callback);
int native_error_kind(untyped_error_callback);
int native_error_kind(void (*)());
struct callback_owner {
  int choose(native_callback fn) { return native_overload(fn); }
};

unsigned native_checked_destructions();
int native_checked_leaf(std::shared_ptr<async_state>, bool, int = 0)
    __objc3_async __objc3_throws(id<NativeFirstError>);
int native_checked_parent(std::shared_ptr<async_state>, bool, int = 0)
    __objc3_async __objc3_throws(id<NativeFirstError>);
int native_checked_recover(std::shared_ptr<async_state>, bool, int = 0)
    __objc3_async;
int native_checked_optional(std::shared_ptr<async_state>, bool) __objc3_async;
int native_checked_force(std::shared_ptr<async_state>) __objc3_async;
int native_checked_sync(std::shared_ptr<async_state>, bool) __objc3_async;
int native_checked_cleanup(std::shared_ptr<async_state>)
    __objc3_async __objc3_throws(id<NativeFirstError>);
int native_checked_cleanup_caught(std::shared_ptr<async_state>)
    __objc3_async __objc3_throws(id<NativeFirstError>);
int native_checked_native_handlers(std::shared_ptr<async_state>) __objc3_async;

NativeAsyncResult *native_checked_result_object(std::shared_ptr<async_state>, bool)
    __objc3_async __objc3_throws(id<NativeFirstError>);
native_result_reader native_checked_result_block(std::shared_ptr<async_state>, bool)
    __objc3_async __objc3_throws(id<NativeFirstError>);
void native_checked_result_void(std::shared_ptr<async_state>, bool)
    __objc3_async __objc3_throws(id<NativeFirstError>);
int native_checked_result_kinds(std::shared_ptr<async_state>, unsigned) __objc3_async;
int native_checked_untyped(std::shared_ptr<async_state>) __objc3_async __objc3_throws;
int native_checked_untyped_recover(std::shared_ptr<async_state>) __objc3_async;
int native_checked_indirect(std::shared_ptr<async_state>) __objc3_async;
int native_checked_hop(objc3_actor_t *, objc3_actor_t *, std::shared_ptr<async_state>)
    __objc3_async;

// No default constructor: checked failure must not fabricate a result object.
struct alignas(64) async_checked_value {
  std::shared_ptr<async_state> state;
  int value;
  async_checked_value() = delete;
  async_checked_value(std::shared_ptr<async_state> s, int n)
      : state(std::move(s)), value(n) { ++state->values; }
  async_checked_value(const async_checked_value &other)
      : state(other.state), value(other.value) { ++state->values; ++state->copies; }
  async_checked_value(async_checked_value &&other) noexcept
      : state(other.state), value(other.value) { ++state->values; ++state->moves; }
  ~async_checked_value() { --state->values; }
};
struct async_checked_target {
  int value = 40;
  int add(int n) const { return value + n; }
};
using async_checked_data = int async_checked_target::*;
using async_checked_method = int (async_checked_target::*)(int) const;
using async_checked_complex = __complex__ double;
async_checked_value native_checked_value(std::shared_ptr<async_state>, bool, int = 0)
    __objc3_async __objc3_throws(id<NativeFirstError>);
async_checked_value native_checked_value_relay(std::shared_ptr<async_state>, bool)
    __objc3_async __objc3_throws(id<NativeFirstError>);
std::string native_checked_text(std::shared_ptr<async_state>, bool)
    __objc3_async __objc3_throws(id<NativeFirstError>);
async_checked_complex native_checked_complex(std::shared_ptr<async_state>, bool)
    __objc3_async __objc3_throws(id<NativeFirstError>);
async_checked_data native_checked_data(std::shared_ptr<async_state>, bool)
    __objc3_async __objc3_throws(id<NativeFirstError>);
async_checked_method native_checked_method(std::shared_ptr<async_state>, bool)
    __objc3_async __objc3_throws(id<NativeFirstError>);
int native_checked_values(std::shared_ptr<async_state>, unsigned) __objc3_async;
int native_checked_value_optional(std::shared_ptr<async_state>, bool) __objc3_async;
int native_checked_value_discard(std::shared_ptr<async_state>, bool) __objc3_async;
inline async_checked_value native_checked_value_inline(std::shared_ptr<async_state> state)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  return __objc3_try(__objc3_await native_checked_value_relay(std::move(state), false));
}

inline int native_checked_inline(std::shared_ptr<async_state> state)
    __objc3_async __objc3_throws(id<NativeFirstError>) {
  return __objc3_try(__objc3_await native_checked_parent(std::move(state), false));
}

inline int native_inline(std::shared_ptr<async_state> state,
                         int value) __objc3_async {
  return __objc3_await native_leaf(std::move(state), value);
}
template <class T> T native_identity(T value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value;
}

// The implicit receiver is borrowed, as for a native C++ coroutine. The
// consumer keeps it alive until every member task has finished or cancelled.
struct native_async_prefix {
  virtual ~native_async_prefix() = default;
  long padding = 1;
};
struct native_async_base {
  virtual ~native_async_base() = default;
  virtual int value(std::shared_ptr<async_state>, int) const __objc3_async = 0;
  virtual int checked(std::shared_ptr<async_state>, bool) const
      __objc3_async __objc3_throws(id<NativeFirstError>) = 0;
};
struct native_async_worker : native_async_prefix, native_async_base {
  int bias = 40;
  int value(std::shared_ptr<async_state>, int) const __objc3_async override;
  int checked(std::shared_ptr<async_state>, bool) const
      __objc3_async __objc3_throws(id<NativeFirstError>) override;
  static int constant(int) __objc3_async;
  int operator()(int) const __objc3_async;
  int operator[](int) const __objc3_async;
  int operator-() const __objc3_async;
  int qualified() & __objc3_async;
  int qualified() && __objc3_async;
  int hopped(objc3_actor_t *, std::shared_ptr<async_state>) const __objc3_async;
  template <class T> T echo(T value) const __objc3_async {
    __objc3_await objc3::task_yield;
    return value;
  }
#if __cplusplus > 202002L
  int copied(this native_async_worker self, int value) __objc3_async {
    __objc3_await objc3::task_yield;
    return self.bias + value;
  }
#endif
};
using native_member_callback = int (native_async_worker::*)(
    std::shared_ptr<async_state>, int) const __objc3_async;
using native_checked_member_callback = int (native_async_base::*)(
    std::shared_ptr<async_state>, bool) const
    __objc3_async __objc3_throws(id<NativeFirstError>);
int operator+(const native_async_worker &, int) __objc3_async;
int native_member_apply(const native_async_worker &, native_member_callback,
                        std::shared_ptr<async_state>, int) __objc3_async;

#ifdef __OBJC__
@protocol NativeAsyncMethodProtocol
- (int)value:(std::shared_ptr<async_state>)state mode:(int)mode __objc3_async;
- (int)checked:(std::shared_ptr<async_state>)state fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeFirstError>);
@end
__attribute__((objc_root_class))
@interface NativeAsyncMethod <NativeAsyncMethodProtocol> {
  Class isa;
@public
  int bias;
}
+ (id)new;
+ (int)constant:(int)value __objc3_async;
- (int)value:(std::shared_ptr<async_state>)state mode:(int)mode __objc3_async;
- (int)checked:(std::shared_ptr<async_state>)state fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeFirstError>);
- (int)recover:(std::shared_ptr<async_state>)state fail:(bool)fail __objc3_async;
- (async_checked_value)record:(std::shared_ptr<async_state>)state fail:(bool)fail
    __objc3_async __objc3_throws(id<NativeFirstError>);
- (NativeAsyncResult *)object:(int)value __objc3_async;
- (native_result_reader)reader:(int)value __objc3_async;
- (int)direct:(int)value __objc3_async __attribute__((objc_direct));
- (void)empty __objc3_async;
- (int)hopped:(objc3_actor_t *)actor state:(std::shared_ptr<async_state>)state __objc3_async;
@end
@interface NativeAsyncMethodChild : NativeAsyncMethod
- (int)value:(std::shared_ptr<async_state>)state mode:(int)mode __objc3_async;
@end
unsigned native_method_destructions();
int native_method_consume(id<NativeAsyncMethodProtocol>, std::shared_ptr<async_state>, bool)
    __objc3_async;
#endif

#ifdef __OBJC__
using native_async_block = int (^)(int) __objc3_async;
objc3::task<int> native_block_lazy(std::shared_ptr<async_state>, int = 0);
objc3::task<async_checked_value> native_block_checked(std::shared_ptr<async_state>, bool);
int native_block_recover(std::shared_ptr<async_state>, bool) __objc3_async;
native_async_block native_block_factory(int) __objc3_async;
objc3::task<int> native_block_hopped(objc3_actor_t *, std::shared_ptr<async_state>);
bool native_block_copy_failure(std::shared_ptr<async_state>);
objc3::task<int> native_block_noescape(std::shared_ptr<async_state>, bool);
#endif

#endif // NATIVE_ASYNC_PROVIDER_H
