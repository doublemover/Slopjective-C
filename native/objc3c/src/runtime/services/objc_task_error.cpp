#include "runtime/public/objc3_runtime_services.h"

#include <cstring>

// Keep this in a separate archive member: ordinary C++ task clients need no
// Objective-C runtime dependency. The reserved compiler bridge consumes +1.
extern "C" void *objc3_error_object_retain(void *) __asm__("objc_retain");
extern "C" void objc3_error_object_release(void *) __asm__("objc_release");
extern "C" void *objc3_result_block_copy(const void *) __asm__("_Block_copy");
extern "C" void
objc3_result_block_release(const void *) __asm__("_Block_release");

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

objc3_service_status copy_block(void *destination,
                                const void *source) noexcept {
  void *value;
  std::memcpy(&value, source, sizeof(value));
  void *copy = nullptr;
  try {
    if (value)
      copy = objc3_result_block_copy(value);
  } catch (...) {
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  if (value && !copy)
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  std::memcpy(destination, &copy, sizeof(copy));
  return OBJC3_SERVICE_OK;
}
void destroy_block(void *storage) noexcept {
  void *value;
  std::memcpy(&value, storage, sizeof(value));
  objc3_result_block_release(value);
}
const objc3_value_ops block_ops{sizeof(objc3_value_ops),
                                OBJC3_VALUE_OPS_VERSION,
                                sizeof(void *),
                                alignof(void *),
                                copy_block,
                                move_object,
                                destroy_block};
} // namespace

extern "C" const objc3_value_ops *
objc3_async_reference_ops(objc3_async_result_kind kind) {
  if (kind == OBJC3_ASYNC_OBJECT)
    return &object_ops;
  if (kind == OBJC3_ASYNC_BLOCK)
    return &block_ops;
  return nullptr;
}

extern "C" objc3_error_t *__objc3_task_error_create(void *owned) noexcept {
  objc3_error_t *error = nullptr;
  const auto status = owned ? objc3_error_create(&owned, &object_ops, &error)
                            : OBJC3_SERVICE_INVALID_ARGUMENT;
  objc3_error_object_release(owned);
  // The bridge consumes its native +1 even when boxing cannot allocate. Keep
  // failure distinct from genuine cancellation using allocation-free transport.
  if (status != OBJC3_SERVICE_OK &&
      objc3_async_error_create_service(status, &error) != OBJC3_SERVICE_OK)
    __builtin_trap();
  if (!error) __builtin_trap();
  return error;
}

extern "C" void *
__objc3_task_error_object(const objc3_error_t *error) noexcept {
  const auto *ops = objc3_error_payload_ops(error);
  if (!ops || ops->copy_init != copy_object)
    return nullptr;
  return *static_cast<void *const *>(objc3_error_payload(error));
}
