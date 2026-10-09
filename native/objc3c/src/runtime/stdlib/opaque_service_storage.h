#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace objc3c::runtime::stdlib_runtime {

// Storage for additive opaque-pointer service ABIs. Tokens are identity-only
// addresses: callers must never dereference them or derive a native object id
// from them. Token objects remain as tombstones for the registry lifetime, so
// allocator address reuse can never make a stale token live again.
template <typename T, std::size_t Capacity = 1U << 20>
class OpaqueServiceStorage {
 public:
  enum class Status { Ok, InvalidHandle, WrongKind, StaleHandle };
  OpaqueServiceStorage() = default;
  ~OpaqueServiceStorage() noexcept {
    try {
      Shutdown();
    } catch (...) {
      // Destructors must not let registry shutdown exceptions escape.
    }
  }
  OpaqueServiceStorage(const OpaqueServiceStorage &) = delete;
  OpaqueServiceStorage &operator=(const OpaqueServiceStorage &) = delete;

  void *Store(std::unique_ptr<T> value, std::uint32_t kind) {
    if (!value || kind == 0) return nullptr;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (shutting_down_ || resetting_ || live_count_ >= Capacity ||
          tokens_.size() >= Capacity) return nullptr;
    }
    // Keep a strong candidate outside the lock's lifetime. If map insertion
    // allocates and throws, its temporary Entry must not run T's destructor
    // while the registry mutex is held.
    std::shared_ptr<T> candidate(std::move(value));
    std::lock_guard<std::mutex> lock(mutex_);
    if (shutting_down_ || resetting_ || live_count_ >= Capacity ||
        tokens_.size() >= Capacity) return nullptr;
    auto token = std::make_unique<Token>();
    token->serial = next_serial_++;
    token->kind = kind;
    void *key = token.get();
    tokens_.push_back(std::move(token));
    entries_.emplace(key, Entry{candidate, 1, kind});
    ++live_count_;
    return key;
  }

  void *Retain(const void *handle, std::uint32_t expected_kind) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (shutting_down_ || resetting_) return nullptr;
    auto found = entries_.find(const_cast<void *>(handle));
    if (found == entries_.end() || !found->second.value ||
        found->second.kind != expected_kind ||
        found->second.references == UINT64_MAX) return nullptr;
    ++found->second.references;
    return const_cast<void *>(handle);
  }

  bool Release(const void *handle, std::uint32_t expected_kind) {
    std::shared_ptr<T> retired;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (shutting_down_ || resetting_) return false;
      auto found = entries_.find(const_cast<void *>(handle));
      if (found == entries_.end() || !found->second.value ||
          found->second.kind != expected_kind ||
          found->second.references == 0) return false;
      if (--found->second.references == 0) {
        retired = std::move(found->second.value);
        --live_count_;
      }
    }
    // User destructors run after releasing the registry lock.
    retired.reset();
    return true;
  }

  template <typename Callback>
  bool With(const void *handle, std::uint32_t expected_kind,
            Callback callback) {
    std::shared_ptr<T> snapshot;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (shutting_down_ || resetting_) return false;
      auto found = entries_.find(const_cast<void *>(handle));
      if (found == entries_.end() || !found->second.value ||
          found->second.kind != expected_kind) return false;
      snapshot = found->second.value;
    }
    callback(*snapshot);
    return true;
  }

  Status Inspect(const void *handle, std::uint32_t expected_kind) const {
    if (handle == nullptr) return Status::InvalidHandle;
    std::lock_guard<std::mutex> lock(mutex_);
    auto found = entries_.find(const_cast<void *>(handle));
    if (found == entries_.end()) return Status::InvalidHandle;
    if (found->second.kind != expected_kind) return Status::WrongKind;
    if (!found->second.value) return Status::StaleHandle;
    return Status::Ok;
  }

  bool AtCapacity() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tokens_.size() >= Capacity;
  }

  std::size_t LiveCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return live_count_;
  }

  std::size_t TokenCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tokens_.size();
  }

  // Reset revokes all tokens and drops all values. Tombstones remain so stale
  // addresses cannot ever be issued again. Values are destroyed outside lock.
  void Reset() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (shutting_down_ || resetting_) return;
      resetting_ = true;
      entries_.swap(retired_entries_);
      live_count_ = 0;
    }
    // Detach under lock, then run value destructors only after unlock. During
    // destruction, reentrant storage operations are rejected until reset ends.
    retired_entries_.clear();
    {
      std::lock_guard<std::mutex> lock(mutex_);
      resetting_ = false;
    }
  }

 private:
  struct Token {
    std::uint64_t serial = 0;
    std::uint32_t kind = 0;
  };
  struct Entry {
    std::shared_ptr<T> value;
    std::uint64_t references;
    std::uint32_t kind;
  };

  void Shutdown() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (shutting_down_) return;
      shutting_down_ = true;
      resetting_ = true;
      entries_.swap(retired_entries_);
      live_count_ = 0;
    }
    // No allocation occurs during shutdown, and destructors run while the
    // registry is still fully constructed but rejects reentrant operations.
    retired_entries_.clear();
  }

  mutable std::mutex mutex_;
  std::unordered_map<void *, Entry> entries_;
  // Preconstruct the detach map with the registry. MSVC's unordered_map
  // constructor may allocate, so creating it inside Shutdown is not safe.
  std::unordered_map<void *, Entry> retired_entries_;
  std::vector<std::unique_ptr<Token>> tokens_;
  std::uint64_t next_serial_ = 1;
  std::size_t live_count_ = 0;
  bool resetting_ = false;
  bool shutting_down_ = false;
};

}  // namespace objc3c::runtime::stdlib_runtime
