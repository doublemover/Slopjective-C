#pragma once

#include "runtime/public/objc3_runtime_services.h"

#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <mutex>
#include <new>
#include <vector>

// A logical instance ID survives address reuse in the event history. Caller
// inputs have ID zero and must never receive the runtime's destroy callback.
namespace owned_payload_test {
struct alignas(64) Payload {
  std::int64_t value = 0;
  std::uint64_t instance = 0;
};
enum class Callback { Copy, Hash, Equal, None };
struct Ledger {
  struct Event { const void *address = nullptr; bool live = false; unsigned destroys = 0; };
  std::mutex mutex;
  std::array<Event, 65536> events{};
  std::uint64_t next = 1;
  std::atomic<unsigned> copies{0};
  unsigned key_copies = 0, value_copies = 0, hashes = 0, equals = 0;
  Callback failing = Callback::None;
  int failure_position = -1;
  unsigned matching_calls = 0;
  bool oom = false, collisions = false, incoherent = false;
  std::atomic<int> fail_allocation_after_copies{-1};
  std::atomic<int> allocation_failure_position{-1};
  bool FailAllocation() {
    if (allocation_failure_position < 0) return false;
    if (allocation_failure_position.fetch_sub(1) != 0) return false;
    allocation_failure_position = -1; return true;
  }
  void (*on_callback)() = nullptr;
  void (*on_destroy)() = nullptr;

  bool Fail(Callback kind) {
    if (on_callback) on_callback();
    if (kind != failing) return false;
    return matching_calls++ == static_cast<unsigned>(failure_position);
  }
  void FailAt(Callback kind, int position, bool allocation_status = false) {
    failing = kind; failure_position = position; matching_calls = 0; oom = allocation_status;
  }
  objc3_service_status Failure() const {
    return oom ? OBJC3_SERVICE_OUT_OF_MEMORY : OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  void Register(Payload &destination) {
    std::lock_guard lock(mutex);
    assert(next < events.size());
    destination.instance = next++;
    events[destination.instance] = {&destination, true, 0};
    ++copies;
  }
  void RegisterZero(void *destination) {
    std::lock_guard lock(mutex);
    for (std::uint64_t i = 1; i < next; ++i)
      assert(!events[i].live || events[i].address != destination);
    assert(next < events.size());
    events[next++] = {destination, true, 0};
    ++copies;
  }
  void DestroyZero(void *destination) {
    std::lock_guard lock(mutex);
    std::uint64_t found = 0;
    for (std::uint64_t i = 1; i < next; ++i)
      if (events[i].live && events[i].address == destination) { assert(!found); found = i; }
    assert(found && "destroyed an uninitialized zero-sized logical resource");
    assert(events[found].destroys == 0);
    events[found].live = false; ++events[found].destroys;
  }
  void Validate(const Payload &value) {
    if (!value.instance) return; // caller-owned source
    std::lock_guard lock(mutex);
    assert(value.instance < next && events[value.instance].live);
    assert(events[value.instance].address == &value);
  }
  void Destroy(Payload &value) {
    {
    std::lock_guard lock(mutex);
    assert(value.instance && value.instance < next && "destroyed a source or uninitialized destination");
    auto &event = events[value.instance];
    assert(event.live && event.destroys == 0 && event.address == &value && "duplicate payload destruction");
    event.live = false;
    ++event.destroys;
    value.instance = 0;
    }
    if (on_destroy) on_destroy();
  }
  std::vector<std::uint64_t> Live() {
    std::lock_guard lock(mutex);
    std::vector<std::uint64_t> result;
    for (std::uint64_t i = 1; i < next; ++i) if (events[i].live) result.push_back(i);
    return result;
  }
  void Balanced() {
    std::lock_guard lock(mutex);
    for (std::uint64_t i = 1; i < next; ++i)
      assert(!events[i].live && events[i].destroys == 1 && "initialized payload was not destroyed exactly once");
  }
  void Reset() {
    Balanced();
    next = 1; copies = 0; key_copies = value_copies = hashes = equals = 0;
    failing = Callback::None; failure_position = -1; matching_calls = 0;
    oom = collisions = incoherent = false; on_callback = nullptr; on_destroy = nullptr;
    fail_allocation_after_copies = allocation_failure_position = -1;
  }
};
inline Ledger ledger;

template <bool Key>
inline objc3_service_status Copy(void *destination, const void *source) {
  assert(reinterpret_cast<std::uintptr_t>(destination) % alignof(Payload) == 0);
  assert(reinterpret_cast<std::uintptr_t>(source) % alignof(Payload) == 0);
  const auto &input = *static_cast<const Payload *>(source);
  ledger.Validate(input);
  auto &output = *new (destination) Payload{input.value, 0};
  if (ledger.Fail(Callback::Copy)) return ledger.Failure();
  ledger.Register(output);
  if constexpr (Key) ++ledger.key_copies; else ++ledger.value_copies;
  return OBJC3_SERVICE_OK;
}
inline void Destroy(void *value) { ledger.Destroy(*static_cast<Payload *>(value)); }
inline objc3_service_status Move(void *destination, void *source) {
  // The ownership tests use copy construction. Keep move valid independently.
  const auto status = Copy<false>(destination, source);
  if (status == OBJC3_SERVICE_OK && static_cast<Payload *>(source)->instance) Destroy(source);
  return status;
}
inline objc3_value_ops Ops(bool key = false) {
  return {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
      sizeof(Payload), alignof(Payload), key ? Copy<true> : Copy<false>, Move, Destroy};
}
inline objc3_service_status Hash(const void *value, std::uint64_t *out) {
  ledger.Validate(*static_cast<const Payload *>(value));
  ++ledger.hashes;
  if (ledger.Fail(Callback::Hash)) return ledger.Failure();
  *out = ledger.collisions ? 0 : static_cast<const Payload *>(value)->value;
  return OBJC3_SERVICE_OK;
}
inline objc3_service_status Equal(const void *left, const void *right, int *out) {
  ledger.Validate(*static_cast<const Payload *>(left));
  ledger.Validate(*static_cast<const Payload *>(right));
  ++ledger.equals;
  if (ledger.Fail(Callback::Equal)) return ledger.Failure();
  const auto a = static_cast<const Payload *>(left)->value;
  const auto b = static_cast<const Payload *>(right)->value;
  *out = ledger.incoherent ? a % 10 == b % 10 : a == b;
  return OBJC3_SERVICE_OK;
}
} // namespace owned_payload_test
