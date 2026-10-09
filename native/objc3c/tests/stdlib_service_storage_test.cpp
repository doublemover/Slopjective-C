#include "runtime/stdlib/opaque_service_storage.h"
#include "runtime/stdlib/stdlib_runtime_storage.h"
#include "runtime/stdlib/stdlib_service_contract.h"
#include "runtime/stdlib/text_runtime_contract.h"
#include "runtime/stdlib/collections_runtime_contract.h"

#include <cassert>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace objc3c::runtime::stdlib_runtime;
namespace objc3c::runtime {
void ResetRuntimeStdlibTextStateForTesting();
void ResetRuntimeStdlibCollectionsStateForTesting();
}

struct LegacyRecord { RecordHeader header; int payload = 0; };
struct ReentrantValue {
  std::function<void()> on_destroy;
  ~ReentrantValue() { if (on_destroy) on_destroy(); }
};
struct OwnerLeaseValue {
  std::function<void()> on_destroy;
  ~OwnerLeaseValue() { if (on_destroy) on_destroy(); }
};

int main() {
  // New typed service supports a zero-sized value, ownership copies, stale
  // rejection after final release, and bounded tombstone capacity.
  OpaqueServiceStorage<int, 3> typed;
  void *zero = typed.Store(std::make_unique<int>(0), 7);
  assert(zero != nullptr);
  int observed = -1;
  assert(typed.With(zero, 7, [&](int &value) {
    observed = value;
    (void)typed.LiveCount();
  }));
  assert(observed == 0);
  assert(typed.Retain(zero, 8) == nullptr);
  assert(typed.Retain(zero, 7) == zero);
  assert(typed.Release(zero, 7));
  assert(typed.LiveCount() == 1);
  assert(typed.Release(zero, 7));
  assert(typed.LiveCount() == 0);
  assert(!typed.With(zero, 7, [](int &) {}));
  void *one = typed.Store(std::make_unique<int>(1), 7);
  void *two = typed.Store(std::make_unique<int>(2), 7);
  assert(one && two && one != zero && two != zero && two != one);
  assert(typed.Store(std::make_unique<int>(3), 7) == nullptr);

  OpaqueServiceStorage<ReentrantValue, 1> reentrant;
  int count_during_destroy = -1;
  auto reentrant_value = std::make_unique<ReentrantValue>();
  reentrant_value->on_destroy = [&] {
    count_during_destroy = static_cast<int>(reentrant.LiveCount());
  };
  void *reentrant_handle = reentrant.Store(std::move(reentrant_value), 9);
  assert(reentrant_handle && reentrant.Release(reentrant_handle, 9));
  assert(count_during_destroy == 0);

  // Reset drops values outside the registry lock and rejects reentrant store
  // until every detached value has been destroyed.
  OpaqueServiceStorage<ReentrantValue, 2> reset_reentrant;
  int reset_count = -1;
  bool reset_store_rejected = false;
  auto value_for_reset_attempt = std::make_unique<ReentrantValue>();
  auto reset_value = std::make_unique<ReentrantValue>();
  reset_value->on_destroy = [&] {
    reset_count = static_cast<int>(reset_reentrant.LiveCount());
    reset_store_rejected = reset_reentrant.Store(
        std::move(value_for_reset_attempt), 10) == nullptr;
    reset_reentrant.Reset();
  };
  assert(reset_reentrant.Store(std::move(reset_value), 10) != nullptr);
  reset_reentrant.Reset();
  assert(reset_count == 0 && reset_store_rejected);
  assert(reset_reentrant.Store(std::make_unique<ReentrantValue>(), 10) !=
         nullptr);

  int shutdown_count = -1;
  bool shutdown_store_rejected = false;
  {
    auto value_for_shutdown_attempt = std::make_unique<ReentrantValue>();
    OpaqueServiceStorage<ReentrantValue, 2> shutdown_reentrant;
    auto shutdown_value = std::make_unique<ReentrantValue>();
    shutdown_value->on_destroy = [&] {
      shutdown_count = static_cast<int>(shutdown_reentrant.LiveCount());
      shutdown_store_rejected = shutdown_reentrant.Store(
          std::move(value_for_shutdown_attempt), 14) == nullptr;
      shutdown_reentrant.Reset();
    };
    assert(shutdown_reentrant.Store(std::move(shutdown_value), 14) != nullptr);
  }
  assert(shutdown_count == 0 && shutdown_store_rejected);

  // A retained view snapshot owns its parent through the end of an in-flight
  // callback, even after the public view handle is released concurrently.
  // The callback destructor reenters the owner registry, proving it runs
  // outside the registry lock. Views have no public retain API.
  OpaqueServiceStorage<ReentrantValue, 2> owner_storage;
  OpaqueServiceStorage<OwnerLeaseValue, 1> view_storage;
  int owner_destroy_count = -1;
  auto owner_value = std::make_unique<ReentrantValue>();
  owner_value->on_destroy = [&] {
    owner_destroy_count = static_cast<int>(owner_storage.LiveCount());
  };
  void *owner_handle = owner_storage.Store(std::move(owner_value), 12);
  assert(owner_handle != nullptr);
  assert(owner_storage.Retain(owner_handle, 12) == owner_handle);
  assert(owner_storage.Release(owner_handle, 12));  // external owner ref
  auto view_value = std::make_unique<OwnerLeaseValue>();
  view_value->on_destroy = [&] { assert(owner_storage.Release(owner_handle, 12)); };
  void *view_handle = view_storage.Store(std::move(view_value), 13);
  assert(view_handle != nullptr);
  std::mutex snapshot_mutex;
  std::condition_variable snapshot_changed;
  bool callback_entered = false;
  bool finish_callback = false;
  bool owner_visible_in_callback = false;
  std::thread reader([&] {
    assert(view_storage.With(view_handle, 13, [&](OwnerLeaseValue &) {
      std::unique_lock<std::mutex> lock(snapshot_mutex);
      callback_entered = true;
      snapshot_changed.notify_all();
      snapshot_changed.wait(lock, [&] { return finish_callback; });
      lock.unlock();
      owner_visible_in_callback = owner_storage.With(
          owner_handle, 12, [](ReentrantValue &) {});
    }));
  });
  {
    std::unique_lock<std::mutex> lock(snapshot_mutex);
    snapshot_changed.wait(lock, [&] { return callback_entered; });
  }
  assert(view_storage.Release(view_handle, 13));
  assert(owner_storage.With(owner_handle, 12, [](ReentrantValue &) {}));
  {
    std::lock_guard<std::mutex> lock(snapshot_mutex);
    finish_callback = true;
  }
  snapshot_changed.notify_all();
  reader.join();
  assert(owner_visible_in_callback);
  assert(owner_destroy_count == 0);
  assert((owner_storage.Inspect(owner_handle, 12) ==
          OpaqueServiceStorage<ReentrantValue, 2>::Status::StaleHandle));

  // Churn past the old 9,999-slot ceiling while keeping live occupancy at 0.
  OpaqueServiceStorage<int, 12000> churn;
  void *previous = nullptr;
  std::unordered_set<void *> identities;
  for (int i = 0; i < 12000; ++i) {
    void *current = churn.Store(std::make_unique<int>(i), 11);
    assert(current && current != previous);
    assert(identities.insert(current).second);
    assert(churn.Release(current, 11));
    assert(churn.LiveCount() == 0);
    previous = current;
  }
  assert(churn.TokenCount() == 12000);
  assert(churn.Store(std::make_unique<int>(0), 11) == nullptr);

  // Legacy handles retain their explicit integer domain. A released slot is
  // tombstoned, stale values fail lookup, and reset generations never wrap.
  HandleTable<LegacyRecord, 2> legacy(HandleTableOwner::Text);
  const int old = legacy.Store(DescriptorKind::TextOwnedUtf8, LegacyRecord{});
  assert(old != 0);
  assert(legacy.Lookup(old, {DescriptorKind::CollectionImmutableArray}).status ==
         LookupStatus::CrossKind);
  assert(legacy.Retain(old) == LookupStatus::Ok);
  assert(legacy.Release(old) == LookupStatus::Ok);
  assert(legacy.Lookup(old, {DescriptorKind::TextOwnedUtf8}).status ==
         LookupStatus::Ok);
  assert(legacy.Release(old) == LookupStatus::Ok);
  assert(legacy.Lookup(old, {DescriptorKind::TextOwnedUtf8}).status ==
         LookupStatus::StaleHandle);
  const int another = legacy.Store(DescriptorKind::TextOwnedUtf8,
                                   LegacyRecord{});
  assert(another && another != old);
  const int second = legacy.Store(DescriptorKind::TextOwnedUtf8, LegacyRecord{});
  assert(second != 0);
  assert(legacy.Store(DescriptorKind::TextOwnedUtf8, LegacyRecord{}) == 0);

  HandleTable<LegacyRecord, 2> reset_boundary(HandleTableOwner::Text);
  const int before_resets = reset_boundary.Store(
      DescriptorKind::TextOwnedUtf8, LegacyRecord{});
  for (int i = 0; i < 98; ++i) reset_boundary.Reset();
  assert(reset_boundary.Store(DescriptorKind::TextOwnedUtf8, LegacyRecord{}) !=
         0);
  assert(reset_boundary.Lookup(before_resets,
      {DescriptorKind::TextOwnedUtf8}).status == LookupStatus::StaleHandle);
  reset_boundary.Reset();
  assert(reset_boundary.Store(DescriptorKind::TextOwnedUtf8, LegacyRecord{}) ==
         0);
  assert(legacy.Release(another) == LookupStatus::Ok);
  const int reused = legacy.Store(DescriptorKind::TextOwnedUtf8, LegacyRecord{});
  assert(reused && reused != another && reused != old);
  assert(legacy.Lookup(another, {DescriptorKind::TextOwnedUtf8}).status ==
         LookupStatus::StaleHandle);

  // Exercise the real C ABI implementation, including strongly separated
  // text/collection domains and legacy compatibility release entrypoints.
  objc3_text_v2_t text = objc3_runtime_stdlib_text_v2_create_utf8(nullptr, 0);
  assert(text != nullptr);
  uint64_t byte_count = 9;
  assert(objc3_runtime_stdlib_text_v2_byte_count(text, &byte_count));
  assert(byte_count == 0);
  assert(objc3_runtime_stdlib_text_v2_retain(text) == text);
  assert(!objc3_runtime_stdlib_collections_v2_count(
      reinterpret_cast<objc3_collection_v2_t>(text), &byte_count));
  objc3_runtime_stdlib_text_v2_release(text);
  assert(objc3_runtime_stdlib_text_v2_byte_count(text, &byte_count));
  objc3_runtime_stdlib_text_v2_release(text);
  assert(!objc3_runtime_stdlib_text_v2_byte_count(text, &byte_count));

  const char sample[] = {'A', '\0', static_cast<char>(0xc3),
                         static_cast<char>(0xa9)};
  objc3_text_v2_t rich_text = nullptr;
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(
             sample, sizeof(sample), &rich_text) == OBJC3_STDLIB_V2_OK);
  uint64_t scalar_count = 0;
  assert(objc3_runtime_stdlib_text_v2_scalar_count_checked(
             rich_text, &scalar_count) == OBJC3_STDLIB_V2_OK);
  assert(scalar_count == 3);
  uint8_t text_byte = 1;
  assert(objc3_runtime_stdlib_text_v2_byte_at_checked(
             rich_text, 1, &text_byte) == OBJC3_STDLIB_V2_OK);
  assert(text_byte == 0);
  assert(objc3_runtime_stdlib_text_v2_byte_at_checked(
             rich_text, sizeof(sample), &text_byte) ==
         OBJC3_STDLIB_V2_OUT_OF_BOUNDS);
  objc3_text_v2_t scalar_slice = nullptr;
  assert(objc3_runtime_stdlib_text_v2_slice_scalars(
             rich_text, 1, 1, &scalar_slice) == OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_byte_count(scalar_slice, &byte_count));
  assert(byte_count == 1);  // U+0000 is one scalar and one byte.
  int text_found = 0;
  uint64_t byte_index = 0;
  assert(objc3_runtime_stdlib_text_v2_find_bytes(
             rich_text, sample + 2, 2, 0, &text_found, &byte_index) ==
         OBJC3_STDLIB_V2_OK);
  assert(text_found == 1 && byte_index == 2);
  assert(objc3_runtime_stdlib_text_v2_find_bytes(
             rich_text, nullptr, 0, sizeof(sample), &text_found,
             &byte_index) == OBJC3_STDLIB_V2_OK);
  assert(text_found == 1 && byte_index == sizeof(sample));
  assert(objc3_runtime_stdlib_text_v2_find_bytes(
             rich_text, nullptr, 0, sizeof(sample) + 1, &text_found,
             &byte_index) == OBJC3_STDLIB_V2_OUT_OF_BOUNDS);
  const char malformed[] = {static_cast<char>(0xff)};
  objc3_text_v2_t invalid_text = nullptr;
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(
             malformed, sizeof(malformed), &invalid_text) ==
         OBJC3_STDLIB_V2_INVALID_UTF8);
  objc3_text_v2_t joined = nullptr;
  assert(objc3_runtime_stdlib_text_v2_concat(
             rich_text, scalar_slice, &joined) == OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_byte_count(joined, &byte_count));
  assert(byte_count == 5);
  objc3_text_builder_v2_t builder = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_create(&builder) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_append_utf8(
             builder, sample, sizeof(sample)) == OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_append_utf8(
             builder, malformed, sizeof(malformed)) ==
         OBJC3_STDLIB_V2_INVALID_UTF8);
  assert(objc3_runtime_stdlib_text_builder_v2_append_text(
             builder, scalar_slice) == OBJC3_STDLIB_V2_OK);
  objc3_text_v2_t built = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_finish(builder, &built) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_byte_count(built, &byte_count));
  assert(byte_count == sizeof(sample) + 1);
  assert(objc3_runtime_stdlib_text_builder_v2_reset(builder) ==
         OBJC3_STDLIB_V2_OK);
  objc3_text_v2_t empty_built = nullptr;
  assert(objc3_runtime_stdlib_text_builder_v2_finish(builder, &empty_built) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_byte_count(empty_built, &byte_count));
  assert(byte_count == 0);
  assert(objc3_runtime_stdlib_text_builder_v2_release(builder) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_builder_v2_release(builder) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);
  assert(objc3_runtime_stdlib_text_v2_release_checked(rich_text) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(scalar_slice) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(joined) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(built) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_release_checked(empty_built) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(
             sample, UINT64_MAX, &invalid_text) ==
         OBJC3_STDLIB_V2_CAPACITY_EXCEEDED);
  std::vector<char> large_bytes(1U << 20, 'x');
  objc3_text_v2_t large_text = nullptr;
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(
             large_bytes.data(), large_bytes.size(), &large_text) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_text_v2_concat(
             large_text, large_text, &invalid_text) ==
         OBJC3_STDLIB_V2_CAPACITY_EXCEEDED);
  assert(objc3_runtime_stdlib_text_v2_release_checked(large_text) ==
         OBJC3_STDLIB_V2_OK);

  objc3_collection_v2_t collection =
      objc3_runtime_stdlib_collections_v2_create_i32(nullptr, 0);
  assert(collection != nullptr);
  assert(objc3_runtime_stdlib_collections_v2_count(collection, &byte_count));
  assert(byte_count == 0);
  objc3_runtime_stdlib_collections_v2_release(collection);
  assert(!objc3_runtime_stdlib_collections_v2_count(collection, &byte_count));

  const int32_t array_values[] = {0, -2, 4};
  objc3_array_i32_v2_t typed_array = nullptr;
  assert(objc3_runtime_stdlib_array_i32_v2_create(
             array_values, 3, &typed_array) == OBJC3_STDLIB_V2_OK);
  int32_t typed_value = 123;
  assert(objc3_runtime_stdlib_array_i32_v2_get(
             typed_array, 0, &typed_value) == OBJC3_STDLIB_V2_OK);
  assert(typed_value == 0);
  assert(objc3_runtime_stdlib_array_i32_v2_get(
             typed_array, 3, &typed_value) == OBJC3_STDLIB_V2_OUT_OF_BOUNDS);
  objc3_slice_i32_v2_t typed_slice = nullptr;
  assert(objc3_runtime_stdlib_array_i32_v2_slice(
             typed_array, 1, 2, &typed_slice) == OBJC3_STDLIB_V2_OK);
  objc3_iterator_i32_v2_t typed_iterator = nullptr;
  assert(objc3_runtime_stdlib_array_i32_v2_iterator(
             typed_array, &typed_iterator) == OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_array_i32_v2_release(typed_array) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_slice_i32_v2_get(
             typed_slice, 0, &typed_value) == OBJC3_STDLIB_V2_OK);
  assert(typed_value == -2);
  int has_value = 0;
  assert(objc3_runtime_stdlib_iterator_i32_v2_next(
             typed_iterator, &has_value, &typed_value) == OBJC3_STDLIB_V2_OK);
  assert(has_value == 1 && typed_value == 0);
  assert(objc3_runtime_stdlib_slice_i32_v2_release(typed_slice) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_iterator_i32_v2_release(typed_iterator) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_slice_i32_v2_release(typed_slice) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);
  assert(objc3_runtime_stdlib_iterator_i32_v2_release(typed_iterator) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);
  assert(objc3_runtime_stdlib_array_i32_v2_count(
             typed_array, &byte_count) == OBJC3_STDLIB_V2_STALE_HANDLE);
  assert(objc3_runtime_stdlib_array_i32_v2_create(
             array_values, (1U << 20) + 1, &typed_array) ==
         OBJC3_STDLIB_V2_CAPACITY_EXCEEDED);

  objc3_array_i32_v2_t mutating_array = nullptr;
  assert(objc3_runtime_stdlib_array_i32_v2_create(
             array_values, 3, &mutating_array) == OBJC3_STDLIB_V2_OK);
  objc3_slice_i32_v2_t invalidated_slice = nullptr;
  assert(objc3_runtime_stdlib_array_i32_v2_slice(
             mutating_array, 0, 2, &invalidated_slice) == OBJC3_STDLIB_V2_OK);
  objc3_iterator_i32_v2_t invalidated_iterator_v2 = nullptr;
  assert(objc3_runtime_stdlib_array_i32_v2_iterator(
             mutating_array, &invalidated_iterator_v2) == OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_array_i32_v2_append(mutating_array, 5) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_slice_i32_v2_count(
             invalidated_slice, &byte_count) ==
         OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION);
  assert(objc3_runtime_stdlib_iterator_i32_v2_next(
             invalidated_iterator_v2, &has_value, &typed_value) ==
         OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION);
  assert(objc3_runtime_stdlib_array_i32_v2_release(mutating_array) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_slice_i32_v2_release(invalidated_slice) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_iterator_i32_v2_release(invalidated_iterator_v2) ==
         OBJC3_STDLIB_V2_OK);

  objc3_map_i32_i32_v2_t typed_map = nullptr;
  assert(objc3_runtime_stdlib_map_i32_i32_v2_create(&typed_map) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_insert(typed_map, 0, 0) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_insert(typed_map, 2, 9) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_insert(typed_map, 0, 7) ==
         OBJC3_STDLIB_V2_OK);
  int map_found = 0;
  int32_t map_value = -1;
  assert(objc3_runtime_stdlib_map_i32_i32_v2_get(
             typed_map, 0, &map_found, &map_value) == OBJC3_STDLIB_V2_OK);
  assert(map_found == 1 && map_value == 7);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_get(
             typed_map, 100, &map_found, &map_value) == OBJC3_STDLIB_V2_OK);
  assert(map_found == 0);
  objc3_iterator_i32_v2_t map_iterator = nullptr;
  assert(objc3_runtime_stdlib_map_i32_i32_v2_iterator(
             typed_map, &map_iterator) == OBJC3_STDLIB_V2_OK);
  int32_t map_key = -1;
  assert(objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(
             map_iterator, &has_value, &map_key, &map_value) ==
         OBJC3_STDLIB_V2_OK);
  assert(has_value == 1 && map_key == 0 && map_value == 7);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(
             map_iterator, &has_value, &map_key, &map_value) ==
         OBJC3_STDLIB_V2_OK);
  assert(has_value == 1 && map_key == 2 && map_value == 9);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_insert(typed_map, 3, 11) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(
             map_iterator, &has_value, &map_key, &map_value) ==
         OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_release(typed_map) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(
             map_iterator, &has_value, &map_key, &map_value) ==
         OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION);
  assert(objc3_runtime_stdlib_iterator_i32_v2_release(map_iterator) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_iterator_i32_v2_release(map_iterator) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);

  objc3_set_i32_v2_t typed_set = nullptr;
  assert(objc3_runtime_stdlib_set_i32_v2_create(&typed_set) ==
         OBJC3_STDLIB_V2_OK);
  int inserted = 0;
  assert(objc3_runtime_stdlib_set_i32_v2_insert(typed_set, 0, &inserted) ==
         OBJC3_STDLIB_V2_OK && inserted == 1);
  assert(objc3_runtime_stdlib_set_i32_v2_insert(typed_set, 0, &inserted) ==
         OBJC3_STDLIB_V2_OK && inserted == 0);
  assert(objc3_runtime_stdlib_set_i32_v2_contains(
             typed_set, 0, &map_found) == OBJC3_STDLIB_V2_OK && map_found == 1);
  objc3_iterator_i32_v2_t set_iterator = nullptr;
  assert(objc3_runtime_stdlib_set_i32_v2_iterator(
             typed_set, &set_iterator) == OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_iterator_i32_v2_next(
             set_iterator, &has_value, &typed_value) == OBJC3_STDLIB_V2_OK);
  assert(has_value == 1 && typed_value == 0);
  assert(objc3_runtime_stdlib_array_i32_v2_count(
             reinterpret_cast<objc3_array_i32_v2_t>(typed_set), &byte_count) ==
         OBJC3_STDLIB_V2_WRONG_KIND);
  assert(objc3_runtime_stdlib_set_i32_v2_release(typed_set) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_iterator_i32_v2_next(
             set_iterator, &has_value, &typed_value) == OBJC3_STDLIB_V2_OK);
  assert(has_value == 0);  // The internal iterator lease keeps the set alive.
  assert(objc3_runtime_stdlib_iterator_i32_v2_release(set_iterator) ==
         OBJC3_STDLIB_V2_OK);
  assert(objc3_runtime_stdlib_iterator_i32_v2_release(set_iterator) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);
  assert(objc3_runtime_stdlib_set_i32_v2_release(typed_set) ==
         OBJC3_STDLIB_V2_STALE_HANDLE);

  const int array_value[] = {42};
  const int array = objc3_runtime_stdlib_collections_array_storage_i32(
      array_value, 1);
  const int iterator =
      objc3_runtime_stdlib_collections_array_iterator_i32(array);
  assert(array != 0 && iterator != 0);
  assert(objc3_runtime_stdlib_collections_release_i32(array) ==
         OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK);
  assert(objc3_runtime_stdlib_collections_iterator_next_or_i32(iterator, -1) ==
         42);
  assert(objc3_runtime_stdlib_collections_release_i32(iterator) ==
         OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK);

  const int mutable_array =
      objc3_runtime_stdlib_collections_mutable_array_i32();
  assert(objc3_runtime_stdlib_collections_mutable_array_append_i32(
             mutable_array, 7) == 1);
  const int invalidated_iterator =
      objc3_runtime_stdlib_collections_array_iterator_i32(mutable_array);
  assert(objc3_runtime_stdlib_collections_mutable_array_append_i32(
             mutable_array, 8) == 2);
  assert(objc3_runtime_stdlib_collections_iterator_next_or_i32(
             invalidated_iterator, -1) == -1);
  assert(objc3_runtime_stdlib_collections_last_status_i32() ==
         OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION);
  assert(objc3_runtime_stdlib_collections_release_i32(mutable_array) ==
         OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK);
  assert(objc3_runtime_stdlib_collections_iterator_next_or_i32(
             invalidated_iterator, -2) == -2);
  assert(objc3_runtime_stdlib_collections_release_i32(invalidated_iterator) ==
         OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK);

  const int legacy_text = objc3_runtime_stdlib_text_utf8_literal_i32(0, 0, 1);
  assert(legacy_text != 0);
  assert(objc3_runtime_stdlib_text_retain_i32(legacy_text) ==
         OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK);
  assert(objc3_runtime_stdlib_text_release_i32(legacy_text) ==
         OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK);
  assert(objc3_runtime_stdlib_text_release_i32(legacy_text) ==
         OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK);
  assert(objc3_runtime_stdlib_text_byte_count_i32(legacy_text) == 0);
  assert(objc3_runtime_stdlib_text_last_status_i32() ==
         OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STALE_HANDLE);
  const int legacy_reused =
      objc3_runtime_stdlib_text_utf8_literal_i32(0, 0, 1);
  assert(legacy_reused != 0 && legacy_reused != legacy_text);
  assert(objc3_runtime_stdlib_text_byte_count_i32(legacy_text) == 0);
  assert(objc3_runtime_stdlib_text_last_status_i32() ==
         OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STALE_HANDLE);

  objc3c::runtime::ResetRuntimeStdlibTextStateForTesting();
  for (int i = 0; i < 140; ++i) {
    objc3c::runtime::ResetRuntimeStdlibTextStateForTesting();
  }
  assert(objc3_runtime_stdlib_text_utf8_literal_i32(0, 0, 1) == 0);
  assert(objc3_runtime_stdlib_text_last_status_i32() ==
         OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED);
  objc3c::runtime::ResetRuntimeStdlibCollectionsStateForTesting();
  for (int i = 0; i < 140; ++i) {
    objc3c::runtime::ResetRuntimeStdlibCollectionsStateForTesting();
  }
  assert(objc3_runtime_stdlib_collections_array_storage_i32(array_value, 1) ==
         0);
  for (int i = 0; i < 140; ++i) legacy.Reset();
  assert(legacy.Lookup(old, {DescriptorKind::TextOwnedUtf8}).status ==
         LookupStatus::StaleHandle);
  assert(legacy.Store(DescriptorKind::TextOwnedUtf8, LegacyRecord{}) == 0);
  return 0;
}
