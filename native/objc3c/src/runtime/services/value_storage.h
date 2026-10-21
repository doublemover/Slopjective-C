#ifndef OBJC3_RUNTIME_SERVICE_VALUE_STORAGE_H
#define OBJC3_RUNTIME_SERVICE_VALUE_STORAGE_H

#include "runtime/public/objc3_runtime_services.h"

#include <algorithm>
#include <cstddef>
#include <new>
#include <utility>

namespace objc3::services {

// Owns exactly one initialized payload. The descriptor is copied, so a
// compiler-generated descriptor need not remain in its caller's stack frame.
class ValueStorage final {
 public:
  ValueStorage() = default;
  ValueStorage(const ValueStorage &) = delete;
  ValueStorage &operator=(const ValueStorage &) = delete;
  ValueStorage(ValueStorage &&other) noexcept { Swap(other); }
  ValueStorage &operator=(ValueStorage &&other) noexcept {
    if (this != &other) {
      Reset();
      Swap(other);
    }
    return *this;
  }
  ~ValueStorage() { Reset(); }

  objc3_service_status Copy(const void *source, const objc3_value_ops *ops) {
    auto status = objc3_value_ops_validate(ops);
    if (status != OBJC3_SERVICE_OK) return status;
    if (ops->payload_size && !source) return OBJC3_SERVICE_INVALID_ARGUMENT;
    // Initialize a new allocation before replacing a live value. Aliasing
    // source with our current payload is therefore safe.
    ValueStorage candidate;
    candidate.ops_ = *ops;
    candidate.alignment_ = std::max<size_t>(alignof(std::max_align_t),
                                         static_cast<size_t>(ops->payload_alignment));
    candidate.data_ = ::operator new(std::max<size_t>(1, ops->payload_size),
                                    std::align_val_t(candidate.alignment_),
                                    std::nothrow);
    if (!candidate.data_) return OBJC3_SERVICE_OUT_OF_MEMORY;
    status = ops->copy_init(candidate.data_, source);
    if (status != OBJC3_SERVICE_OK) return status;
    candidate.initialized_ = true;
    Swap(candidate);
    return OBJC3_SERVICE_OK;
  }

  void Reset() noexcept {
    if (!data_) return;
    void *data = std::exchange(data_, nullptr);
    bool initialized = std::exchange(initialized_, false);
    // Clear state before user code runs, allowing safe destruction reentry.
    if (initialized) ops_.destroy(data);
    ::operator delete(data, std::align_val_t(alignment_));
  }
  const void *data() const { return initialized_ ? data_ : nullptr; }
  const objc3_value_ops *ops() const { return initialized_ ? &ops_ : nullptr; }
  bool initialized() const { return initialized_; }

 private:
  void Swap(ValueStorage &other) noexcept {
    std::swap(data_, other.data_);
    std::swap(ops_, other.ops_);
    std::swap(alignment_, other.alignment_);
    std::swap(initialized_, other.initialized_);
  }
  void *data_ = nullptr;
  objc3_value_ops ops_{};
  size_t alignment_ = alignof(std::max_align_t);
  bool initialized_ = false;
};
}  // namespace objc3::services
#endif
