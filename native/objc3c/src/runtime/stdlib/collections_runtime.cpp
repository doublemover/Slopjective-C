#include "runtime/stdlib/collections_runtime_contract.h"

#include "runtime/stdlib/stdlib_runtime_storage.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <mutex>
#include <utility>
#include <vector>

namespace {

namespace storage = objc3c::runtime::stdlib_runtime;

struct MapEntry {
  int key = 0;
  int value = 0;
};

struct CollectionRecord {
  storage::RecordHeader header;
  std::vector<int> values;
  std::vector<MapEntry> entries;
  int array_handle = 0;
  storage::DescriptorKind array_handle_kind = storage::DescriptorKind::Unknown;
  std::uint64_t expected_array_mutation_generation = 0;
  int start = 0;
  int count = 0;
  int iterator_position = 0;
  int iterator_source_handle = 0;
  storage::DescriptorKind iterator_source_kind =
      storage::DescriptorKind::Unknown;
  std::uint64_t expected_mutation_generation = 0;
};

struct RuntimeStdlibCollectionsState {
  std::mutex mutex;
  storage::HandleTable<CollectionRecord> records{
      storage::HandleTableOwner::Collections};
  std::uint64_t total_call_count = 0;
  std::uint64_t array_create_call_count = 0;
  std::uint64_t array_query_call_count = 0;
  std::uint64_t map_create_call_count = 0;
  std::uint64_t map_query_call_count = 0;
  std::uint64_t map_mutation_call_count = 0;
  std::uint64_t set_create_call_count = 0;
  std::uint64_t set_query_call_count = 0;
  std::uint64_t set_mutation_call_count = 0;
  std::uint64_t slice_create_call_count = 0;
  std::uint64_t slice_query_call_count = 0;
  std::uint64_t iterator_create_call_count = 0;
  std::uint64_t iterator_query_call_count = 0;
  std::uint64_t status_call_count = 0;
  std::uint64_t mutation_generation = 0;
  std::uint64_t invalid_handle_failure_count = 0;
  std::uint64_t cross_kind_handle_failure_count = 0;
  std::uint64_t stale_handle_failure_count = 0;
  std::uint64_t malformed_descriptor_failure_count = 0;
  std::uint64_t capacity_failure_count = 0;
  std::uint64_t iterator_invalidation_count = 0;
  int last_handle = 0;
  int last_input_a = 0;
  int last_input_b = 0;
  int last_input_c = 0;
  int last_status = OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
  int last_result = 0;
};

RuntimeStdlibCollectionsState &State() {
  static RuntimeStdlibCollectionsState state;
  return state;
}

void RecordStatusCounter(RuntimeStdlibCollectionsState &state, int status) {
  switch (status) {
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE:
      ++state.invalid_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CROSS_KIND_HANDLE:
      ++state.cross_kind_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_STALE_HANDLE:
      ++state.stale_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MALFORMED_DESCRIPTOR:
      ++state.malformed_descriptor_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED:
      ++state.capacity_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION:
      ++state.iterator_invalidation_count;
      break;
    default:
      break;
  }
}

void RecordCall(RuntimeStdlibCollectionsState &state,
                std::uint64_t &family_count,
                int handle,
                int input_a,
                int input_b,
                int input_c,
                int status,
                int result) {
  ++state.total_call_count;
  ++family_count;
  state.last_handle = handle;
  state.last_input_a = input_a;
  state.last_input_b = input_b;
  state.last_input_c = input_c;
  state.last_status = status;
  state.last_result = result;
  RecordStatusCounter(state, status);
}

int StatusForLookup(storage::LookupStatus status) {
  switch (status) {
    case storage::LookupStatus::Ok:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
    case storage::LookupStatus::CrossKind:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CROSS_KIND_HANDLE;
    case storage::LookupStatus::StaleHandle:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_STALE_HANDLE;
    case storage::LookupStatus::InvalidHandle:
    default:
      return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE;
  }
}

CollectionRecord MakeArrayRecord(const int *values,
                                 int count,
                                 storage::DescriptorKind kind) {
  CollectionRecord record;
  record.values.reserve(static_cast<std::size_t>(count));
  for (int index = 0; index < count; ++index) {
    record.values.push_back(values[index]);
  }
  record.count = count;
  record.header.descriptor_kind = kind;
  return record;
}

MapEntry *FindMapEntry(CollectionRecord &record, int key) {
  auto iterator = std::find_if(record.entries.begin(), record.entries.end(),
                               [key](const MapEntry &entry) {
                                 return entry.key == key;
                               });
  if (iterator == record.entries.end()) {
    return nullptr;
  }
  return &(*iterator);
}

bool ContainsValue(const std::vector<int> &values, int value) {
  return std::find(values.begin(), values.end(), value) != values.end();
}

bool AppendUniqueValue(std::vector<int> &values, int value) {
  if (ContainsValue(values, value)) {
    return false;
  }
  values.push_back(value);
  return true;
}

bool SumArrayFitsInt(const CollectionRecord &record, int *out) {
  int sum = 0;
  for (int value : record.values) {
    if (storage::AddWouldOverflowInt(sum, value)) {
      return false;
    }
    sum += value;
  }
  *out = sum;
  return true;
}

int ValidateSourceMutation(RuntimeStdlibCollectionsState &state,
                           int handle,
                           storage::DescriptorKind kind,
                           std::uint64_t expected_generation) {
  auto lookup = state.records.Lookup(handle, {kind});
  if (lookup.status != storage::LookupStatus::Ok) {
    return StatusForLookup(lookup.status);
  }
  if (lookup.record->header.mutation_generation != expected_generation) {
    return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION;
  }
  return OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
}

std::vector<int> SliceValues(const CollectionRecord &record,
                             int start,
                             int count) {
  std::vector<int> values;
  values.reserve(static_cast<std::size_t>(count));
  for (int index = 0; index < count; ++index) {
    values.push_back(record.values[static_cast<std::size_t>(start + index)]);
  }
  return values;
}

int ArrayRecordCount(const RuntimeStdlibCollectionsState &state) {
  return state.records.RecordCount(
             storage::DescriptorKind::CollectionImmutableArray) +
         state.records.RecordCount(
             storage::DescriptorKind::CollectionMutableArray);
}

}  // namespace

namespace objc3c::runtime {

void ResetRuntimeStdlibCollectionsStateForTesting() {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.total_call_count = 0;
  state.array_create_call_count = 0;
  state.array_query_call_count = 0;
  state.map_create_call_count = 0;
  state.map_query_call_count = 0;
  state.map_mutation_call_count = 0;
  state.set_create_call_count = 0;
  state.set_query_call_count = 0;
  state.set_mutation_call_count = 0;
  state.slice_create_call_count = 0;
  state.slice_query_call_count = 0;
  state.iterator_create_call_count = 0;
  state.iterator_query_call_count = 0;
  state.status_call_count = 0;
  state.mutation_generation = 0;
  state.invalid_handle_failure_count = 0;
  state.cross_kind_handle_failure_count = 0;
  state.stale_handle_failure_count = 0;
  state.malformed_descriptor_failure_count = 0;
  state.capacity_failure_count = 0;
  state.iterator_invalidation_count = 0;
  state.last_handle = 0;
  state.last_input_a = 0;
  state.last_input_b = 0;
  state.last_input_c = 0;
  state.last_status = OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
  state.last_result = 0;
  state.records.Reset();
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_stdlib_collections_array3_i32(int first,
                                                           int second,
                                                           int third,
                                                           int count) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (count < 0 || count > 3) {
    RecordCall(state, state.array_create_call_count, 0, first, second, third,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_COUNT, 0);
    return 0;
  }
  const std::array<int, 3> inputs = {first, second, third};
  const int handle = state.records.Store(
      storage::DescriptorKind::CollectionImmutableArray,
      MakeArrayRecord(inputs.data(), count,
                      storage::DescriptorKind::CollectionImmutableArray));
  if (handle == 0) {
    RecordCall(state, state.array_create_call_count, 0, first, second, third,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.array_create_call_count, handle, first, second,
             third, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_array_storage_i32(
    const int *values,
    int count) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (count < 0) {
    RecordCall(state, state.array_create_call_count, 0, count, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_COUNT, 0);
    return 0;
  }
  if (storage::CountExceedsStorageCapacity(count)) {
    RecordCall(state, state.array_create_call_count, 0, count, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  if (values == nullptr && count != 0) {
    RecordCall(state, state.array_create_call_count, 0, count, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MALFORMED_DESCRIPTOR,
               0);
    return 0;
  }
  const int handle = state.records.Store(
      storage::DescriptorKind::CollectionImmutableArray,
      MakeArrayRecord(values, count,
                      storage::DescriptorKind::CollectionImmutableArray));
  if (handle == 0) {
    RecordCall(state, state.array_create_call_count, 0, count, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.array_create_call_count, handle, count, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_mutable_array_i32(void) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  CollectionRecord record;
  const int handle = state.records.Store(
      storage::DescriptorKind::CollectionMutableArray, std::move(record));
  if (handle == 0) {
    RecordCall(state, state.array_create_call_count, 0, 0, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.array_create_call_count, handle, 0, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_mutable_array_append_i32(
    int handle,
    int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.array_create_call_count, handle, value, 0, 0,
               status, 0);
    return 0;
  }
  if (lookup.record->values.size() >=
      static_cast<std::size_t>(storage::kMaxStorageElements)) {
    RecordCall(state, state.array_create_call_count, handle, value, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  lookup.record->values.push_back(value);
  lookup.record->count = static_cast<int>(lookup.record->values.size());
  ++state.mutation_generation;
  lookup.record->header.mutation_generation = state.mutation_generation;
  RecordCall(state, state.array_create_call_count, handle, value, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             lookup.record->count);
  return lookup.record->count;
}

extern "C" int objc3_runtime_stdlib_collections_mutable_array_set_i32(
    int handle,
    int index,
    int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.array_create_call_count, handle, index, value, 0,
               status, 0);
    return 0;
  }
  if (index < 0 || index >= static_cast<int>(lookup.record->values.size())) {
    RecordCall(state, state.array_create_call_count, handle, index, value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS, 0);
    return 0;
  }
  lookup.record->values[static_cast<std::size_t>(index)] = value;
  ++state.mutation_generation;
  lookup.record->header.mutation_generation = state.mutation_generation;
  RecordCall(state, state.array_create_call_count, handle, index, value, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, value);
  return value;
}

extern "C" int objc3_runtime_stdlib_collections_mutable_array_remove_at_i32(
    int handle,
    int index) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.array_create_call_count, handle, index, 0, 0,
               status, 0);
    return 0;
  }
  if (index < 0 || index >= static_cast<int>(lookup.record->values.size())) {
    RecordCall(state, state.array_create_call_count, handle, index, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS, 0);
    return 0;
  }
  lookup.record->values.erase(lookup.record->values.begin() + index);
  lookup.record->count = static_cast<int>(lookup.record->values.size());
  ++state.mutation_generation;
  lookup.record->header.mutation_generation = state.mutation_generation;
  RecordCall(state, state.array_create_call_count, handle, index, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             lookup.record->count);
  return lookup.record->count;
}

extern "C" int objc3_runtime_stdlib_collections_array_count_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionImmutableArray,
               storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  const int result = static_cast<int>(lookup.record->values.size());
  RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_array_get_or_i32(
    int handle,
    int index,
    int default_value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionImmutableArray,
               storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.array_query_call_count, handle, index,
               default_value, 0, status, default_value);
    return default_value;
  }
  if (index < 0 || index >= static_cast<int>(lookup.record->values.size())) {
    RecordCall(state, state.array_query_call_count, handle, index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS,
               default_value);
    return default_value;
  }
  const int result = lookup.record->values[static_cast<std::size_t>(index)];
  RecordCall(state, state.array_query_call_count, handle, index, default_value,
             0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_array_prefix_count_i32(
    int handle,
    int requested) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionImmutableArray,
               storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.array_query_call_count, handle, requested, 0, 0,
               status, 0);
    return 0;
  }
  const int result = std::min(static_cast<int>(lookup.record->values.size()),
                              std::max(requested, 0));
  RecordCall(state, state.array_query_call_count, handle, requested, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_array_sum_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionImmutableArray,
               storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  int result = 0;
  if (!SumArrayFitsInt(*lookup.record, &result)) {
    RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OVERFLOW, 0);
    return 0;
  }
  RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_array_slice_i32(int handle,
                                                                int start,
                                                                int count) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionImmutableArray,
               storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.slice_create_call_count, handle, start, count, 0,
               status, 0);
    return 0;
  }
  const int array_count = static_cast<int>(lookup.record->values.size());
  if (start < 0 || count < 0 || start > array_count ||
      count > array_count - start) {
    RecordCall(state, state.slice_create_call_count, handle, start, count, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_RANGE, 0);
    return 0;
  }
  CollectionRecord slice;
  slice.array_handle = handle;
  slice.array_handle_kind = lookup.record->header.descriptor_kind;
  slice.expected_array_mutation_generation =
      lookup.record->header.mutation_generation;
  slice.start = start;
  slice.count = count;
  const int slice_handle = state.records.Store(
      storage::DescriptorKind::CollectionSlice, std::move(slice));
  if (slice_handle == 0) {
    RecordCall(state, state.slice_create_call_count, handle, start, count, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.slice_create_call_count, slice_handle, start, count,
             0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, slice_handle);
  return slice_handle;
}

extern "C" int objc3_runtime_stdlib_collections_array_iterator_i32(
    int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionImmutableArray,
               storage::DescriptorKind::CollectionMutableArray});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  CollectionRecord iterator;
  iterator.values = lookup.record->values;
  iterator.iterator_source_handle = handle;
  iterator.iterator_source_kind = lookup.record->header.descriptor_kind;
  iterator.expected_mutation_generation =
      lookup.record->header.mutation_generation;
  const int iterator_handle = state.records.Store(
      storage::DescriptorKind::CollectionIterator, std::move(iterator));
  if (iterator_handle == 0) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.iterator_create_call_count, iterator_handle, handle,
             0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             iterator_handle);
  return iterator_handle;
}

extern "C" int objc3_runtime_stdlib_collections_map_entry_i32(int key,
                                                              int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  CollectionRecord record;
  record.entries.push_back(MapEntry{key, value});
  const int handle = state.records.Store(
      storage::DescriptorKind::CollectionMap, std::move(record));
  if (handle == 0) {
    RecordCall(state, state.map_create_call_count, 0, key, value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.map_create_call_count, handle, key, value, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_map_empty_i32(void) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  CollectionRecord record;
  const int handle = state.records.Store(
      storage::DescriptorKind::CollectionMap, std::move(record));
  if (handle == 0) {
    RecordCall(state, state.map_create_call_count, 0, 0, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.map_create_call_count, handle, 0, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_map_count_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionMap});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.map_query_call_count, handle, handle, 0, 0, status,
               0);
    return 0;
  }
  const int result = static_cast<int>(lookup.record->entries.size());
  RecordCall(state, state.map_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_map_contains_i32(int handle,
                                                                 int key) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionMap});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.map_query_call_count, handle, key, 0, 0, status,
               0);
    return 0;
  }
  const bool found = FindMapEntry(*lookup.record, key) != nullptr;
  RecordCall(state, state.map_query_call_count, handle, key, 0, 0,
             found ? OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK
                   : OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND,
             found ? 1 : 0);
  return found ? 1 : 0;
}

extern "C" int objc3_runtime_stdlib_collections_map_insert_i32(int handle,
                                                               int key,
                                                               int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionMap});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.map_mutation_call_count, handle, key, value, 0,
               status, 0);
    return 0;
  }
  MapEntry *entry = FindMapEntry(*lookup.record, key);
  bool mutated = false;
  if (entry == nullptr) {
    if (lookup.record->entries.size() >=
        static_cast<std::size_t>(storage::kMaxStorageElements)) {
      RecordCall(state, state.map_mutation_call_count, handle, key, value, 0,
                 OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
      return 0;
    }
    lookup.record->entries.push_back(MapEntry{key, value});
    mutated = true;
  } else if (entry->value != value) {
    entry->value = value;
    mutated = true;
  }
  if (mutated) {
    ++state.mutation_generation;
    lookup.record->header.mutation_generation = state.mutation_generation;
  }
  const int result = static_cast<int>(lookup.record->entries.size());
  RecordCall(state, state.map_mutation_call_count, handle, key, value, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_map_delete_i32(int handle,
                                                               int key) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionMap});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.map_mutation_call_count, handle, key, 0, 0,
               status, 0);
    return 0;
  }
  auto iterator = std::find_if(lookup.record->entries.begin(),
                               lookup.record->entries.end(),
                               [key](const MapEntry &entry) {
                                 return entry.key == key;
                               });
  if (iterator == lookup.record->entries.end()) {
    RecordCall(state, state.map_mutation_call_count, handle, key, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND, 0);
    return 0;
  }
  lookup.record->entries.erase(iterator);
  ++state.mutation_generation;
  lookup.record->header.mutation_generation = state.mutation_generation;
  const int result = static_cast<int>(lookup.record->entries.size());
  RecordCall(state, state.map_mutation_call_count, handle, key, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_map_lookup_or_i32(
    int handle,
    int key,
    int default_value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionMap});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.map_query_call_count, handle, key, default_value, 0,
               status, default_value);
    return default_value;
  }
  MapEntry *entry = FindMapEntry(*lookup.record, key);
  if (entry == nullptr) {
    RecordCall(state, state.map_query_call_count, handle, key, default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND,
               default_value);
    return default_value;
  }
  RecordCall(state, state.map_query_call_count, handle, key, default_value, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, entry->value);
  return entry->value;
}

extern "C" int objc3_runtime_stdlib_collections_map_key_iterator_i32(
    int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionMap});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  CollectionRecord iterator;
  iterator.values.reserve(lookup.record->entries.size());
  for (const MapEntry &entry : lookup.record->entries) {
    iterator.values.push_back(entry.key);
  }
  iterator.iterator_source_handle = handle;
  iterator.iterator_source_kind = storage::DescriptorKind::CollectionMap;
  iterator.expected_mutation_generation =
      lookup.record->header.mutation_generation;
  const int iterator_handle = state.records.Store(
      storage::DescriptorKind::CollectionIterator, std::move(iterator));
  if (iterator_handle == 0) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.iterator_create_call_count, iterator_handle, handle,
             0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             iterator_handle);
  return iterator_handle;
}

extern "C" int objc3_runtime_stdlib_collections_map_value_iterator_i32(
    int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionMap});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  CollectionRecord iterator;
  iterator.values.reserve(lookup.record->entries.size());
  for (const MapEntry &entry : lookup.record->entries) {
    iterator.values.push_back(entry.value);
  }
  iterator.iterator_source_handle = handle;
  iterator.iterator_source_kind = storage::DescriptorKind::CollectionMap;
  iterator.expected_mutation_generation =
      lookup.record->header.mutation_generation;
  const int iterator_handle = state.records.Store(
      storage::DescriptorKind::CollectionIterator, std::move(iterator));
  if (iterator_handle == 0) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.iterator_create_call_count, iterator_handle, handle,
             0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             iterator_handle);
  return iterator_handle;
}

extern "C" int objc3_runtime_stdlib_collections_set3_i32(int first,
                                                         int second,
                                                         int third,
                                                         int count) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (count < 0 || count > 3) {
    RecordCall(state, state.set_create_call_count, 0, first, second, third,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_COUNT, 0);
    return 0;
  }
  CollectionRecord record;
  const std::array<int, 3> inputs = {first, second, third};
  for (int index = 0; index < count; ++index) {
    AppendUniqueValue(record.values, inputs[static_cast<std::size_t>(index)]);
  }
  record.count = static_cast<int>(record.values.size());
  const int handle = state.records.Store(
      storage::DescriptorKind::CollectionSet, std::move(record));
  if (handle == 0) {
    RecordCall(state, state.set_create_call_count, 0, first, second, third,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.set_create_call_count, handle, first, second, third,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_set_storage_i32(
    const int *values,
    int count) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (count < 0) {
    RecordCall(state, state.set_create_call_count, 0, count, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_COUNT, 0);
    return 0;
  }
  if (storage::CountExceedsStorageCapacity(count)) {
    RecordCall(state, state.set_create_call_count, 0, count, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  if (values == nullptr && count != 0) {
    RecordCall(state, state.set_create_call_count, 0, count, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MALFORMED_DESCRIPTOR,
               0);
    return 0;
  }
  CollectionRecord record;
  for (int index = 0; index < count; ++index) {
    AppendUniqueValue(record.values, values[index]);
  }
  record.count = static_cast<int>(record.values.size());
  const int handle = state.records.Store(
      storage::DescriptorKind::CollectionSet, std::move(record));
  if (handle == 0) {
    RecordCall(state, state.set_create_call_count, 0, count, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.set_create_call_count, handle, count, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_set_count_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionSet});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.set_query_call_count, handle, handle, 0, 0, status,
               0);
    return 0;
  }
  const int result = static_cast<int>(lookup.record->values.size());
  RecordCall(state, state.set_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_set_contains_i32(int handle,
                                                                 int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionSet});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.set_query_call_count, handle, value, 0, 0, status,
               0);
    return 0;
  }
  const bool found = ContainsValue(lookup.record->values, value);
  RecordCall(state, state.set_query_call_count, handle, value, 0, 0,
             found ? OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK
                   : OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND,
             found ? 1 : 0);
  return found ? 1 : 0;
}

extern "C" int objc3_runtime_stdlib_collections_set_insert_i32(int handle,
                                                               int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionSet});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.set_mutation_call_count, handle, value, 0, 0,
               status, 0);
    return 0;
  }
  if (lookup.record->values.size() >=
      static_cast<std::size_t>(storage::kMaxStorageElements)) {
    RecordCall(state, state.set_mutation_call_count, handle, value, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  const bool inserted = AppendUniqueValue(lookup.record->values, value);
  if (inserted) {
    lookup.record->count = static_cast<int>(lookup.record->values.size());
    ++state.mutation_generation;
    lookup.record->header.mutation_generation = state.mutation_generation;
  }
  const int result = static_cast<int>(lookup.record->values.size());
  RecordCall(state, state.set_mutation_call_count, handle, value, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_set_delete_i32(int handle,
                                                               int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionSet});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.set_mutation_call_count, handle, value, 0, 0,
               status, 0);
    return 0;
  }
  auto iterator = std::find(lookup.record->values.begin(),
                            lookup.record->values.end(), value);
  if (iterator == lookup.record->values.end()) {
    RecordCall(state, state.set_mutation_call_count, handle, value, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND, 0);
    return 0;
  }
  lookup.record->values.erase(iterator);
  lookup.record->count = static_cast<int>(lookup.record->values.size());
  ++state.mutation_generation;
  lookup.record->header.mutation_generation = state.mutation_generation;
  RecordCall(state, state.set_mutation_call_count, handle, value, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             lookup.record->count);
  return lookup.record->count;
}

extern "C" int objc3_runtime_stdlib_collections_slice_count_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionSlice});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.slice_query_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  const int source_status = ValidateSourceMutation(
      state, lookup.record->array_handle, lookup.record->array_handle_kind,
      lookup.record->expected_array_mutation_generation);
  if (source_status != OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK) {
    RecordCall(state, state.slice_query_call_count, handle, handle, 0, 0,
               source_status, 0);
    return 0;
  }
  RecordCall(state, state.slice_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, lookup.record->count);
  return lookup.record->count;
}

extern "C" int objc3_runtime_stdlib_collections_slice_get_or_i32(
    int handle,
    int index,
    int default_value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto slice_lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionSlice});
  if (slice_lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(slice_lookup.status);
    RecordCall(state, state.slice_query_call_count, handle, index,
               default_value, 0, status, default_value);
    return default_value;
  }
  auto array_lookup = state.records.Lookup(
      slice_lookup.record->array_handle,
      {slice_lookup.record->array_handle_kind});
  if (array_lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(array_lookup.status);
    RecordCall(state, state.slice_query_call_count, handle, index,
               default_value, 0, status, default_value);
    return default_value;
  }
  if (array_lookup.record->header.mutation_generation !=
      slice_lookup.record->expected_array_mutation_generation) {
    RecordCall(
        state, state.slice_query_call_count, handle, index, default_value, 0,
        OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION,
        default_value);
    return default_value;
  }
  if (index < 0 || index >= slice_lookup.record->count) {
    RecordCall(state, state.slice_query_call_count, handle, index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS,
               default_value);
    return default_value;
  }
  const int result = array_lookup.record->values[static_cast<std::size_t>(
      slice_lookup.record->start + index)];
  RecordCall(state, state.slice_query_call_count, handle, index, default_value,
             0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_slice_iterator_i32(
    int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto slice_lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionSlice});
  if (slice_lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(slice_lookup.status);
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  auto array_lookup = state.records.Lookup(
      slice_lookup.record->array_handle,
      {slice_lookup.record->array_handle_kind});
  if (array_lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(array_lookup.status);
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  if (array_lookup.record->header.mutation_generation !=
      slice_lookup.record->expected_array_mutation_generation) {
    RecordCall(
        state, state.iterator_create_call_count, handle, handle, 0, 0,
        OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION, 0);
    return 0;
  }
  CollectionRecord iterator;
  iterator.values = SliceValues(*array_lookup.record, slice_lookup.record->start,
                                slice_lookup.record->count);
  iterator.iterator_source_handle = slice_lookup.record->array_handle;
  iterator.iterator_source_kind = slice_lookup.record->array_handle_kind;
  iterator.expected_mutation_generation =
      slice_lookup.record->expected_array_mutation_generation;
  const int iterator_handle = state.records.Store(
      storage::DescriptorKind::CollectionIterator, std::move(iterator));
  if (iterator_handle == 0) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.iterator_create_call_count, iterator_handle, handle,
             0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             iterator_handle);
  return iterator_handle;
}

extern "C" int objc3_runtime_stdlib_collections_set_iterator_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup =
      state.records.Lookup(handle, {storage::DescriptorKind::CollectionSet});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  CollectionRecord iterator;
  iterator.values = lookup.record->values;
  iterator.iterator_source_handle = handle;
  iterator.iterator_source_kind = storage::DescriptorKind::CollectionSet;
  iterator.expected_mutation_generation =
      lookup.record->header.mutation_generation;
  const int iterator_handle = state.records.Store(
      storage::DescriptorKind::CollectionIterator, std::move(iterator));
  if (iterator_handle == 0) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.iterator_create_call_count, iterator_handle, handle,
             0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             iterator_handle);
  return iterator_handle;
}

extern "C" int objc3_runtime_stdlib_collections_iterator_next_or_i32(
    int handle,
    int default_value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::CollectionIterator});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.iterator_query_call_count, handle, default_value,
               0, 0, status, default_value);
    return default_value;
  }
  CollectionRecord &iterator = *lookup.record;
  if (iterator.iterator_source_handle > 0) {
    const int source_status = ValidateSourceMutation(
        state, iterator.iterator_source_handle, iterator.iterator_source_kind,
        iterator.expected_mutation_generation);
    if (source_status != OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK) {
      RecordCall(state, state.iterator_query_call_count, handle, default_value,
                 0, 0, source_status, default_value);
      return default_value;
    }
  }
  if (iterator.iterator_position >=
      static_cast<int>(iterator.values.size())) {
    RecordCall(state, state.iterator_query_call_count, handle, default_value,
               0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_ITERATION_END,
               default_value);
    return default_value;
  }
  const int result =
      iterator.values[static_cast<std::size_t>(iterator.iterator_position)];
  ++iterator.iterator_position;
  RecordCall(state, state.iterator_query_call_count, handle, default_value, 0,
             0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_last_status_i32(void) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ++state.total_call_count;
  ++state.status_call_count;
  return state.last_status;
}

extern "C" int objc3_runtime_copy_stdlib_collections_state_for_testing(
    objc3_runtime_stdlib_collections_snapshot *out) {
  if (out == nullptr) {
    return -1;
  }
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  out->reset_generation = state.records.reset_generation();
  out->total_call_count = state.total_call_count;
  out->array_create_call_count = state.array_create_call_count;
  out->array_query_call_count = state.array_query_call_count;
  out->map_create_call_count = state.map_create_call_count;
  out->map_query_call_count = state.map_query_call_count;
  out->map_mutation_call_count = state.map_mutation_call_count;
  out->set_create_call_count = state.set_create_call_count;
  out->set_query_call_count = state.set_query_call_count;
  out->set_mutation_call_count = state.set_mutation_call_count;
  out->slice_create_call_count = state.slice_create_call_count;
  out->slice_query_call_count = state.slice_query_call_count;
  out->iterator_create_call_count = state.iterator_create_call_count;
  out->iterator_query_call_count = state.iterator_query_call_count;
  out->status_call_count = state.status_call_count;
  out->array_record_count = ArrayRecordCount(state);
  out->map_record_count =
      state.records.RecordCount(storage::DescriptorKind::CollectionMap);
  out->set_record_count =
      state.records.RecordCount(storage::DescriptorKind::CollectionSet);
  out->slice_record_count =
      state.records.RecordCount(storage::DescriptorKind::CollectionSlice);
  out->iterator_record_count =
      state.records.RecordCount(storage::DescriptorKind::CollectionIterator);
  out->last_handle = state.last_handle;
  out->last_input_a = state.last_input_a;
  out->last_input_b = state.last_input_b;
  out->last_input_c = state.last_input_c;
  out->last_status = state.last_status;
  out->last_result = state.last_result;
  out->abi_version = storage::kStdlibRuntimeAbiVersion;
  out->handle_generation = state.records.handle_generation();
  out->mutation_generation = state.mutation_generation;
  out->invalid_handle_failure_count = state.invalid_handle_failure_count;
  out->cross_kind_handle_failure_count =
      state.cross_kind_handle_failure_count;
  out->stale_handle_failure_count = state.stale_handle_failure_count;
  out->malformed_descriptor_failure_count =
      state.malformed_descriptor_failure_count;
  out->capacity_failure_count = state.capacity_failure_count;
  out->iterator_invalidation_count = state.iterator_invalidation_count;
  out->immutable_array_record_count = state.records.RecordCount(
      storage::DescriptorKind::CollectionImmutableArray);
  out->mutable_array_record_count = state.records.RecordCount(
      storage::DescriptorKind::CollectionMutableArray);
  out->descriptor_record_count = state.records.RecordCount(
      storage::DescriptorKind::CollectionDescriptor);
  out->stale_record_count =
      static_cast<int>(state.records.stale_record_count());
  return 0;
}
