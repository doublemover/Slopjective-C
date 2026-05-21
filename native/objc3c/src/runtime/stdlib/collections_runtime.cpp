#include "runtime/stdlib/collections_runtime_contract.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <mutex>
#include <vector>

namespace {

struct ArrayRecord {
  std::array<int, 3> values{};
  int count = 0;
};

struct MapRecord {
  int key = 0;
  int value = 0;
};

struct SetRecord {
  std::vector<int> values;
  int version = 0;
};

struct SliceRecord {
  int array_handle = 0;
  int start = 0;
  int count = 0;
};

struct IteratorRecord {
  std::vector<int> values;
  int position = 0;
  int set_handle = 0;
  int expected_set_version = 0;
};

struct RuntimeStdlibCollectionsState {
  std::mutex mutex;
  std::uint64_t reset_generation = 0;
  std::uint64_t total_call_count = 0;
  std::uint64_t array_create_call_count = 0;
  std::uint64_t array_query_call_count = 0;
  std::uint64_t map_create_call_count = 0;
  std::uint64_t map_query_call_count = 0;
  std::uint64_t set_create_call_count = 0;
  std::uint64_t set_query_call_count = 0;
  std::uint64_t set_mutation_call_count = 0;
  std::uint64_t slice_create_call_count = 0;
  std::uint64_t slice_query_call_count = 0;
  std::uint64_t iterator_create_call_count = 0;
  std::uint64_t iterator_query_call_count = 0;
  std::uint64_t status_call_count = 0;
  std::vector<ArrayRecord> arrays;
  std::vector<MapRecord> maps;
  std::vector<SetRecord> sets;
  std::vector<SliceRecord> slices;
  std::vector<IteratorRecord> iterators;
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
}

ArrayRecord *FindArray(RuntimeStdlibCollectionsState &state, int handle) {
  if (handle <= 0 ||
      handle > static_cast<int>(state.arrays.size())) {
    return nullptr;
  }
  return &state.arrays[static_cast<std::size_t>(handle - 1)];
}

MapRecord *FindMap(RuntimeStdlibCollectionsState &state, int handle) {
  if (handle <= 0 ||
      handle > static_cast<int>(state.maps.size())) {
    return nullptr;
  }
  return &state.maps[static_cast<std::size_t>(handle - 1)];
}

SetRecord *FindSet(RuntimeStdlibCollectionsState &state, int handle) {
  if (handle <= 0 ||
      handle > static_cast<int>(state.sets.size())) {
    return nullptr;
  }
  return &state.sets[static_cast<std::size_t>(handle - 1)];
}

SliceRecord *FindSlice(RuntimeStdlibCollectionsState &state, int handle) {
  if (handle <= 0 ||
      handle > static_cast<int>(state.slices.size())) {
    return nullptr;
  }
  return &state.slices[static_cast<std::size_t>(handle - 1)];
}

IteratorRecord *FindIterator(RuntimeStdlibCollectionsState &state,
                             int handle) {
  if (handle <= 0 ||
      handle > static_cast<int>(state.iterators.size())) {
    return nullptr;
  }
  return &state.iterators[static_cast<std::size_t>(handle - 1)];
}

int ClampToZero(int value) {
  return std::max(value, 0);
}

bool ContainsValue(const std::vector<int> &values, int value) {
  return std::find(values.begin(), values.end(), value) != values.end();
}

void AppendUniqueValue(std::vector<int> &values, int value) {
  if (!ContainsValue(values, value)) {
    values.push_back(value);
  }
}

std::vector<int> SliceValues(const ArrayRecord &record, int start, int count) {
  std::vector<int> values;
  values.reserve(static_cast<std::size_t>(count));
  for (int index = 0; index < count; ++index) {
    values.push_back(record.values[static_cast<std::size_t>(start + index)]);
  }
  return values;
}

bool SumArrayFitsInt(const ArrayRecord &record, int *out) {
  int sum = 0;
  for (int index = 0; index < record.count; ++index) {
    const int value = record.values[static_cast<std::size_t>(index)];
    if ((value > 0 && sum > std::numeric_limits<int>::max() - value) ||
        (value < 0 && sum < std::numeric_limits<int>::min() - value)) {
      return false;
    }
    sum += value;
  }
  *out = sum;
  return true;
}

}  // namespace

namespace objc3c::runtime {

void ResetRuntimeStdlibCollectionsStateForTesting() {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const std::uint64_t next_reset_generation = state.reset_generation + 1;
  state.total_call_count = 0;
  state.array_create_call_count = 0;
  state.array_query_call_count = 0;
  state.map_create_call_count = 0;
  state.map_query_call_count = 0;
  state.set_create_call_count = 0;
  state.set_query_call_count = 0;
  state.set_mutation_call_count = 0;
  state.slice_create_call_count = 0;
  state.slice_query_call_count = 0;
  state.iterator_create_call_count = 0;
  state.iterator_query_call_count = 0;
  state.status_call_count = 0;
  state.arrays.clear();
  state.maps.clear();
  state.sets.clear();
  state.slices.clear();
  state.iterators.clear();
  state.last_handle = 0;
  state.last_input_a = 0;
  state.last_input_b = 0;
  state.last_input_c = 0;
  state.last_status = OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
  state.last_result = 0;
  state.reset_generation = next_reset_generation;
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
  ArrayRecord record{};
  record.values = {first, second, third};
  record.count = count;
  state.arrays.push_back(record);
  const int handle = static_cast<int>(state.arrays.size());
  RecordCall(state, state.array_create_call_count, handle, first, second,
             third, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_array_count_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ArrayRecord *record = FindArray(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, record->count);
  return record->count;
}

extern "C" int objc3_runtime_stdlib_collections_array_get_or_i32(
    int handle,
    int index,
    int default_value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ArrayRecord *record = FindArray(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.array_query_call_count, handle, index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE,
               default_value);
    return default_value;
  }
  if (index < 0 || index >= record->count) {
    RecordCall(state, state.array_query_call_count, handle, index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS,
               default_value);
    return default_value;
  }
  const int result = record->values[static_cast<std::size_t>(index)];
  RecordCall(state, state.array_query_call_count, handle, index, default_value,
             0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_array_prefix_count_i32(
    int handle,
    int requested) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ArrayRecord *record = FindArray(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.array_query_call_count, handle, requested, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  const int result = std::min(record->count, ClampToZero(requested));
  RecordCall(state, state.array_query_call_count, handle, requested, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_array_sum_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ArrayRecord *record = FindArray(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  int result = 0;
  if (!SumArrayFitsInt(*record, &result)) {
    RecordCall(state, state.array_query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_COUNT, 0);
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
  ArrayRecord *record = FindArray(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.slice_create_call_count, handle, start, count, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  if (start < 0 || count < 0 || start > record->count ||
      count > record->count - start) {
    RecordCall(state, state.slice_create_call_count, handle, start, count, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_RANGE, 0);
    return 0;
  }
  state.slices.push_back(SliceRecord{handle, start, count});
  const int slice_handle = static_cast<int>(state.slices.size());
  RecordCall(state, state.slice_create_call_count, slice_handle, start, count,
             0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, slice_handle);
  return slice_handle;
}

extern "C" int objc3_runtime_stdlib_collections_array_iterator_i32(
    int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ArrayRecord *record = FindArray(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  state.iterators.push_back(
      IteratorRecord{SliceValues(*record, 0, record->count), 0, 0, 0});
  const int iterator_handle = static_cast<int>(state.iterators.size());
  RecordCall(state, state.iterator_create_call_count, iterator_handle, handle,
             0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             iterator_handle);
  return iterator_handle;
}

extern "C" int objc3_runtime_stdlib_collections_map_entry_i32(int key,
                                                              int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.maps.push_back(MapRecord{key, value});
  const int handle = static_cast<int>(state.maps.size());
  RecordCall(state, state.map_create_call_count, handle, key, value, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_map_count_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  MapRecord *record = FindMap(state, handle);
  const int result = record == nullptr ? 0 : 1;
  const int status = record == nullptr
                         ? OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE
                         : OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK;
  RecordCall(state, state.map_query_call_count, handle, handle, 0, 0, status,
             result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_map_contains_i32(int handle,
                                                                 int key) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  MapRecord *record = FindMap(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.map_query_call_count, handle, key, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  const bool found = record->key == key;
  RecordCall(state, state.map_query_call_count, handle, key, 0, 0,
             found ? OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK
                   : OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND,
             found ? 1 : 0);
  return found ? 1 : 0;
}

extern "C" int objc3_runtime_stdlib_collections_map_lookup_or_i32(
    int handle,
    int key,
    int default_value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  MapRecord *record = FindMap(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.map_query_call_count, handle, key, default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE,
               default_value);
    return default_value;
  }
  if (record->key != key) {
    RecordCall(state, state.map_query_call_count, handle, key, default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND,
               default_value);
    return default_value;
  }
  RecordCall(state, state.map_query_call_count, handle, key, default_value, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, record->value);
  return record->value;
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
  SetRecord record{};
  const std::array<int, 3> inputs = {first, second, third};
  for (int index = 0; index < count; ++index) {
    AppendUniqueValue(record.values, inputs[static_cast<std::size_t>(index)]);
  }
  state.sets.push_back(record);
  const int handle = static_cast<int>(state.sets.size());
  RecordCall(state, state.set_create_call_count, handle, first, second, third,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_collections_set_count_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  SetRecord *record = FindSet(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.set_query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  const int result = static_cast<int>(record->values.size());
  RecordCall(state, state.set_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_set_contains_i32(int handle,
                                                                 int value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  SetRecord *record = FindSet(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.set_query_call_count, handle, value, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  const bool found = ContainsValue(record->values, value);
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
  SetRecord *record = FindSet(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.set_mutation_call_count, handle, value, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  const int before_count = static_cast<int>(record->values.size());
  AppendUniqueValue(record->values, value);
  if (static_cast<int>(record->values.size()) != before_count) {
    ++record->version;
  }
  const int result = static_cast<int>(record->values.size());
  RecordCall(state, state.set_mutation_call_count, handle, value, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_slice_count_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  SliceRecord *record = FindSlice(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.slice_query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  RecordCall(state, state.slice_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, record->count);
  return record->count;
}

extern "C" int objc3_runtime_stdlib_collections_slice_get_or_i32(
    int handle,
    int index,
    int default_value) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  SliceRecord *slice = FindSlice(state, handle);
  if (slice == nullptr) {
    RecordCall(state, state.slice_query_call_count, handle, index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE,
               default_value);
    return default_value;
  }
  ArrayRecord *array = FindArray(state, slice->array_handle);
  if (array == nullptr) {
    RecordCall(state, state.slice_query_call_count, handle, index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE,
               default_value);
    return default_value;
  }
  if (index < 0 || index >= slice->count) {
    RecordCall(state, state.slice_query_call_count, handle, index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS,
               default_value);
    return default_value;
  }
  const int result =
      array->values[static_cast<std::size_t>(slice->start + index)];
  RecordCall(state, state.slice_query_call_count, handle, index, default_value,
             0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_collections_slice_iterator_i32(
    int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  SliceRecord *slice = FindSlice(state, handle);
  if (slice == nullptr) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  ArrayRecord *array = FindArray(state, slice->array_handle);
  if (array == nullptr) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  state.iterators.push_back(IteratorRecord{
      SliceValues(*array, slice->start, slice->count), 0, 0, 0});
  const int iterator_handle = static_cast<int>(state.iterators.size());
  RecordCall(state, state.iterator_create_call_count, iterator_handle, handle,
             0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK,
             iterator_handle);
  return iterator_handle;
}

extern "C" int objc3_runtime_stdlib_collections_set_iterator_i32(int handle) {
  RuntimeStdlibCollectionsState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  SetRecord *record = FindSet(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.iterator_create_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  state.iterators.push_back(
      IteratorRecord{record->values, 0, handle, record->version});
  const int iterator_handle = static_cast<int>(state.iterators.size());
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
  IteratorRecord *iterator = FindIterator(state, handle);
  if (iterator == nullptr) {
    RecordCall(state, state.iterator_query_call_count, handle, default_value,
               0, 0,
               OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE,
               default_value);
    return default_value;
  }
  if (iterator->set_handle > 0) {
    SetRecord *set = FindSet(state, iterator->set_handle);
    if (set == nullptr) {
      RecordCall(state, state.iterator_query_call_count, handle, default_value,
                 0, 0,
                 OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE,
                 default_value);
      return default_value;
    }
    if (set->version != iterator->expected_set_version) {
      RecordCall(
          state, state.iterator_query_call_count, handle, default_value, 0, 0,
          OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION,
          default_value);
      return default_value;
    }
  }
  if (iterator->position >= static_cast<int>(iterator->values.size())) {
    RecordCall(state, state.iterator_query_call_count, handle, default_value,
               0, 0, OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_ITERATION_END,
               default_value);
    return default_value;
  }
  const int result =
      iterator->values[static_cast<std::size_t>(iterator->position)];
  ++iterator->position;
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
  out->reset_generation = state.reset_generation;
  out->total_call_count = state.total_call_count;
  out->array_create_call_count = state.array_create_call_count;
  out->array_query_call_count = state.array_query_call_count;
  out->map_create_call_count = state.map_create_call_count;
  out->map_query_call_count = state.map_query_call_count;
  out->set_create_call_count = state.set_create_call_count;
  out->set_query_call_count = state.set_query_call_count;
  out->set_mutation_call_count = state.set_mutation_call_count;
  out->slice_create_call_count = state.slice_create_call_count;
  out->slice_query_call_count = state.slice_query_call_count;
  out->iterator_create_call_count = state.iterator_create_call_count;
  out->iterator_query_call_count = state.iterator_query_call_count;
  out->status_call_count = state.status_call_count;
  out->array_record_count = static_cast<int>(state.arrays.size());
  out->map_record_count = static_cast<int>(state.maps.size());
  out->set_record_count = static_cast<int>(state.sets.size());
  out->slice_record_count = static_cast<int>(state.slices.size());
  out->iterator_record_count = static_cast<int>(state.iterators.size());
  out->last_handle = state.last_handle;
  out->last_input_a = state.last_input_a;
  out->last_input_b = state.last_input_b;
  out->last_input_c = state.last_input_c;
  out->last_status = state.last_status;
  out->last_result = state.last_result;
  return 0;
}
