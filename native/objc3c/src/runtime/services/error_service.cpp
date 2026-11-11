#include "runtime/public/objc3_runtime_services.h"
#include "runtime/services/value_storage.h"

#include <atomic>
#include <limits>
#include <new>

struct objc3_error {
  std::atomic<size_t> references{1};
  objc3::services::ValueStorage payload;
  const bool process_owned{false};
  const objc3_service_status service_status{OBJC3_SERVICE_OK};

  objc3_error() = default;
  explicit objc3_error(objc3_service_status status) noexcept
      : process_owned(true), service_status(status) {}
};

namespace {
objc3_service_status copy_service_status(void *destination,
                                         const void *source) noexcept {
  *static_cast<objc3_service_status *>(destination) =
      *static_cast<const objc3_service_status *>(source);
  return OBJC3_SERVICE_OK;
}
objc3_service_status move_service_status(void *destination,
                                         void *source) noexcept {
  return copy_service_status(destination, source);
}
void destroy_service_status(void *) noexcept {}
const objc3_value_ops service_status_ops{
    sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
    sizeof(objc3_service_status), alignof(objc3_service_status),
    copy_service_status, move_service_status, destroy_service_status};

struct ServiceErrors final {
  static constexpr size_t count = OBJC3_SERVICE_TYPE_MISMATCH + 1;
  alignas(objc3_error) unsigned char storage[count][sizeof(objc3_error)];
  objc3_error_t *values[count]{};

  ServiceErrors() noexcept {
    for (size_t index = 0; index != count; ++index)
      values[index] = new (storage[index])
          objc3_error(static_cast<objc3_service_status>(index));
  }
  // Raw storage has a trivial destructor. Do not register atexit destruction
  // of immutable errors which can remain owned by process-lifetime workers.
};

objc3_error_t *service_error(objc3_service_status status) noexcept {
  static ServiceErrors errors;
  return errors.values[static_cast<size_t>(status)];
}
} // namespace

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
  // An immutable service error's process owner makes every shared lease valid
  // without allocation or reference-count saturation, including reporting OOM.
  if (error->process_owned) return error;
  auto count = error->references.load(std::memory_order_relaxed);
  do {
    if (count == std::numeric_limits<size_t>::max()) return nullptr;
  } while (!error->references.compare_exchange_weak(
      count, count + 1, std::memory_order_relaxed));
  return error;
}

extern "C" void objc3_error_release(objc3_error_t *error) {
  if (error && error->process_owned) return;
  if (error && error->references.fetch_sub(1, std::memory_order_acq_rel) == 1)
    delete error;
}
extern "C" const void *objc3_error_payload(const objc3_error_t *error) {
  return error ? error->process_owned ? &error->service_status
                                     : error->payload.data() : nullptr;
}
extern "C" const objc3_value_ops *objc3_error_payload_ops(
    const objc3_error_t *error) {
  return error ? error->process_owned ? &service_status_ops
                                     : error->payload.ops() : nullptr;
}

extern "C" objc3_service_status objc3_async_error_create_service(
    objc3_service_status status, objc3_error_t **out_error) {
  if (!out_error) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_error = nullptr;
  if (status <= OBJC3_SERVICE_OK || status > OBJC3_SERVICE_TYPE_MISMATCH ||
      status == OBJC3_SERVICE_CANCELLED)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_error = service_error(status);
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status objc3_async_error_get_service(
    const objc3_error_t *error, objc3_service_status *out_status) {
  if (!error || !out_status) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!error->process_owned) return OBJC3_SERVICE_TYPE_MISMATCH;
  *out_status = error->service_status;
  return OBJC3_SERVICE_OK;
}
