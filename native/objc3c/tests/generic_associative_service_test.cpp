#include "runtime/public/generic_associative_service.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <new>

namespace {
using Status = objc3_generic_collection_status;
struct Item { std::string text; bool fail_copy = false; bool oom_copy = false; };
objc3_generic_map_t reentrant_map = nullptr;
bool reenter_hash = false;
bool inside_hash = false;
bool equal_all_keys = false;
bool reenter_destroy = false;
objc3_generic_map_t destroy_reentry_map = nullptr;
int destroy_reentry_status = -1;
int item_destroyed = 0;

objc3_service_status CopyI64(void *dst, const void *src) {
  std::memcpy(dst, src, sizeof(std::int64_t));
  return OBJC3_SERVICE_OK;
}
objc3_service_status MoveI64(void *dst, void *src) {
  std::memcpy(dst, src, sizeof(std::int64_t));
  return OBJC3_SERVICE_OK;
}
void DestroyI64(void *) {}
objc3_service_status HashI64(const void *key, std::uint64_t *out) {
  std::int64_t value = 0;
  std::memcpy(&value, key, sizeof(value));
  *out = static_cast<std::uint64_t>(value) * UINT64_C(11400714819323198485);
  if (reenter_hash && !inside_hash && reentrant_map) {
    reenter_hash = false;
    inside_hash = true;
    const std::int64_t nested_key = 3;
    const std::int64_t nested_value = 30;
    int inserted = 0;
    assert(objc3_generic_map_insert(reentrant_map, &nested_key,
        &nested_value, &inserted) == OBJC3_GENERIC_COLLECTION_OK && inserted == 1);
    inside_hash = false;
  }
  return OBJC3_SERVICE_OK;
}
objc3_service_status EqualI64(const void *left, const void *right,
                              int *out_equal) {
  if (equal_all_keys) { *out_equal = 1; return OBJC3_SERVICE_OK; }
  std::int64_t lhs = 0, rhs = 0;
  std::memcpy(&lhs, left, sizeof(lhs));
  std::memcpy(&rhs, right, sizeof(rhs));
  *out_equal = lhs == rhs;
  return OBJC3_SERVICE_OK;
}
objc3_service_status CopyItem(void *dst, const void *src) {
  const auto &source = *static_cast<const Item *>(src);
  if (source.oom_copy) return OBJC3_SERVICE_OUT_OF_MEMORY;
  if (source.fail_copy) return OBJC3_SERVICE_INVALID_ARGUMENT;
  new (dst) Item(source);
  return OBJC3_SERVICE_OK;
}
objc3_service_status MoveItem(void *dst, void *src) {
  new (dst) Item(std::move(*static_cast<Item *>(src)));
  static_cast<Item *>(src)->~Item();
  return OBJC3_SERVICE_OK;
}
void DestroyItem(void *item) {
  static_cast<Item *>(item)->~Item();
  ++item_destroyed;
  if (reenter_destroy && destroy_reentry_map) {
    std::uint64_t count = 0;
    destroy_reentry_status = objc3_generic_map_count(destroy_reentry_map,
        &count);
  }
}

objc3_value_ops I64Ops(objc3_service_status (*copy)(void *, const void *) = CopyI64) {
  return {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
      sizeof(std::int64_t), alignof(std::int64_t), copy, MoveI64, DestroyI64};
}
objc3_value_ops ItemOps() {
  return {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
      sizeof(Item), alignof(Item), CopyItem, MoveItem, DestroyItem};
}
objc3_generic_map_descriptor MapDescriptor(
    objc3_value_ops key_ops = I64Ops(), objc3_value_ops value_ops = I64Ops()) {
  return {sizeof(objc3_generic_map_descriptor),
      OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION, key_ops, value_ops,
      HashI64, EqualI64};
}
objc3_generic_set_descriptor SetDescriptor() {
  return {sizeof(objc3_generic_set_descriptor),
      OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION, I64Ops(), HashI64, EqualI64};
}
}  // namespace

int main() {
  auto descriptor = MapDescriptor();
  objc3_generic_map_t map = nullptr;
  assert(objc3_generic_map_create_empty(&descriptor, &map) ==
         OBJC3_GENERIC_COLLECTION_OK);
  const std::int64_t zero = 0, value_zero = 0, one = 1, seven = 7;
  int inserted = -1;
  assert(objc3_generic_map_insert(map, &zero, &value_zero, &inserted) ==
         OBJC3_GENERIC_COLLECTION_OK && inserted == 1);
  assert(objc3_generic_map_insert(map, &one, &seven, &inserted) ==
         OBJC3_GENERIC_COLLECTION_OK && inserted == 1);
  const std::int64_t nine = 9;
  assert(objc3_generic_map_insert(map, &zero, &nine, &inserted) ==
         OBJC3_GENERIC_COLLECTION_OK && inserted == 0);
  std::int64_t value = -1;
  int has_value = -1;
  assert(objc3_generic_map_lookup(map, &zero, &has_value, &value) ==
         OBJC3_GENERIC_COLLECTION_OK && has_value == 1 && value == 9);
  const std::int64_t missing = 100;
  value = 444;
  assert(objc3_generic_map_lookup(map, &missing, &has_value, &value) ==
         OBJC3_GENERIC_COLLECTION_OK && has_value == 0 && value == 444);
  std::uint64_t count = 0;
  assert(objc3_generic_map_count(map, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 2);

  auto noncopy_descriptor = MapDescriptor(I64Ops(), I64Ops(nullptr));
  objc3_generic_map_t noncopy_map = nullptr;
  assert(objc3_generic_map_create_empty(&noncopy_descriptor, &noncopy_map) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_insert(noncopy_map, &zero, &value_zero, &inserted) ==
         OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION && inserted == 0);
  assert(objc3_generic_map_lookup(noncopy_map, &zero, &has_value, &value) ==
         OBJC3_GENERIC_COLLECTION_OK && has_value == 0);
  assert(objc3_generic_map_release(noncopy_map) == OBJC3_GENERIC_COLLECTION_OK);

  // The key/value spans have independent descriptors and independent stride.
  const std::int64_t keys[] = {2, 2, 0};
  const Item values[] = {{"first", false}, {"replacement", false}, {"zero", false}};
  auto aggregate_descriptor = MapDescriptor(I64Ops(), ItemOps());
  objc3_generic_map_t aggregate_map = nullptr;
  assert(objc3_generic_map_create(&aggregate_descriptor, keys, values, 3,
      &aggregate_map) == OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_count(aggregate_map, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 2);
  alignas(Item) unsigned char item_storage[sizeof(Item)];
  std::int64_t key_two = 2;
  assert(objc3_generic_map_lookup(aggregate_map, &key_two, &has_value,
      item_storage) == OBJC3_GENERIC_COLLECTION_OK && has_value == 1 &&
      reinterpret_cast<Item *>(item_storage)->text == "replacement");
  DestroyItem(item_storage);

  // Replacing an owned value destroys the retired value after releasing the
  // map lock. Its destructor safely reenters the same map service.
  Item replacement{"replacement-2", false};
  destroy_reentry_map = aggregate_map;
  reenter_destroy = true;
  assert(objc3_generic_map_insert(aggregate_map, &key_two, &replacement,
      &inserted) == OBJC3_GENERIC_COLLECTION_OK && inserted == 0);
  reenter_destroy = false;
  destroy_reentry_map = nullptr;
  assert(destroy_reentry_status == OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_lookup(aggregate_map, &key_two, &has_value,
      item_storage) == OBJC3_GENERIC_COLLECTION_OK && has_value == 1 &&
      reinterpret_cast<Item *>(item_storage)->text == "replacement-2");
  DestroyItem(item_storage);
  Item allocation_failure{"out-of-memory", false, true};
  assert(objc3_generic_map_insert(aggregate_map, &key_two,
      &allocation_failure, &inserted) ==
      OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY && inserted == 0);
  assert(objc3_generic_map_lookup(aggregate_map, &key_two, &has_value,
      item_storage) == OBJC3_GENERIC_COLLECTION_OK && has_value == 1 &&
      reinterpret_cast<Item *>(item_storage)->text == "replacement-2");
  DestroyItem(item_storage);
  Item copy_failure{"unavailable", true};
  assert(objc3_generic_map_insert(aggregate_map, &key_two, &copy_failure,
      &inserted) == OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT && inserted == 0);
  assert(objc3_generic_map_count(aggregate_map, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 2);

  // Insertion order is stable and replacing an existing key does not move it.
  objc3_generic_associative_iterator_t iterator = nullptr;
  assert(objc3_generic_map_iterator(map, &iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  std::int64_t read_key = -1, read_value = -1;
  assert(objc3_generic_associative_iterator_next(iterator, &has_value,
      &read_key, &read_value) == OBJC3_GENERIC_COLLECTION_OK &&
      has_value == 1 && read_key == 0 && read_value == 9);
  assert(objc3_generic_associative_iterator_next(iterator, &has_value,
      &read_key, &read_value) == OBJC3_GENERIC_COLLECTION_OK &&
      has_value == 1 && read_key == 1 && read_value == 7);
  assert(objc3_generic_associative_iterator_next(iterator, &has_value,
      &read_key, &read_value) == OBJC3_GENERIC_COLLECTION_OK && has_value == 0);
  assert(objc3_generic_associative_iterator_release(iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);

  objc3_generic_map_t invalidated_map = nullptr;
  assert(objc3_generic_map_create_empty(&descriptor, &invalidated_map) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_insert(invalidated_map, &zero, &value_zero,
      &inserted) == OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_iterator(invalidated_map, &iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_insert(invalidated_map, &one, &seven, &inserted) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_associative_iterator_next(iterator, &has_value,
      &read_key, &read_value) ==
      OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION && has_value == 0);
  assert(objc3_generic_associative_iterator_release(iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_release(invalidated_map) ==
         OBJC3_GENERIC_COLLECTION_OK);

  int removed = -1;
  assert(objc3_generic_map_remove(map, &missing, &removed) ==
         OBJC3_GENERIC_COLLECTION_OK && removed == 0);
  assert(objc3_generic_map_remove(map, &zero, &removed) ==
         OBJC3_GENERIC_COLLECTION_OK && removed == 1);
  assert(objc3_generic_map_count(map, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 1);

  // Equal keys with incoherent hashes are diagnosed transactionally.
  auto incoherent_descriptor = MapDescriptor();
  objc3_generic_map_t incoherent = nullptr;
  assert(objc3_generic_map_create_empty(&incoherent_descriptor, &incoherent) ==
         OBJC3_GENERIC_COLLECTION_OK);
  equal_all_keys = true;
  assert(objc3_generic_map_insert(incoherent, &zero, &value_zero, &inserted) ==
         OBJC3_GENERIC_COLLECTION_OK && inserted == 1);
  assert(objc3_generic_map_insert(incoherent, &one, &seven, &inserted) ==
         OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT && inserted == 0);
  equal_all_keys = false;
  assert(objc3_generic_map_count(incoherent, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 1);

  // A hashing callback may reenter and commit a different insertion. The
  // prepared outer insertion loses its generation check and leaves that
  // reentrant commit intact.
  objc3_generic_map_t reentrant = nullptr;
  assert(objc3_generic_map_create_empty(&descriptor, &reentrant) ==
         OBJC3_GENERIC_COLLECTION_OK);
  reentrant_map = reentrant;
  reenter_hash = true;
  const std::int64_t outer_key = 2, outer_value = 20;
  assert(objc3_generic_map_insert(reentrant, &outer_key, &outer_value,
      &inserted) == OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION &&
      inserted == 0);
  reentrant_map = nullptr;
  assert(objc3_generic_map_count(reentrant, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 1);
  const std::int64_t nested_key = 3;
  assert(objc3_generic_map_lookup(reentrant, &nested_key, &has_value, &value) ==
         OBJC3_GENERIC_COLLECTION_OK && has_value == 1 && value == 30);

  const std::int64_t set_values[] = {0, 1, 0};
  auto set_descriptor = SetDescriptor();
  objc3_generic_set_t set = nullptr;
  assert(objc3_generic_set_create(&set_descriptor, set_values, 3, &set) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_set_count(set, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 2);
  int contains = -1;
  assert(objc3_generic_set_contains(set, &zero, &contains) ==
         OBJC3_GENERIC_COLLECTION_OK && contains == 1);
  objc3_generic_associative_iterator_t set_iterator = nullptr;
  assert(objc3_generic_set_iterator(set, &set_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_set_insert(set, &zero, &inserted) ==
         OBJC3_GENERIC_COLLECTION_OK && inserted == 0);
  assert(objc3_generic_associative_iterator_next(set_iterator, &has_value,
      &read_key, nullptr) == OBJC3_GENERIC_COLLECTION_OK && has_value == 1 &&
      read_key == 0);
  assert(objc3_generic_associative_iterator_release(set_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_set_remove(set, &one, &removed) ==
         OBJC3_GENERIC_COLLECTION_OK && removed == 1);
  assert(objc3_generic_set_count(set, &count) ==
         OBJC3_GENERIC_COLLECTION_OK && count == 1);
  set_iterator = nullptr;
  assert(objc3_generic_set_iterator(set, &set_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_associative_iterator_next(set_iterator, &has_value,
      &read_key, nullptr) == OBJC3_GENERIC_COLLECTION_OK && has_value == 1 &&
      read_key == 0);
  assert(objc3_generic_associative_iterator_release(set_iterator) ==
         OBJC3_GENERIC_COLLECTION_OK);

  assert(objc3_generic_set_release(set) == OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_release(reentrant) == OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_release(incoherent) == OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_release(aggregate_map) ==
         OBJC3_GENERIC_COLLECTION_OK);
  assert(objc3_generic_map_release(map) == OBJC3_GENERIC_COLLECTION_OK);
  return 0;
}
