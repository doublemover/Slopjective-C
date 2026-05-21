#include "runtime/stdlib/collections_runtime_contract.h"

#include <algorithm>
#include <array>
#include <cstdint>
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

struct RuntimeStdlibCollectionsState {
  std::mutex mutex;
  std::uint64_t reset_generation = 0;
  std::uint64_t total_call_count = 0;
  std::uint64_t array_create_call_count = 0;
  std::uint64_t array_query_call_count = 0;
  std::uint64_t map_create_call_count = 0;
  std::uint64_t map_query_call_count = 0;
  std::uint64_t status_call_count = 0;
  std::vector<ArrayRecord> arrays;
  std::vector<MapRecord> maps;
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

int ClampToZero(int value) {
  return std::max(value, 0);
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
  state.status_call_count = 0;
  state.arrays.clear();
  state.maps.clear();
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
  out->status_call_count = state.status_call_count;
  out->array_record_count = static_cast<int>(state.arrays.size());
  out->map_record_count = static_cast<int>(state.maps.size());
  out->last_handle = state.last_handle;
  out->last_input_a = state.last_input_a;
  out->last_input_b = state.last_input_b;
  out->last_input_c = state.last_input_c;
  out->last_status = state.last_status;
  out->last_result = state.last_result;
  return 0;
}
