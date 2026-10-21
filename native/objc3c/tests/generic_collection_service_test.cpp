#include "runtime/public/generic_collection_service.h"

#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <new>
#include <string>
#include <thread>

namespace {
struct Item { std::string text; bool fail_copy = false; };
int item_destroys = 0;
int copy_failures = 0;
bool reenter_copy = false;
bool inside_reentry = false;
objc3_generic_array_t reentrant_array = nullptr;
objc3_generic_iterator_t reentrant_iterator = nullptr;
bool reenter_next = false;
Item reentrant_value{"from callback", false};
std::mutex block_mutex;
std::condition_variable block_changed;
bool block_copy = false;
bool copy_entered = false;
bool allow_copy_finish = false;

objc3_service_status CopyI64(void *dst, const void *src) {
  std::memcpy(dst, src, sizeof(std::int64_t));
  return OBJC3_SERVICE_OK;
}
objc3_service_status MoveI64(void *dst, void *src) {
  std::memcpy(dst, src, sizeof(std::int64_t));
  return OBJC3_SERVICE_OK;
}
void DestroyI64(void *) {}
void DestroyItem(void *value);

objc3_service_status CopyItem(void *dst, const void *src) {
  const auto &from = *static_cast<const Item *>(src);
  if (from.fail_copy) { ++copy_failures; return OBJC3_SERVICE_INVALID_ARGUMENT; }
  if (block_copy) {
    std::unique_lock<std::mutex> lock(block_mutex);
    copy_entered = true;
    block_changed.notify_all();
    block_changed.wait(lock, [] { return allow_copy_finish; });
  }
  if (reenter_copy && !inside_reentry && reentrant_array) {
    reenter_copy = false;
    inside_reentry = true;
    assert(objc3_generic_array_append(reentrant_array, &reentrant_value) ==
           OBJC3_GENERIC_COLLECTION_OK);
    inside_reentry = false;
  }
  if (reenter_next && !inside_reentry && reentrant_iterator) {
    reenter_next = false;
    inside_reentry = true;
    alignas(Item) unsigned char nested_result[sizeof(Item)];
    int nested_has_value = 0;
    assert(objc3_generic_iterator_next(reentrant_iterator, &nested_has_value,
        nested_result) == OBJC3_GENERIC_COLLECTION_OK && nested_has_value == 1);
    assert(reinterpret_cast<Item *>(nested_result)->text == "first");
    DestroyItem(nested_result);
    inside_reentry = false;
  }
  new (dst) Item(from);
  return OBJC3_SERVICE_OK;
}
objc3_service_status MoveItem(void *dst, void *src) {
  new (dst) Item(std::move(*static_cast<Item *>(src)));
  static_cast<Item *>(src)->~Item();
  return OBJC3_SERVICE_OK;
}
void DestroyItem(void *value) {
  static_cast<Item *>(value)->~Item();
  ++item_destroys;
}

struct ZeroPayload {};
struct alignas(64) AlignedPayload { std::uint64_t words[8]; };
objc3_service_status CopyZero(void *dst, const void *src) {
  return dst && src ? OBJC3_SERVICE_OK : OBJC3_SERVICE_INVALID_ARGUMENT;
}
objc3_service_status MoveZero(void *, void *) { return OBJC3_SERVICE_OK; }
void DestroyZero(void *) {}
objc3_service_status CopyAligned(void *dst, const void *src) {
  if (!dst || !src || reinterpret_cast<std::uintptr_t>(dst) % 64 ||
      reinterpret_cast<std::uintptr_t>(src) % 64)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::memcpy(dst, src, sizeof(AlignedPayload));
  return OBJC3_SERVICE_OK;
}
objc3_service_status MoveAligned(void *dst, void *src) {
  return CopyAligned(dst, src);
}
void DestroyAligned(void *) {}

objc3_value_ops MakeOps(std::uint64_t size, std::uint64_t alignment,
    objc3_service_status (*copy)(void *, const void *),
    objc3_service_status (*move)(void *, void *), void (*destroy)(void *)) {
  return {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION, size, alignment,
          copy, move, destroy};
}
}  // namespace

int main() {
  auto i64_ops = MakeOps(sizeof(std::int64_t), alignof(std::int64_t),
                         CopyI64, MoveI64, DestroyI64);
  const std::int64_t wide_values[] = {0, INT64_C(0x102030405060708)};
  objc3_generic_array_t wide = nullptr;
  assert(objc3_generic_array_create(&i64_ops, wide_values, 2, &wide) ==
         OBJC3_GENERIC_COLLECTION_OK);
  std::int64_t observed = -1;
  assert(objc3_generic_array_get(wide, 0, &observed) ==
         OBJC3_GENERIC_COLLECTION_OK && observed == 0);
  std::uint64_t count = 0;
  assert(objc3_generic_array_count(wide, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 2);
  alignas(std::int64_t) unsigned char misaligned_i64[sizeof(std::int64_t) + 1];
  objc3_generic_array_t invalid_array = nullptr;
  assert(objc3_generic_array_create(&i64_ops, misaligned_i64 + 1, 1,
         &invalid_array) ==
         OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT);
  assert(objc3_generic_array_get(wide, 0, misaligned_i64 + 1) ==
         OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT);
  alignas(objc3_value_ops) unsigned char misaligned_ops_storage[
      sizeof(objc3_value_ops) + 1];
  std::memcpy(misaligned_ops_storage + 1, &i64_ops, sizeof(i64_ops));
  auto *misaligned_ops = reinterpret_cast<const objc3_value_ops *>(
      misaligned_ops_storage + 1);
  assert(objc3_generic_array_create(misaligned_ops, wide_values, 2,
      &invalid_array) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT);

  auto aligned_ops = MakeOps(sizeof(AlignedPayload), alignof(AlignedPayload),
                             CopyAligned, MoveAligned, DestroyAligned);
  AlignedPayload aligned_values[2]{};
  aligned_values[1].words[7] = 42;
  objc3_generic_array_t aligned_array = nullptr;
  assert(objc3_generic_array_create(&aligned_ops, aligned_values, 2,
      &aligned_array) == OBJC3_GENERIC_COLLECTION_OK);
  alignas(64) unsigned char aligned_output[sizeof(AlignedPayload)];
  assert(objc3_generic_array_get(aligned_array, 1, aligned_output) ==
         OBJC3_GENERIC_COLLECTION_OK &&
         reinterpret_cast<AlignedPayload *>(aligned_output)->words[7] == 42);
  assert(objc3_generic_array_release(aligned_array) ==
         OBJC3_GENERIC_COLLECTION_OK);
  DestroyAligned(aligned_output);

  auto zero_ops = MakeOps(0, alignof(ZeroPayload), CopyZero, MoveZero, DestroyZero);
  objc3_generic_array_t zero_array = nullptr;
  assert(objc3_generic_array_create(&zero_ops, nullptr, 3, &zero_array) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_array_set(zero_array, 0, nullptr) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_array_append(zero_array, nullptr) ==
         OBJC3_GENERIC_COLLECTION_OK);
  objc3_generic_iterator_t zero_iterator = nullptr;
  assert(objc3_generic_array_iterator(zero_array, &zero_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  int has_value = -1;
  assert(objc3_generic_iterator_next(zero_iterator, &has_value, nullptr) ==
         OBJC3_GENERIC_COLLECTION_OK && has_value == 1);
  assert(objc3_generic_array_count(zero_array, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 4);
  for (int i = 1; i < 4; ++i) {
    has_value = -1;
    assert(objc3_generic_iterator_next(zero_iterator, &has_value, nullptr) ==
           OBJC3_GENERIC_COLLECTION_OK && has_value == 1);
  }
  has_value = -1;
  assert(objc3_generic_iterator_next(zero_iterator, &has_value, nullptr) ==
         OBJC3_GENERIC_COLLECTION_OK && has_value == 0);
  assert(objc3_generic_iterator_release(zero_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_array_release(zero_array) == OBJC3_GENERIC_COLLECTION_OK);

  auto item_ops = MakeOps(sizeof(Item), alignof(Item), CopyItem, MoveItem,
                          DestroyItem);
  auto move_only_ops = item_ops;
  move_only_ops.copy_init = nullptr;
  alignas(Item) unsigned char move_source[sizeof(Item)];
  new (move_source) Item{"owned", false};
  objc3_generic_array_t move_only = nullptr;
  assert(objc3_generic_array_create(&move_only_ops, move_source, 1,
      &move_only) == OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION);
  assert(objc3_generic_array_create_move(&move_only_ops, move_source, 1,
      &move_only) == OBJC3_GENERIC_COLLECTION_OK);
  alignas(Item) unsigned char move_out[sizeof(Item)];
  assert(objc3_generic_array_get(move_only, 0, move_out) ==
         OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION);
  assert(objc3_generic_array_append(move_only, &move_source[0]) ==
         OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION);
  assert(objc3_generic_array_release(move_only) == OBJC3_GENERIC_COLLECTION_OK);
  Item initial[] = {{"first", false}, {"second", false}};
  objc3_generic_array_t items = nullptr;
  assert(objc3_generic_array_create(&item_ops, initial, 2, &items) ==
         OBJC3_GENERIC_COLLECTION_OK);
  auto before_failure = item_destroys;
  Item failure{"fail", true};
  assert(objc3_generic_array_append(items, &failure) ==
         OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT);
  assert(copy_failures == 1);
  assert(objc3_generic_array_count(items, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 2);
  alignas(Item) unsigned char read_storage[sizeof(Item)];
  assert(objc3_generic_array_get(items, 0, read_storage) ==
         OBJC3_GENERIC_COLLECTION_OK &&
         reinterpret_cast<Item *>(read_storage)->text == "first");
  DestroyItem(read_storage);
  assert(item_destroys > before_failure);

  // A callback may mutate the same array. The outer prepared mutation observes
  // the generation change and fails without replacing the reentrant commit.
  objc3_generic_view_t stale_view = nullptr;
  objc3_generic_iterator_t stale_iterator = nullptr;
  assert(objc3_generic_array_slice(items, 0, 2, &stale_view) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_array_iterator(items, &stale_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  reentrant_array = items;
  reenter_copy = true;
  Item third{"third", false};
  assert(objc3_generic_array_append(items, &third) ==
         OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION);
  reenter_copy = false;
  reentrant_array = nullptr;
  assert(objc3_generic_array_count(items, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 3);
  alignas(Item) unsigned char preserved[sizeof(Item)];
  assert(objc3_generic_array_get(items, 1, preserved) ==
         OBJC3_GENERIC_COLLECTION_OK &&
         reinterpret_cast<Item *>(preserved)->text == "second");
  DestroyItem(preserved);
  alignas(Item) unsigned char stale_output[sizeof(Item)];
  assert(objc3_generic_view_get(stale_view, 0, stale_output) ==
         OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION);
  has_value = -1;
  assert(objc3_generic_iterator_next(stale_iterator, &has_value, stale_output) ==
         OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION && has_value == 0);
  assert(objc3_generic_view_release(stale_view) == OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_iterator_release(stale_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);

  // A recursive next() wins the cursor position while the outer copy callback
  // is active. The losing call destroys its result and does not advance again.
  objc3_generic_iterator_t recursive_iterator = nullptr;
  assert(objc3_generic_array_iterator(items, &recursive_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  reentrant_iterator = recursive_iterator;
  reenter_next = true;
  alignas(Item) unsigned char recursive_outer[sizeof(Item)];
  has_value = 0;
  assert(objc3_generic_iterator_next(recursive_iterator, &has_value,
      recursive_outer) == OBJC3_GENERIC_COLLECTION_CONCURRENT_ADVANCE &&
      has_value == 0);
  reentrant_iterator = nullptr;
  alignas(Item) unsigned char recursive_second[sizeof(Item)];
  assert(objc3_generic_iterator_next(recursive_iterator, &has_value,
      recursive_second) == OBJC3_GENERIC_COLLECTION_OK && has_value == 1 &&
      reinterpret_cast<Item *>(recursive_second)->text == "second");
  DestroyItem(recursive_second);
  assert(objc3_generic_iterator_release(recursive_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  alignas(Item) unsigned char reentrant_result[sizeof(Item)];
  assert(objc3_generic_array_get(items, 2, reentrant_result) ==
         OBJC3_GENERIC_COLLECTION_OK &&
         reinterpret_cast<Item *>(reentrant_result)->text == "from callback");
  DestroyItem(reentrant_result);

  objc3_generic_view_t reentrant_view = nullptr;
  assert(objc3_generic_array_slice(items, 0, 1, &reentrant_view) ==
         OBJC3_GENERIC_COLLECTION_OK);
  reentrant_array = items;
  reenter_copy = true;
  alignas(Item) unsigned char invalidated_read[sizeof(Item)];
  assert(objc3_generic_view_get(reentrant_view, 0, invalidated_read) ==
         OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION);
  reenter_copy = false;
  reentrant_array = nullptr;
  assert(objc3_generic_view_release(reentrant_view) ==
         OBJC3_GENERIC_COLLECTION_OK);

  // In-flight view reads hold both the view snapshot and the owner's lease
  // while its copy callback is running; concurrent public releases are safe.
  objc3_generic_view_t view = nullptr;
  assert(objc3_generic_array_slice(items, 0, 1, &view) ==
         OBJC3_GENERIC_COLLECTION_OK);
  block_copy = true;
  objc3_generic_collection_status read_status = OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT;
  alignas(Item) unsigned char concurrent_read[sizeof(Item)];
  std::thread reader([&] {
    read_status = objc3_generic_view_get(view, 0, &concurrent_read);
  });
  {
    std::unique_lock<std::mutex> lock(block_mutex);
    block_changed.wait(lock, [] { return copy_entered; });
  }
  assert(objc3_generic_view_release(view) == OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_array_release(items) == OBJC3_GENERIC_COLLECTION_OK);
  {
    std::lock_guard<std::mutex> lock(block_mutex);
    allow_copy_finish = true;
  }
  block_changed.notify_all();
  reader.join();
  block_copy = false;
  assert(read_status == OBJC3_GENERIC_COLLECTION_OK &&
         reinterpret_cast<Item *>(concurrent_read)->text == "first");
  DestroyItem(concurrent_read);

  assert(objc3_generic_array_release(wide) == OBJC3_GENERIC_COLLECTION_OK);
  return 0;
}
