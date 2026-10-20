#ifndef NATIVE_ASYNC_PROVIDER_H
#define NATIVE_ASYNC_PROVIDER_H
#include "objc3_task.hpp"
#include <atomic>
#include <memory>
#include <string>

struct async_state {
  std::atomic<int> entered{0}, destroyed{0}, resumed{0}, parents{0},
      deferred{0};
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
int native_exception_number(__unsafe_unretained id);
unsigned native_object_destructions();
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

inline int native_inline(std::shared_ptr<async_state> state,
                         int value) __objc3_async {
  return __objc3_await native_leaf(std::move(state), value);
}
template <class T> T native_identity(T value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value;
}
#endif
