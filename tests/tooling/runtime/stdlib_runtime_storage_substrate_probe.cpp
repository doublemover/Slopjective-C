#include "runtime/public/objc3_runtime_api.h"

#include <iostream>
#include <limits>

namespace {

int Fail(const char *message) {
  std::cerr << "stdlib-runtime-storage-substrate-probe: " << message << "\n";
  return 1;
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  const int stale_text =
      objc3_runtime_stdlib_text_utf8_storage_i32("old", 3);
  const int stale_values[] = {1, 2, 3, 4};
  const int stale_array =
      objc3_runtime_stdlib_collections_array_storage_i32(stale_values, 4);
  if (stale_text <= 0 || stale_array <= 0) {
    return Fail("initial handles were not allocated");
  }

  objc3_runtime_reset_for_testing();
  if (objc3_runtime_stdlib_text_byte_count_i32(stale_text) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STALE_HANDLE) {
    return Fail("text stale handle after reset did not fail closed");
  }
  if (objc3_runtime_stdlib_collections_array_count_i32(stale_array) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_STALE_HANDLE) {
    return Fail("collection stale handle after reset did not fail closed");
  }

  const int text = objc3_runtime_stdlib_text_utf8_storage_i32("abcd", 4);
  const int values[] = {5, 6, 7, 8, 9};
  const int array = objc3_runtime_stdlib_collections_array_storage_i32(
      values, 5);
  if (text <= 0 || array <= 0 ||
      objc3_runtime_stdlib_collections_array_count_i32(array) != 5 ||
      objc3_runtime_stdlib_collections_array_sum_i32(array) != 35) {
    return Fail("arbitrary-length text or array storage drifted");
  }
  if (objc3_runtime_stdlib_text_byte_count_i32(array) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CROSS_KIND_HANDLE) {
    return Fail("collection handle accepted by text runtime");
  }
  if (objc3_runtime_stdlib_collections_array_count_i32(text) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CROSS_KIND_HANDLE) {
    return Fail("text handle accepted by collection runtime");
  }

  if (objc3_runtime_stdlib_text_utf8_storage_i32(nullptr, 2) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_DESCRIPTOR) {
    return Fail("text malformed descriptor did not fail closed");
  }
  if (objc3_runtime_stdlib_text_utf8_storage_i32(
          nullptr, 1048577) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED) {
    return Fail("text capacity failure did not fail closed before allocation");
  }
  if (objc3_runtime_stdlib_collections_array_storage_i32(nullptr, 2) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MALFORMED_DESCRIPTOR) {
    return Fail("collection malformed descriptor did not fail closed");
  }
  if (objc3_runtime_stdlib_collections_array_storage_i32(
          nullptr, 1048577) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED) {
    return Fail("collection capacity failure did not fail closed before allocation");
  }

  const int huge = objc3_runtime_stdlib_text_utf8_literal_i32(
      std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 1);
  const int one = objc3_runtime_stdlib_text_utf8_literal_i32(1, 1, 1);
  if (huge <= 0 || one <= 0 ||
      objc3_runtime_stdlib_text_concat_i32(huge, one) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OVERFLOW) {
    return Fail("text concat overflow did not fail closed");
  }

  const int builder = objc3_runtime_stdlib_text_builder_i32();
  if (builder <= 0 ||
      objc3_runtime_stdlib_text_builder_append_utf8_i32(builder, "ab", 2) !=
          2) {
    return Fail("text builder did not allocate or append storage");
  }
  const int scalar_iterator =
      objc3_runtime_stdlib_text_scalar_iterator_i32(builder);
  if (scalar_iterator <= 0 ||
      objc3_runtime_stdlib_text_builder_append_utf8_i32(builder, "c", 1) != 3 ||
      objc3_runtime_stdlib_text_scalar_iterator_next_or_i32(
          scalar_iterator, 777) != 777 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MUTATED_DURING_ITERATION) {
    return Fail("text iterator mutation generation was not enforced");
  }

  const int mutable_array =
      objc3_runtime_stdlib_collections_mutable_array_i32();
  if (mutable_array <= 0 ||
      objc3_runtime_stdlib_collections_mutable_array_append_i32(
          mutable_array, 10) != 1 ||
      objc3_runtime_stdlib_collections_mutable_array_append_i32(
          mutable_array, 20) != 2) {
    return Fail("mutable array did not allocate or append");
  }
  const int array_iterator =
      objc3_runtime_stdlib_collections_array_iterator_i32(mutable_array);
  if (array_iterator <= 0 ||
      objc3_runtime_stdlib_collections_mutable_array_append_i32(
          mutable_array, 30) != 3 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          array_iterator, 444) != 444 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION) {
    return Fail("mutable array iterator generation was not enforced");
  }

  objc3_runtime_stdlib_text_snapshot text_snapshot{};
  objc3_runtime_stdlib_collections_snapshot collections_snapshot{};
  if (objc3_runtime_copy_stdlib_text_state_for_testing(&text_snapshot) != 0 ||
      objc3_runtime_copy_stdlib_collections_state_for_testing(
          &collections_snapshot) != 0) {
    return Fail("snapshot copy failed");
  }
  if (text_snapshot.abi_version != 1 ||
      text_snapshot.cross_kind_handle_failure_count < 1 ||
      text_snapshot.stale_handle_failure_count < 1 ||
      text_snapshot.malformed_descriptor_failure_count < 1 ||
      text_snapshot.capacity_failure_count < 1 ||
      text_snapshot.iterator_invalidation_count < 1 ||
      text_snapshot.stale_record_count < 1 ||
      text_snapshot.builder_record_count != 1 ||
      text_snapshot.scalar_iterator_record_count != 1) {
    return Fail("text substrate snapshot fields drifted");
  }
  if (collections_snapshot.abi_version != 1 ||
      collections_snapshot.cross_kind_handle_failure_count < 1 ||
      collections_snapshot.stale_handle_failure_count < 1 ||
      collections_snapshot.malformed_descriptor_failure_count < 1 ||
      collections_snapshot.capacity_failure_count < 1 ||
      collections_snapshot.iterator_invalidation_count < 1 ||
      collections_snapshot.stale_record_count < 1 ||
      collections_snapshot.mutation_generation < 1 ||
      collections_snapshot.mutable_array_record_count != 1) {
    return Fail("collections substrate snapshot fields drifted");
  }

  std::cout << "{"
            << "\"text_handle_generation\":"
            << text_snapshot.handle_generation
            << ",\"text_cross_kind_failures\":"
            << text_snapshot.cross_kind_handle_failure_count
            << ",\"text_stale_failures\":"
            << text_snapshot.stale_handle_failure_count
            << ",\"text_stale_record_count\":"
            << text_snapshot.stale_record_count
            << ",\"text_iterator_invalidations\":"
            << text_snapshot.iterator_invalidation_count
            << ",\"collections_handle_generation\":"
            << collections_snapshot.handle_generation
            << ",\"collections_cross_kind_failures\":"
            << collections_snapshot.cross_kind_handle_failure_count
            << ",\"collections_stale_failures\":"
            << collections_snapshot.stale_handle_failure_count
            << ",\"collections_stale_record_count\":"
            << collections_snapshot.stale_record_count
            << ",\"collections_mutation_generation\":"
            << collections_snapshot.mutation_generation
            << ",\"collections_iterator_invalidations\":"
            << collections_snapshot.iterator_invalidation_count
            << "}\n";
  return 0;
}
