#include "runtime/stdlib/text_runtime_contract.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <mutex>
#include <vector>

namespace {

struct TextRecord {
  int byte_count = 0;
  int unit_count = 0;
  bool valid_utf8 = false;
};

struct RuntimeStdlibTextState {
  std::mutex mutex;
  std::uint64_t reset_generation = 0;
  std::uint64_t total_call_count = 0;
  std::uint64_t literal_call_count = 0;
  std::uint64_t query_call_count = 0;
  std::uint64_t concat_call_count = 0;
  std::uint64_t status_call_count = 0;
  std::vector<TextRecord> records;
  int last_handle = 0;
  int last_input_a = 0;
  int last_input_b = 0;
  int last_input_c = 0;
  int last_status = OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK;
  int last_result = 0;
};

RuntimeStdlibTextState &State() {
  static RuntimeStdlibTextState state;
  return state;
}

void RecordCall(RuntimeStdlibTextState &state,
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

TextRecord *FindText(RuntimeStdlibTextState &state, int handle) {
  if (handle <= 0 ||
      handle > static_cast<int>(state.records.size())) {
    return nullptr;
  }
  return &state.records[static_cast<std::size_t>(handle - 1)];
}

bool HasValidTextShape(int byte_count, int unit_count) {
  return byte_count >= 0 && unit_count >= 0 && unit_count <= byte_count;
}

bool SumFitsInt(int left, int right) {
  return left <= std::numeric_limits<int>::max() - right;
}

}  // namespace

namespace objc3c::runtime {

void ResetRuntimeStdlibTextStateForTesting() {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const std::uint64_t next_reset_generation = state.reset_generation + 1;
  state.total_call_count = 0;
  state.literal_call_count = 0;
  state.query_call_count = 0;
  state.concat_call_count = 0;
  state.status_call_count = 0;
  state.records.clear();
  state.last_handle = 0;
  state.last_input_a = 0;
  state.last_input_b = 0;
  state.last_input_c = 0;
  state.last_status = OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK;
  state.last_result = 0;
  state.reset_generation = next_reset_generation;
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_stdlib_text_utf8_literal_i32(int byte_count,
                                                          int unit_count,
                                                          int valid_utf8) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (!HasValidTextShape(byte_count, unit_count)) {
    RecordCall(state, state.literal_call_count, 0, byte_count, unit_count,
               valid_utf8, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE, 0);
    return 0;
  }
  if (valid_utf8 == 0) {
    RecordCall(state, state.literal_call_count, 0, byte_count, unit_count,
               valid_utf8, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8,
               0);
    return 0;
  }
  state.records.push_back(TextRecord{byte_count, unit_count, true});
  const int handle = static_cast<int>(state.records.size());
  RecordCall(state, state.literal_call_count, handle, byte_count, unit_count,
             valid_utf8, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_text_byte_count_i32(int handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *record = FindText(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  RecordCall(state, state.query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, record->byte_count);
  return record->byte_count;
}

extern "C" int objc3_runtime_stdlib_text_unit_count_i32(int handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *record = FindText(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  RecordCall(state, state.query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, record->unit_count);
  return record->unit_count;
}

extern "C" int objc3_runtime_stdlib_text_is_valid_utf8_i32(int handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *record = FindText(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.query_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  const int result = record->valid_utf8 ? 1 : 0;
  RecordCall(state, state.query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_prefix_units_i32(
    int handle,
    int requested_units) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *record = FindText(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.query_call_count, handle, requested_units, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  const int result = std::min(record->unit_count, std::max(requested_units, 0));
  RecordCall(state, state.query_call_count, handle, requested_units, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_concat_i32(int left_handle,
                                                    int right_handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *left = FindText(state, left_handle);
  TextRecord *right = FindText(state, right_handle);
  if (left == nullptr || right == nullptr) {
    RecordCall(state, state.concat_call_count, 0, left_handle, right_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  if (!SumFitsInt(left->byte_count, right->byte_count) ||
      !SumFitsInt(left->unit_count, right->unit_count)) {
    RecordCall(state, state.concat_call_count, 0, left_handle, right_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE, 0);
    return 0;
  }
  state.records.push_back(TextRecord{
      left->byte_count + right->byte_count,
      left->unit_count + right->unit_count,
      left->valid_utf8 && right->valid_utf8,
  });
  const int handle = static_cast<int>(state.records.size());
  RecordCall(state, state.concat_call_count, handle, left_handle, right_handle,
             0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_text_last_status_i32(void) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ++state.total_call_count;
  ++state.status_call_count;
  return state.last_status;
}

extern "C" int objc3_runtime_copy_stdlib_text_state_for_testing(
    objc3_runtime_stdlib_text_snapshot *out) {
  if (out == nullptr) {
    return -1;
  }
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  out->reset_generation = state.reset_generation;
  out->total_call_count = state.total_call_count;
  out->literal_call_count = state.literal_call_count;
  out->query_call_count = state.query_call_count;
  out->concat_call_count = state.concat_call_count;
  out->status_call_count = state.status_call_count;
  out->text_record_count = static_cast<int>(state.records.size());
  out->last_handle = state.last_handle;
  out->last_input_a = state.last_input_a;
  out->last_input_b = state.last_input_b;
  out->last_input_c = state.last_input_c;
  out->last_status = state.last_status;
  out->last_result = state.last_result;
  return 0;
}
