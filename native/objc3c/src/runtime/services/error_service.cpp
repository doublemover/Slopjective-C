#include "runtime/public/objc3_runtime_services.h"
#include "runtime/services/value_storage.h"

#include <atomic>
#include <limits>
#include <new>

struct objc3_error {
  std::atomic<size_t> references{1};
  objc3::services::ValueStorage payload;
};

extern "C" objc3_service_status objc3_value_ops_validate(
    const objc3_value_ops *ops) {
  if (!ops || ops->struct_size < sizeof(objc3_value_ops))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (ops->version != OBJC3_VALUE_OPS_VERSION)
    return OBJC3_SERVICE_UNSUPPORTED_VERSION;
  const auto alignment = ops->payload_alignment;
  if (!alignment || (alignment & (alignment - 1)) ||
      alignment > std::numeric_limits<size_t>::max() ||
      ops->payload_size > std::numeric_limits<size_t>::max() ||
      !ops->copy_init || !ops->move_init || !ops->destroy)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status objc3_error_create(
    const void *payload, const objc3_value_ops *ops, objc3_error_t **out_error) {
  if (!out_error) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_error = nullptr;
  auto status = objc3_value_ops_validate(ops);
  if (status != OBJC3_SERVICE_OK) return status;
  if (ops->payload_size && !payload) return OBJC3_SERVICE_INVALID_ARGUMENT;
  auto *error = new (std::nothrow) objc3_error;
  if (!error) return OBJC3_SERVICE_OUT_OF_MEMORY;
  status = error->payload.Copy(payload, ops);
  if (status != OBJC3_SERVICE_OK) {
    delete error;
    return status;
  }
  *out_error = error;
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_error_t *objc3_error_retain(objc3_error_t *error) {
  if (!error) return nullptr;
  auto count = error->references.load(std::memory_order_relaxed);
  do {
    if (count == std::numeric_limits<size_t>::max()) return nullptr;
  } while (!error->references.compare_exchange_weak(
      count, count + 1, std::memory_order_relaxed));
  return error;
}

extern "C" void objc3_error_release(objc3_error_t *error) {
  if (error && error->references.fetch_sub(1, std::memory_order_acq_rel) == 1)
    delete error;
}
extern "C" const void *objc3_error_payload(const objc3_error_t *error) {
  return error ? error->payload.data() : nullptr;
}
extern "C" const objc3_value_ops *objc3_error_payload_ops(
    const objc3_error_t *error) {
  return error ? error->payload.ops() : nullptr;
}
