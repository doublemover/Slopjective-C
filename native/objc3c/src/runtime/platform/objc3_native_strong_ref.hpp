#ifndef OBJC3_NATIVE_STRONG_REF_HPP
#define OBJC3_NATIVE_STRONG_REF_HPP

#include "runtime/platform/objc3_native_object_adapter.h"

#include <utility>

namespace objc3::runtime::platform {

struct AdoptRetainedReference final {};
inline constexpr AdoptRetainedReference adopt_retained_reference{};

/* Small C++ cleanup bridge for extension aggregates and ObjC++ fixtures. */
class StrongRef final {
public:
  StrongRef() noexcept = default;
  explicit StrongRef(objc3_native_object_t object) noexcept
      : object_(objc3_native_object_retain(object)) {}
  StrongRef(AdoptRetainedReference, objc3_native_object_t object) noexcept
      : object_(object) {}
  StrongRef(const StrongRef &other) noexcept
      : object_(objc3_native_object_retain(other.object_)) {}
  StrongRef(StrongRef &&other) noexcept
      : object_(std::exchange(other.object_, nullptr)) {}

  StrongRef &operator=(const StrongRef &other) noexcept {
    reset(other.object_); // retain replacement before releasing old value
    return *this;
  }
  StrongRef &operator=(StrongRef &&other) noexcept {
    if (this != &other) {
      objc3_native_object_release(object_);
      object_ = std::exchange(other.object_, nullptr);
    }
    return *this;
  }
  ~StrongRef() { objc3_native_object_release(object_); }

  objc3_native_object_t get() const noexcept { return object_; }
  objc3_native_object_t detach() noexcept {
    return std::exchange(object_, nullptr);
  }
  void reset(objc3_native_object_t object = nullptr) noexcept {
    objc3_native_object_t replacement = objc3_native_object_retain(object);
    objc3_native_object_t prior = std::exchange(object_, replacement);
    objc3_native_object_release(prior);
  }

private:
  objc3_native_object_t object_ = nullptr;
};

/*
 * Weak slots are runtime-managed locations, not trivially-copyable pointer
 * fields. This wrapper is intentionally noncopyable and every load returns a
 * retained object that the caller must release or adopt into StrongRef.
 */
class WeakRef final {
public:
  explicit WeakRef(objc3_native_object_t object = nullptr) noexcept {
    objc3_native_weak_init(&slot_, object);
  }
  WeakRef(const WeakRef &) = delete;
  WeakRef &operator=(const WeakRef &) = delete;
  ~WeakRef() { objc3_native_weak_destroy(&slot_); }

  objc3_native_object_t loadRetained() const noexcept {
    return objc3_native_weak_load_retained(
        const_cast<objc3_native_weak_slot *>(&slot_));
  }
  void store(objc3_native_object_t object) noexcept {
    (void)objc3_native_weak_store(&slot_, object);
  }

private:
  objc3_native_weak_slot slot_{};
};

class EscapingBlock final {
public:
  EscapingBlock() noexcept = default;
  explicit EscapingBlock(objc3_native_block_t block) noexcept
      : block_(objc3_native_block_copy(block)) {}
  EscapingBlock(const EscapingBlock &other) noexcept
      : block_(objc3_native_block_copy(other.block_)) {}
  EscapingBlock(EscapingBlock &&other) noexcept
      : block_(std::exchange(other.block_, nullptr)) {}
  EscapingBlock &operator=(EscapingBlock other) noexcept {
    std::swap(block_, other.block_);
    return *this;
  }
  ~EscapingBlock() { objc3_native_block_release(block_); }
  objc3_native_block_t get() const noexcept { return block_; }

private:
  objc3_native_block_t block_ = nullptr;
};

} // namespace objc3::runtime::platform

#endif
