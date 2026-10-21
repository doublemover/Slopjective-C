#include "runtime/public/objc3_runtime_services.h"

// Keep this in a separate archive member: ordinary C++ task clients need no
// Objective-C runtime dependency. The reserved compiler bridge consumes +1.
extern "C" void *objc3_error_object_retain(void *) __asm__("objc_retain");
extern "C" void objc3_error_object_release(void *) __asm__("objc_release");

namespace {
objc3_service_status copy_object(void *destination, const void *source) noexcept {
  *static_cast<void **>(destination) =
      objc3_error_object_retain(*static_cast<void *const *>(source));
  return OBJC3_SERVICE_OK;
}
void destroy_object(void *storage) noexcept {
  objc3_error_object_release(*static_cast<void **>(storage));
}
objc3_service_status move_object(void *destination, void *source) noexcept {
  *static_cast<void **>(destination) = *static_cast<void **>(source);
  *static_cast<void **>(source) = nullptr;
  return OBJC3_SERVICE_OK;
}
const objc3_value_ops object_ops{sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
                               sizeof(void *), alignof(void *), copy_object,
                               move_object, destroy_object};
} // namespace

extern "C" objc3_error_t *__objc3_task_error_create(void *owned) noexcept {
  objc3_error_t *error = nullptr;
  if (owned)
    (void)objc3_error_create(&owned, &object_ops, &error);
  objc3_error_object_release(owned);
  return error;
}

extern "C" void *
__objc3_task_error_object(const objc3_error_t *error) noexcept {
  const auto *ops = objc3_error_payload_ops(error);
  if (!ops || ops->copy_init != copy_object)
    return nullptr;
  return *static_cast<void *const *>(objc3_error_payload(error));
}
