#include "runtime/public/objc3_runtime_api.h"

#include <iostream>

namespace {

int Fail(const char *message) {
  std::cerr << "stdlib-foundation-next-runtime-probe: " << message << "\n";
  return 1;
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  const int hello = objc3_runtime_stdlib_text_utf8_literal_i32(5, 5, 1);
  const int wide = objc3_runtime_stdlib_text_utf8_literal_i32(6, 3, 1);
  const int joined = objc3_runtime_stdlib_text_concat_i32(hello, wide);
  if (hello <= 0 || wide <= 0 || joined <= 0) {
    return Fail("text handles were not runtime-owned positive ids");
  }
  if (objc3_runtime_stdlib_text_byte_count_i32(joined) != 11 ||
      objc3_runtime_stdlib_text_unit_count_i32(joined) != 8 ||
      objc3_runtime_stdlib_text_is_valid_utf8_i32(joined) != 1) {
    return Fail("text byte/unit/validity queries drifted");
  }
  if (objc3_runtime_stdlib_text_prefix_units_i32(joined, 4) != 4 ||
      objc3_runtime_stdlib_text_prefix_units_i32(joined, -3) != 0) {
    return Fail("text prefix helper did not clamp requested units");
  }
  if (objc3_runtime_stdlib_text_byte_count_i32(99) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE) {
    return Fail("text invalid handle did not fail closed");
  }
  if (objc3_runtime_stdlib_text_utf8_literal_i32(2, 3, 1) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE) {
    return Fail("text invalid byte/unit shape did not fail closed");
  }
  if (objc3_runtime_stdlib_text_utf8_literal_i32(4, 4, 0) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8) {
    return Fail("text malformed UTF-8 flag did not fail closed");
  }
  const int stored = objc3_runtime_stdlib_text_utf8_storage_i32("hi", 2);
  char stored_bytes[2] = {};
  if (stored <= 0 ||
      objc3_runtime_stdlib_text_byte_at_or_i32(stored, 0, 0) != 104 ||
      objc3_runtime_copy_stdlib_text_utf8_bytes_for_testing(stored,
                                                            stored_bytes,
                                                            2) != 2 ||
      stored_bytes[0] != 'h' || stored_bytes[1] != 'i') {
    return Fail("text owned UTF-8 storage retrieval drifted");
  }

  objc3_runtime_stdlib_text_snapshot text_snapshot{};
  if (objc3_runtime_copy_stdlib_text_state_for_testing(&text_snapshot) != 0) {
    return Fail("text snapshot copy failed");
  }
  if (text_snapshot.total_call_count != 17 ||
      text_snapshot.literal_call_count != 4 ||
      text_snapshot.query_call_count != 6 ||
      text_snapshot.concat_call_count != 1 ||
      text_snapshot.storage_create_call_count != 1 ||
      text_snapshot.storage_query_call_count != 2 ||
      text_snapshot.status_call_count != 3 ||
      text_snapshot.text_record_count != 4 ||
      text_snapshot.owned_storage_record_count != 1 ||
      text_snapshot.owned_storage_byte_count != 2) {
    return Fail("text runtime call counters drifted");
  }

  const int array = objc3_runtime_stdlib_collections_array3_i32(4, 5, 6, 3);
  if (array <= 0) {
    return Fail("array handle was not a runtime-owned positive id");
  }
  if (objc3_runtime_stdlib_collections_array_count_i32(array) != 3 ||
      objc3_runtime_stdlib_collections_array_get_or_i32(array, 2, 99) != 6) {
    return Fail("array count/index helpers drifted");
  }
  if (objc3_runtime_stdlib_collections_array_get_or_i32(array, 3, 99) != 99 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS) {
    return Fail("array bounds failure was not explicit");
  }
  if (objc3_runtime_stdlib_collections_array_prefix_count_i32(array, 2) != 2) {
    return Fail("array prefix helper drifted");
  }
  if (objc3_runtime_stdlib_collections_array_sum_i32(array) != 15) {
    return Fail("array sum helper drifted");
  }
  if (objc3_runtime_stdlib_collections_array_sum_i32(99) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE) {
    return Fail("array sum invalid handle did not fail closed");
  }
  const int overflow_array =
      objc3_runtime_stdlib_collections_array3_i32(2147483647, 1, 0, 2);
  if (overflow_array <= 0 ||
      objc3_runtime_stdlib_collections_array_sum_i32(overflow_array) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OVERFLOW) {
    return Fail("array sum overflow did not fail closed");
  }
  if (objc3_runtime_stdlib_collections_array3_i32(1, 2, 3, 4) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_COUNT) {
    return Fail("array invalid count did not fail closed");
  }

  const int array_descriptor = objc3_runtime_stdlib_collections_descriptor_i32(
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_ARRAY,
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32,
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_NONE);
  const int map_descriptor = objc3_runtime_stdlib_collections_descriptor_i32(
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MAP,
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32,
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32);
  const int set_descriptor = objc3_runtime_stdlib_collections_descriptor_i32(
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_SET,
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32,
      OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_NONE);
  if (array_descriptor <= 0 || map_descriptor <= 0 || set_descriptor <= 0) {
    return Fail("collection descriptor handles were not runtime-owned ids");
  }
  if (objc3_runtime_stdlib_collections_descriptor_i32(
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MAP,
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32,
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_NONE) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MALFORMED_DESCRIPTOR) {
    return Fail("malformed map descriptor shape did not fail closed");
  }

  const int described_array =
      objc3_runtime_stdlib_collections_array3_descriptor_i32(
          array_descriptor, 1, 2, 0, 2);
  if (described_array <= 0 ||
      objc3_runtime_stdlib_collections_array_count_i32(described_array) != 2 ||
      objc3_runtime_stdlib_collections_descriptor_matches_i32(
          array_descriptor, described_array) != 1) {
    return Fail("descriptor-backed array identity drifted");
  }
  const int described_map =
      objc3_runtime_stdlib_collections_map_entry_descriptor_i32(
          map_descriptor, 2, 30);
  if (described_map <= 0 ||
      objc3_runtime_stdlib_collections_map_lookup_or_i32(
          described_map, 2, 0) != 30 ||
      objc3_runtime_stdlib_collections_descriptor_matches_i32(
          map_descriptor, described_map) != 1) {
    return Fail("descriptor-backed map identity drifted");
  }
  const int described_set =
      objc3_runtime_stdlib_collections_set3_descriptor_i32(
          set_descriptor, 4, 4, 5, 3);
  if (described_set <= 0 ||
      objc3_runtime_stdlib_collections_set_count_i32(described_set) != 2 ||
      objc3_runtime_stdlib_collections_descriptor_matches_i32(
          set_descriptor, described_set) != 1) {
    return Fail("descriptor-backed set identity drifted");
  }
  if (objc3_runtime_stdlib_collections_array3_descriptor_i32(
          map_descriptor, 1, 2, 3, 3) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_DESCRIPTOR_MISMATCH) {
    return Fail("descriptor-backed array mismatch did not fail closed");
  }
  if (objc3_runtime_stdlib_collections_descriptor_matches_i32(
          map_descriptor, described_array) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_DESCRIPTOR_MISMATCH) {
    return Fail("descriptor query mismatch did not fail closed");
  }

  const int slice = objc3_runtime_stdlib_collections_array_slice_i32(array, 1, 2);
  if (slice <= 0) {
    return Fail("slice handle was not a runtime-owned positive id");
  }
  if (objc3_runtime_stdlib_collections_slice_count_i32(slice) != 2 ||
      objc3_runtime_stdlib_collections_slice_get_or_i32(slice, 1, 99) != 6) {
    return Fail("slice count/index helpers drifted");
  }
  if (objc3_runtime_stdlib_collections_slice_get_or_i32(slice, 2, 99) != 99 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS) {
    return Fail("slice bounds failure was not explicit");
  }
  if (objc3_runtime_stdlib_collections_array_slice_i32(array, 2, 3) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_RANGE) {
    return Fail("slice invalid range did not fail closed");
  }

  const int array_iterator =
      objc3_runtime_stdlib_collections_array_iterator_i32(array);
  if (array_iterator <= 0 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(array_iterator,
                                                           99) != 4 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(array_iterator,
                                                           99) != 5 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(array_iterator,
                                                           99) != 6 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(array_iterator,
                                                           99) != 99 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_ITERATION_END) {
    return Fail("array iterator did not preserve deterministic order");
  }

  const int slice_iterator =
      objc3_runtime_stdlib_collections_slice_iterator_i32(slice);
  if (slice_iterator <= 0 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(slice_iterator,
                                                           99) != 5 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(slice_iterator,
                                                           99) != 6 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(slice_iterator,
                                                           99) != 99) {
    return Fail("slice iterator did not preserve deterministic order");
  }

  const int map = objc3_runtime_stdlib_collections_map_entry_i32(7, 42);
  if (map <= 0) {
    return Fail("map handle was not a runtime-owned positive id");
  }
  if (objc3_runtime_stdlib_collections_map_count_i32(map) != 1 ||
      objc3_runtime_stdlib_collections_map_insert_i32(map, 8, 64) != 2 ||
      objc3_runtime_stdlib_collections_map_count_i32(map) != 2 ||
      objc3_runtime_stdlib_collections_map_contains_i32(map, 7) != 1 ||
      objc3_runtime_stdlib_collections_map_contains_i32(map, 8) != 1 ||
      objc3_runtime_stdlib_collections_map_lookup_or_i32(map, 7, 9) != 42 ||
      objc3_runtime_stdlib_collections_map_lookup_or_i32(map, 8, 9) != 64) {
    return Fail("map hit helpers drifted");
  }
  if (objc3_runtime_stdlib_collections_map_insert_i32(map, 7, 45) != 2 ||
      objc3_runtime_stdlib_collections_map_count_i32(map) != 2 ||
      objc3_runtime_stdlib_collections_map_lookup_or_i32(map, 7, 9) != 45) {
    return Fail("map replacement helpers drifted");
  }
  if (objc3_runtime_stdlib_collections_map_lookup_or_i32(map, 10, 9) != 9 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND) {
    return Fail("map missing-key failure was not explicit");
  }
  if (objc3_runtime_stdlib_collections_map_insert_i32(99, 8, 64) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE) {
    return Fail("map insert invalid handle did not fail closed");
  }
  if (objc3_runtime_stdlib_collections_map_count_i32(99) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE) {
    return Fail("map invalid handle did not fail closed");
  }
  const int map_key_iterator =
      objc3_runtime_stdlib_collections_map_key_iterator_i32(map);
  if (map_key_iterator <= 0 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          map_key_iterator, 99) != 7 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          map_key_iterator, 99) != 8 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          map_key_iterator, 99) != 99) {
    return Fail("map key iterator did not preserve insertion order");
  }
  const int map_value_iterator =
      objc3_runtime_stdlib_collections_map_value_iterator_i32(map);
  if (map_value_iterator <= 0 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          map_value_iterator, 99) != 45 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          map_value_iterator, 99) != 64 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          map_value_iterator, 99) != 99) {
    return Fail("map value iterator did not preserve insertion order");
  }
  const int map_delete_remaining_count =
      objc3_runtime_stdlib_collections_map_delete_i32(map, 8);
  if (map_delete_remaining_count != 1 ||
      objc3_runtime_stdlib_collections_map_contains_i32(map, 8) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND) {
    return Fail("map delete did not remove the requested key");
  }
  if (objc3_runtime_stdlib_collections_map_delete_i32(map, 8) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND) {
    return Fail("map delete missing-key status did not fail closed");
  }
  const int invalidated_map_iterator =
      objc3_runtime_stdlib_collections_map_key_iterator_i32(map);
  if (invalidated_map_iterator <= 0 ||
      objc3_runtime_stdlib_collections_map_insert_i32(map, 9, 81) != 2 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          invalidated_map_iterator, 99) != 99 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION) {
    return Fail("map mutation during iteration did not fail closed");
  }

  const int set = objc3_runtime_stdlib_collections_set3_i32(7, 7, 9, 3);
  if (set <= 0) {
    return Fail("set handle was not a runtime-owned positive id");
  }
  if (objc3_runtime_stdlib_collections_set_count_i32(set) != 2 ||
      objc3_runtime_stdlib_collections_set_contains_i32(set, 9) != 1) {
    return Fail("set count/contains helpers drifted");
  }
  if (objc3_runtime_stdlib_collections_set_contains_i32(set, 8) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND) {
    return Fail("set missing-value failure was not explicit");
  }
  const int set_iterator =
      objc3_runtime_stdlib_collections_set_iterator_i32(set);
  if (set_iterator <= 0 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(set_iterator,
                                                           99) != 7 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(set_iterator,
                                                           99) != 9 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(set_iterator,
                                                           99) != 99 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_ITERATION_END) {
    return Fail("set iterator did not preserve insertion order");
  }
  const int set_delete_remaining_count =
      objc3_runtime_stdlib_collections_set_delete_i32(set, 7);
  if (set_delete_remaining_count != 1 ||
      objc3_runtime_stdlib_collections_set_contains_i32(set, 7) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND) {
    return Fail("set delete did not remove the requested value");
  }
  if (objc3_runtime_stdlib_collections_set_delete_i32(set, 7) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND) {
    return Fail("set delete missing-value status did not fail closed");
  }
  const int set_after_delete_iterator =
      objc3_runtime_stdlib_collections_set_iterator_i32(set);
  if (set_after_delete_iterator <= 0 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          set_after_delete_iterator, 99) != 9 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          set_after_delete_iterator, 99) != 99) {
    return Fail("set iterator did not reflect delete mutation");
  }
  const int invalidated_set_iterator =
      objc3_runtime_stdlib_collections_set_iterator_i32(set);
  if (invalidated_set_iterator <= 0 ||
      objc3_runtime_stdlib_collections_set_insert_i32(set, 11) != 2 ||
      objc3_runtime_stdlib_collections_iterator_next_or_i32(
          invalidated_set_iterator, 99) != 99 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION) {
    return Fail("set mutation during iteration did not fail closed");
  }
  if (objc3_runtime_stdlib_collections_set3_i32(1, 2, 3, 4) != 0 ||
      objc3_runtime_stdlib_collections_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_COUNT) {
    return Fail("set invalid count did not fail closed");
  }

  objc3_runtime_stdlib_collections_snapshot collections_snapshot{};
  if (objc3_runtime_copy_stdlib_collections_state_for_testing(
          &collections_snapshot) != 0) {
    return Fail("collections snapshot copy failed");
  }
  if (collections_snapshot.total_call_count != 107 ||
      collections_snapshot.array_create_call_count != 5 ||
      collections_snapshot.array_query_call_count != 8 ||
      collections_snapshot.map_create_call_count != 2 ||
      collections_snapshot.map_query_call_count != 12 ||
      collections_snapshot.map_mutation_call_count != 6 ||
      collections_snapshot.set_create_call_count != 3 ||
      collections_snapshot.set_query_call_count != 5 ||
      collections_snapshot.set_mutation_call_count != 3 ||
      collections_snapshot.slice_create_call_count != 2 ||
      collections_snapshot.slice_query_call_count != 3 ||
      collections_snapshot.iterator_create_call_count != 8 ||
      collections_snapshot.iterator_query_call_count != 20 ||
      collections_snapshot.descriptor_create_call_count != 4 ||
      collections_snapshot.descriptor_query_call_count != 4 ||
      collections_snapshot.status_call_count != 22 ||
      collections_snapshot.array_record_count != 3 ||
      collections_snapshot.map_record_count != 2 ||
      collections_snapshot.set_record_count != 2 ||
      collections_snapshot.slice_record_count != 1 ||
      collections_snapshot.iterator_record_count != 8 ||
      collections_snapshot.descriptor_record_count != 3 ||
      collections_snapshot.malformed_descriptor_failure_count != 1 ||
      collections_snapshot.descriptor_mismatch_failure_count != 2 ||
      collections_snapshot.iterator_invalidation_count != 2 ||
      collections_snapshot.last_descriptor_status !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_DESCRIPTOR_MISMATCH ||
      collections_snapshot.last_descriptor_actual_kind !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MAP ||
      collections_snapshot.last_descriptor_expected_kind !=
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_ARRAY) {
    return Fail("collections runtime call counters drifted");
  }

  std::cout << "{"
            << "\"text_total_call_count\":"
            << text_snapshot.total_call_count
            << ",\"text_record_count\":" << text_snapshot.text_record_count
            << ",\"collections_total_call_count\":"
            << collections_snapshot.total_call_count
            << ",\"array_record_count\":"
            << collections_snapshot.array_record_count
            << ",\"descriptor_record_count\":"
            << collections_snapshot.descriptor_record_count
            << ",\"descriptor_create_call_count\":"
            << collections_snapshot.descriptor_create_call_count
            << ",\"descriptor_query_call_count\":"
            << collections_snapshot.descriptor_query_call_count
            << ",\"descriptor_mismatch_failure_count\":"
            << collections_snapshot.descriptor_mismatch_failure_count
            << ",\"last_descriptor_status\":"
            << collections_snapshot.last_descriptor_status
            << ",\"last_descriptor_actual_kind\":"
            << collections_snapshot.last_descriptor_actual_kind
            << ",\"last_descriptor_expected_kind\":"
            << collections_snapshot.last_descriptor_expected_kind
            << ",\"map_record_count\":"
            << collections_snapshot.map_record_count
            << ",\"map_mutation_call_count\":"
            << collections_snapshot.map_mutation_call_count
            << ",\"map_delete_remaining_count\":"
            << map_delete_remaining_count
            << ",\"set_record_count\":"
            << collections_snapshot.set_record_count
            << ",\"set_mutation_call_count\":"
            << collections_snapshot.set_mutation_call_count
            << ",\"set_delete_remaining_count\":"
            << set_delete_remaining_count
            << ",\"slice_record_count\":"
            << collections_snapshot.slice_record_count
            << ",\"iterator_record_count\":"
            << collections_snapshot.iterator_record_count
            << ",\"last_collection_status\":"
            << collections_snapshot.last_status << "}\n";
  return 0;
}
