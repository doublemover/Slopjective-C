#include "runtime/services/carrier_service.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <new>
#include <utility>

namespace {
class Payload final {
 public:
  Payload() = default;
  Payload(const Payload &) = delete;
  Payload &operator=(const Payload &) = delete;
  ~Payload() { Reset(); }

  objc3_service_status Copy(const void *source, const objc3_value_ops *ops) {
    return Initialize(source, nullptr, ops, false);
  }
  objc3_service_status Move(void *source, const objc3_value_ops *ops) {
    return Initialize(nullptr, source, ops, true);
  }
  void Reset() noexcept {
    void *data = std::exchange(data_, nullptr);
    const bool initialized = std::exchange(initialized_, false);
    const auto alignment = alignment_;
    if (!data) return;
    if (initialized) ops_.destroy(data);
    ::operator delete(data, std::align_val_t(alignment));
    ops_ = {};
    alignment_ = alignof(std::max_align_t);
  }
  void ReleaseMovedSource() noexcept {
    void *data = std::exchange(data_, nullptr);
    const auto alignment = alignment_;
    initialized_ = false;
    ops_ = {};
    alignment_ = alignof(std::max_align_t);
    if (data) ::operator delete(data, std::align_val_t(alignment));
  }
  void Swap(Payload &other) noexcept {
    std::swap(data_, other.data_);
    std::swap(ops_, other.ops_);
    std::swap(alignment_, other.alignment_);
    std::swap(initialized_, other.initialized_);
  }
  bool initialized() const { return initialized_; }
  const void *data() const { return initialized_ ? data_ : nullptr; }
  const objc3_value_ops *ops() const { return initialized_ ? &ops_ : nullptr; }

 private:
  objc3_service_status Initialize(const void *copy_source, void *move_source,
                                 const objc3_value_ops *ops, bool moving) {
    auto status = objc3_value_ops_validate(ops);
    if (status != OBJC3_SERVICE_OK) return status;
    const void *source = moving ? move_source : copy_source;
    if (ops->payload_size && !source) return OBJC3_SERVICE_INVALID_ARGUMENT;
    Payload candidate;
    candidate.ops_ = *ops;
    candidate.alignment_ = std::max<size_t>(
        alignof(std::max_align_t), static_cast<size_t>(ops->payload_alignment));
    candidate.data_ = ::operator new(
        std::max<size_t>(1, static_cast<size_t>(ops->payload_size)),
        std::align_val_t(candidate.alignment_), std::nothrow);
    if (!candidate.data_) return OBJC3_SERVICE_OUT_OF_MEMORY;
    status = moving ? ops->move_init(candidate.data_, move_source)
                    : ops->copy_init(candidate.data_, copy_source);
    if (status != OBJC3_SERVICE_OK) return status;
    candidate.initialized_ = true;
    Swap(candidate);
    return OBJC3_SERVICE_OK;
  }

  void *data_ = nullptr;
  objc3_value_ops ops_{};
  size_t alignment_ = alignof(std::max_align_t);
  bool initialized_ = false;
};

enum class CarrierKind { Optional, Result };
struct Carrier {
  CarrierKind kind;
  uint32_t tag = 0;
  bool has_payload = false;
  Payload payload;
  explicit Carrier(CarrierKind carrier_kind) : kind(carrier_kind) {}
  bool initialized() const { return tag != 0; }
  void Clear() {
    // Publish the uninitialized state before user destruction code executes.
    tag = 0;
    has_payload = false;
    payload.Reset();
  }
  void ClearMovedSource() {
    tag = 0;
    has_payload = false;
    payload.ReleaseMovedSource();
  }
  void Commit(uint32_t new_tag, bool new_has_payload, Payload &candidate) {
    Payload previous;
    previous.Swap(payload);
    payload.Swap(candidate);
    has_payload = new_has_payload;
    tag = new_tag;
  }
};

objc3_service_status CheckCarrier(const Carrier *carrier, CarrierKind kind) {
  return carrier && carrier->kind == kind ? OBJC3_SERVICE_OK
                                           : OBJC3_SERVICE_INVALID_ARGUMENT;
}
objc3_service_status InitVoid(Carrier *carrier, CarrierKind kind,
                              uint32_t tag) {
  auto status = CheckCarrier(carrier, kind);
  if (status != OBJC3_SERVICE_OK) return status;
  if (carrier->initialized()) return OBJC3_SERVICE_INVALID_ARGUMENT;
  carrier->tag = tag;
  carrier->has_payload = false;
  return OBJC3_SERVICE_OK;
}
objc3_service_status InitPayload(Carrier *carrier, CarrierKind kind,
                                 uint32_t tag, const void *copy_source,
                                 void *move_source,
                                 const objc3_value_ops *ops, bool moving) {
  auto status = CheckCarrier(carrier, kind);
  if (status != OBJC3_SERVICE_OK) return status;
  if (carrier->initialized()) return OBJC3_SERVICE_INVALID_ARGUMENT;
  Payload candidate;
  status = moving ? candidate.Move(move_source, ops)
                  : candidate.Copy(copy_source, ops);
  if (status != OBJC3_SERVICE_OK) return status;
  carrier->Commit(tag, true, candidate);
  return OBJC3_SERVICE_OK;
}
objc3_service_status ReadView(const Carrier *carrier, CarrierKind kind,
                              objc3_carrier_payload_view *out_view) {
  auto status = CheckCarrier(carrier, kind);
  if (status != OBJC3_SERVICE_OK || !out_view ||
      out_view->struct_size < sizeof(objc3_carrier_payload_view))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (!carrier->initialized()) return OBJC3_SERVICE_NOT_READY;
  out_view->version = OBJC3_VALUE_OPS_VERSION;
  out_view->has_payload = carrier->has_payload ? 1u : 0u;
  out_view->reserved = 0;
  out_view->payload = carrier->has_payload ? carrier->payload.data() : nullptr;
  out_view->payload_ops = carrier->has_payload ? carrier->payload.ops() : nullptr;
  return OBJC3_SERVICE_OK;
}
objc3_service_status CloneInit(Carrier *destination, const Carrier *source,
                               CarrierKind kind, bool moving) {
  auto status = CheckCarrier(destination, kind);
  if (status != OBJC3_SERVICE_OK ||
      CheckCarrier(source, kind) != OBJC3_SERVICE_OK)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (destination == source || destination->initialized() ||
      !source->initialized())
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  Payload candidate;
  if (source->has_payload) {
    status = moving
                 ? candidate.Move(const_cast<void *>(source->payload.data()),
                                  source->payload.ops())
                 : candidate.Copy(source->payload.data(), source->payload.ops());
    if (status != OBJC3_SERVICE_OK) return status;
  }
  const uint32_t tag = source->tag;
  const bool has_payload = source->has_payload;
  if (moving) const_cast<Carrier *>(source)->ClearMovedSource();
  destination->Commit(tag, has_payload, candidate);
  return OBJC3_SERVICE_OK;
}
objc3_service_status Assign(Carrier *destination, Carrier *source,
                            CarrierKind kind, bool moving) {
  auto status = CheckCarrier(destination, kind);
  if (status != OBJC3_SERVICE_OK || CheckCarrier(source, kind) != OBJC3_SERVICE_OK)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (destination == source) return OBJC3_SERVICE_OK;
  if (!source->initialized()) return OBJC3_SERVICE_NOT_READY;
  Payload candidate;
  if (source->has_payload) {
    status = moving
                 ? candidate.Move(const_cast<void *>(source->payload.data()), source->payload.ops())
                 : candidate.Copy(source->payload.data(), source->payload.ops());
    if (status != OBJC3_SERVICE_OK) return status;
  }
  const uint32_t tag = source->tag;
  const bool has_payload = source->has_payload;
  if (moving) source->ClearMovedSource();
  destination->Commit(tag, has_payload, candidate);
  return OBJC3_SERVICE_OK;
}
}  // namespace

struct objc3_optional_value { Carrier value{CarrierKind::Optional}; };
struct objc3_result_value { Carrier value{CarrierKind::Result}; };

extern "C" objc3_service_status objc3_optional_value_create(
    objc3_optional_value_t **out_value) {
  if (!out_value) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_value = new (std::nothrow) objc3_optional_value;
  return *out_value ? OBJC3_SERVICE_OK : OBJC3_SERVICE_OUT_OF_MEMORY;
}
extern "C" void objc3_optional_value_destroy(objc3_optional_value_t *value) {
  delete value;
}
extern "C" objc3_optional_tag objc3_optional_value_get_tag(
    const objc3_optional_value_t *value) {
  return value ? static_cast<objc3_optional_tag>(value->value.tag)
               : OBJC3_OPTIONAL_UNINITIALIZED;
}
extern "C" objc3_service_status objc3_optional_value_get_payload(
    const objc3_optional_value_t *value, objc3_carrier_payload_view *out_view) {
  return value ? ReadView(&value->value, CarrierKind::Optional, out_view)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_optional_none_init(
    objc3_optional_value_t *value) {
  return value ? InitVoid(&value->value, CarrierKind::Optional,
                          OBJC3_OPTIONAL_NONE)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_optional_some_copy_init(
    objc3_optional_value_t *value, const void *payload,
    const objc3_value_ops *ops) {
  return value ? InitPayload(&value->value, CarrierKind::Optional,
                             OBJC3_OPTIONAL_SOME, payload, nullptr, ops, false)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_optional_some_move_init(
    objc3_optional_value_t *value, void *payload,
    const objc3_value_ops *ops) {
  return value ? InitPayload(&value->value, CarrierKind::Optional,
                             OBJC3_OPTIONAL_SOME, nullptr, payload, ops, true)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_optional_copy_init(
    objc3_optional_value_t *destination, const objc3_optional_value_t *source) {
  return CloneInit(destination ? &destination->value : nullptr,
                   source ? &source->value : nullptr, CarrierKind::Optional,
                   false);
}
extern "C" objc3_service_status objc3_optional_move_init(
    objc3_optional_value_t *destination, objc3_optional_value_t *source) {
  return CloneInit(destination ? &destination->value : nullptr,
                   source ? &source->value : nullptr, CarrierKind::Optional,
                   true);
}
extern "C" objc3_service_status objc3_optional_copy_assign(
    objc3_optional_value_t *destination, const objc3_optional_value_t *source) {
  return Assign(destination ? &destination->value : nullptr,
                const_cast<Carrier *>(source ? &source->value : nullptr),
                CarrierKind::Optional, false);
}
extern "C" objc3_service_status objc3_optional_move_assign(
    objc3_optional_value_t *destination, objc3_optional_value_t *source) {
  return Assign(destination ? &destination->value : nullptr,
                source ? &source->value : nullptr, CarrierKind::Optional, true);
}

extern "C" objc3_service_status objc3_result_value_create(
    objc3_result_value_t **out_value) {
  if (!out_value) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_value = new (std::nothrow) objc3_result_value;
  return *out_value ? OBJC3_SERVICE_OK : OBJC3_SERVICE_OUT_OF_MEMORY;
}
extern "C" void objc3_result_value_destroy(objc3_result_value_t *value) {
  delete value;
}
extern "C" objc3_result_tag objc3_result_value_get_tag(
    const objc3_result_value_t *value) {
  return value ? static_cast<objc3_result_tag>(value->value.tag)
               : OBJC3_RESULT_UNINITIALIZED;
}
extern "C" objc3_service_status objc3_result_value_get_payload(
    const objc3_result_value_t *value, objc3_carrier_payload_view *out_view) {
  return value ? ReadView(&value->value, CarrierKind::Result, out_view)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_result_success_copy_init(
    objc3_result_value_t *value, const void *payload,
    const objc3_value_ops *ops) {
  return value ? InitPayload(&value->value, CarrierKind::Result,
                             OBJC3_RESULT_SUCCESS, payload, nullptr, ops, false)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_result_success_move_init(
    objc3_result_value_t *value, void *payload,
    const objc3_value_ops *ops) {
  return value ? InitPayload(&value->value, CarrierKind::Result,
                             OBJC3_RESULT_SUCCESS, nullptr, payload, ops, true)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_result_error_copy_init(
    objc3_result_value_t *value, const void *payload,
    const objc3_value_ops *ops) {
  return value ? InitPayload(&value->value, CarrierKind::Result,
                             OBJC3_RESULT_ERROR, payload, nullptr, ops, false)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_result_error_move_init(
    objc3_result_value_t *value, void *payload,
    const objc3_value_ops *ops) {
  return value ? InitPayload(&value->value, CarrierKind::Result,
                             OBJC3_RESULT_ERROR, nullptr, payload, ops, true)
               : OBJC3_SERVICE_INVALID_ARGUMENT;
}
extern "C" objc3_service_status objc3_result_copy_init(
    objc3_result_value_t *destination, const objc3_result_value_t *source) {
  return CloneInit(destination ? &destination->value : nullptr,
                   source ? &source->value : nullptr, CarrierKind::Result, false);
}
extern "C" objc3_service_status objc3_result_move_init(
    objc3_result_value_t *destination, objc3_result_value_t *source) {
  return CloneInit(destination ? &destination->value : nullptr,
                   source ? &source->value : nullptr, CarrierKind::Result, true);
}
extern "C" objc3_service_status objc3_result_copy_assign(
    objc3_result_value_t *destination, const objc3_result_value_t *source) {
  return Assign(destination ? &destination->value : nullptr,
                const_cast<Carrier *>(source ? &source->value : nullptr),
                CarrierKind::Result, false);
}
extern "C" objc3_service_status objc3_result_move_assign(
    objc3_result_value_t *destination, objc3_result_value_t *source) {
  return Assign(destination ? &destination->value : nullptr,
                source ? &source->value : nullptr, CarrierKind::Result, true);
}
