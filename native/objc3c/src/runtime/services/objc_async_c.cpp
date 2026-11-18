#include "runtime/public/objc3_async_c.h"
#include "runtime/concurrency/service_autorelease_pool.h"

#include <cstdlib>
#include <cstring>

extern "C" void *_Block_copy(const void *);
extern "C" void _Block_release(const void *);
extern "C" void *__objc3_task_error_object(const objc3_error_t *) noexcept;

@protocol Error
@end

// Class objects are process-lifetime Objective-C objects. This keeps both
// distinguished errors available even when task startup fails to allocate.
__attribute__((objc_root_class))
@interface ObjC3AsyncCompletionError <Error>
+ (id)self;
+ (id)retain;
+ (oneway void)release;
+ (id)autorelease;
@end

@implementation ObjC3AsyncCompletionError
+ (id)self { return self; }
+ (id)retain { return self; }
+ (oneway void)release {}
+ (id)autorelease { return self; }
@end

@interface ObjC3CancellationError : ObjC3AsyncCompletionError <Error>
@end

@implementation ObjC3CancellationError
@end

namespace {
objc3_async_c_error cancellation_error() {
  return (objc3_async_c_error)[ObjC3CancellationError self];
}

void complete(const objc3_completion *completion, bool checked,
              __objc3_async_c_delivery delivery) noexcept {
  objc3::services::CallbackAutoreleasePool pool;
  try {
    if (completion->kind != OBJC3_COMPLETION_ERROR) {
      delivery(completion);
      return;
    }
    if (checked && __objc3_task_error_object(completion->error) !=
                       (void *)cancellation_error()) {
      delivery(completion);
      return;
    }
    if (!checked) {
      // Infrastructure failure ends nonthrowing work without a language error.
      // A native checked error here would mean the export has a false effect.
      objc3_service_status service_status;
      if (objc3_async_error_get_service(completion->error, &service_status) !=
          OBJC3_SERVICE_OK)
        std::abort();
    }
    // A checked propagation of the cancellation object is still cancellation
    // at the C boundary, including when the child used an ordinary error slot.
    const objc3_completion cancelled{sizeof(objc3_completion),
        OBJC3_SERVICE_ABI_VERSION, OBJC3_COMPLETION_CANCELLED, 0,
        nullptr, nullptr, nullptr, nullptr};
    delivery(&cancelled);
  } catch (...) {
    // Native completion callbacks must not unwind across the C ABI.
  }
}

template<bool Checked>
void deliver(void *context, const objc3_completion *completion) noexcept {
  complete(completion, Checked, (__objc3_async_c_delivery)context);
  _Block_release(context);
}
} // namespace

extern "C" int objc3_async_c_error_is_cancellation(objc3_async_c_error error) {
  return error == cancellation_error();
}

extern "C" objc3_async_c_error objc3_task_current_cancellation_error(void) {
  return objc3_task_current_cancel_requested() ? cancellation_error() : nullptr;
}

extern "C" void *__objc3_task_cancellation_error_object() noexcept {
  return (void *)cancellation_error();
}

extern "C" objc3_async_c_error
__objc3_async_c_completion_error(const objc3_completion *completion) {
  switch (completion->kind) {
  case OBJC3_COMPLETION_SUCCESS:
    return nullptr;
  case OBJC3_COMPLETION_CANCELLED:
    return cancellation_error();
  case OBJC3_COMPLETION_ERROR:
    if (void *object = __objc3_task_error_object(completion->error))
      return (objc3_async_c_error)object;
    return (objc3_async_c_error)[ObjC3AsyncCompletionError self];
  }
  std::abort();
}

extern "C" void __objc3_async_c_submit(
    objc3_async_frame *frame, objc3_executor_t *executor, objc3_actor_t *actor,
    int checked, __objc3_async_c_delivery delivery) {
  if (!delivery) std::abort();
  void *owned = _Block_copy((const void *)delivery);
  const auto started = owned
      ? objc3_async_frame_submit(frame, executor, actor,
                                checked ? deliver<true> : deliver<false>, owned)
      : OBJC3_SERVICE_OUT_OF_MEMORY;
  if (started == OBJC3_SERVICE_OK) return;
  if (owned) _Block_release(owned);
  if (objc3_async_frame_dispose(frame) != OBJC3_SERVICE_OK) std::abort();
  objc3_error_t *error = nullptr;
  if (objc3_async_error_create_service(started, &error) != OBJC3_SERVICE_OK)
    std::abort();
  const objc3_completion failure{sizeof(objc3_completion),
      OBJC3_SERVICE_ABI_VERSION, OBJC3_COMPLETION_ERROR, 0,
      nullptr, nullptr, error, nullptr};
  complete(&failure, checked, delivery);
  objc3_error_release(error);
}

extern "C" void __objc3_async_c_check_result(
    const objc3_async_frame *frame, objc3_async_result_kind kind,
    const char *identity, size_t size, size_t alignment) {
  if (objc3_async_frame_validate(frame) != OBJC3_SERVICE_OK) std::abort();
  const auto &type = *frame->result_type;
  if (type.kind != kind || type.storage_size != size ||
      type.storage_alignment != alignment || type.identity_domain ||
      type.identity_size != std::strlen(identity) ||
      std::memcmp(type.identity, identity, type.identity_size))
    std::abort();
}
