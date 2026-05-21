#include "runtime/stdlib/text_runtime_contract.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <mutex>
#include <utility>
#include <vector>

namespace {

struct TextRecord {
  int byte_count = 0;
  int unit_count = 0;
  bool valid_utf8 = false;
  bool owns_utf8_storage = false;
  std::vector<unsigned char> utf8_bytes;
};

struct RuntimeStdlibTextState {
  std::mutex mutex;
  std::uint64_t reset_generation = 0;
  std::uint64_t total_call_count = 0;
  std::uint64_t literal_call_count = 0;
  std::uint64_t query_call_count = 0;
  std::uint64_t concat_call_count = 0;
  std::uint64_t storage_create_call_count = 0;
  std::uint64_t storage_query_call_count = 0;
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
  if (handle <= 0 || handle > static_cast<int>(state.records.size())) {
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

bool IsContinuation(unsigned char byte) {
  return byte >= 0x80 && byte <= 0xBF;
}

bool TryCountUtf8Scalars(const unsigned char *bytes,
                         int byte_count,
                         int &scalar_count) {
  scalar_count = 0;
  if (byte_count < 0 || (bytes == nullptr && byte_count != 0)) {
    return false;
  }

  int index = 0;
  while (index < byte_count) {
    const unsigned char first = bytes[index];
    if (first <= 0x7F) {
      ++index;
      ++scalar_count;
      continue;
    }
    if (first >= 0xC2 && first <= 0xDF) {
      if (index + 1 >= byte_count || !IsContinuation(bytes[index + 1])) {
        return false;
      }
      index += 2;
      ++scalar_count;
      continue;
    }
    if (first == 0xE0) {
      if (index + 2 >= byte_count || bytes[index + 1] < 0xA0 ||
          bytes[index + 1] > 0xBF || !IsContinuation(bytes[index + 2])) {
        return false;
      }
      index += 3;
      ++scalar_count;
      continue;
    }
    if ((first >= 0xE1 && first <= 0xEC) ||
        (first >= 0xEE && first <= 0xEF)) {
      if (index + 2 >= byte_count || !IsContinuation(bytes[index + 1]) ||
          !IsContinuation(bytes[index + 2])) {
        return false;
      }
      index += 3;
      ++scalar_count;
      continue;
    }
    if (first == 0xED) {
      if (index + 2 >= byte_count || bytes[index + 1] < 0x80 ||
          bytes[index + 1] > 0x9F || !IsContinuation(bytes[index + 2])) {
        return false;
      }
      index += 3;
      ++scalar_count;
      continue;
    }
    if (first == 0xF0) {
      if (index + 3 >= byte_count || bytes[index + 1] < 0x90 ||
          bytes[index + 1] > 0xBF || !IsContinuation(bytes[index + 2]) ||
          !IsContinuation(bytes[index + 3])) {
        return false;
      }
      index += 4;
      ++scalar_count;
      continue;
    }
    if (first >= 0xF1 && first <= 0xF3) {
      if (index + 3 >= byte_count || !IsContinuation(bytes[index + 1]) ||
          !IsContinuation(bytes[index + 2]) ||
          !IsContinuation(bytes[index + 3])) {
        return false;
      }
      index += 4;
      ++scalar_count;
      continue;
    }
    if (first == 0xF4) {
      if (index + 3 >= byte_count || bytes[index + 1] < 0x80 ||
          bytes[index + 1] > 0x8F || !IsContinuation(bytes[index + 2]) ||
          !IsContinuation(bytes[index + 3])) {
        return false;
      }
      index += 4;
      ++scalar_count;
      continue;
    }
    return false;
  }
  return true;
}

TextRecord MakeOwnedUtf8Record(const unsigned char *bytes,
                               int byte_count,
                               int scalar_count) {
  TextRecord record;
  record.byte_count = byte_count;
  record.unit_count = scalar_count;
  record.valid_utf8 = true;
  record.owns_utf8_storage = true;
  if (byte_count > 0) {
    record.utf8_bytes.assign(bytes, bytes + byte_count);
  }
  return record;
}

int CountOwnedStorageRecords(const RuntimeStdlibTextState &state) {
  return static_cast<int>(std::count_if(
      state.records.begin(), state.records.end(), [](const TextRecord &record) {
        return record.owns_utf8_storage;
      }));
}

int CountOwnedStorageBytes(const RuntimeStdlibTextState &state) {
  int total = 0;
  for (const TextRecord &record : state.records) {
    if (!record.owns_utf8_storage) {
      continue;
    }
    if (!SumFitsInt(total, record.byte_count)) {
      return std::numeric_limits<int>::max();
    }
    total += record.byte_count;
  }
  return total;
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
  state.storage_create_call_count = 0;
  state.storage_query_call_count = 0;
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
  TextRecord record;
  record.byte_count = byte_count;
  record.unit_count = unit_count;
  record.valid_utf8 = true;
  state.records.push_back(std::move(record));
  const int handle = static_cast<int>(state.records.size());
  RecordCall(state, state.literal_call_count, handle, byte_count, unit_count,
             valid_utf8, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_text_utf8_storage_i32(
    const char *utf8_bytes,
    int byte_count) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (byte_count < 0 || (utf8_bytes == nullptr && byte_count != 0)) {
    RecordCall(state, state.storage_create_call_count, 0, byte_count, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE, 0);
    return 0;
  }

  int scalar_count = 0;
  const auto *bytes = reinterpret_cast<const unsigned char *>(utf8_bytes);
  if (!TryCountUtf8Scalars(bytes, byte_count, scalar_count)) {
    RecordCall(state, state.storage_create_call_count, 0, byte_count, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8, 0);
    return 0;
  }

  state.records.push_back(MakeOwnedUtf8Record(bytes, byte_count, scalar_count));
  const int handle = static_cast<int>(state.records.size());
  RecordCall(state, state.storage_create_call_count, handle, byte_count,
             scalar_count, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
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

extern "C" int objc3_runtime_stdlib_text_scalar_count_i32(int handle) {
  return objc3_runtime_stdlib_text_unit_count_i32(handle);
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

extern "C" int objc3_runtime_stdlib_text_byte_at_or_i32(int handle,
                                                        int byte_index,
                                                        int default_value) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *record = FindText(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.storage_query_call_count, handle, byte_index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, default_value);
    return default_value;
  }
  if (!record->owns_utf8_storage) {
    RecordCall(state, state.storage_query_call_count, handle, byte_index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE,
               default_value);
    return default_value;
  }
  if (byte_index < 0 || byte_index >= record->byte_count) {
    RecordCall(state, state.storage_query_call_count, handle, byte_index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUT_OF_BOUNDS, default_value);
    return default_value;
  }
  const int result =
      static_cast<int>(record->utf8_bytes[static_cast<std::size_t>(byte_index)]);
  RecordCall(state, state.storage_query_call_count, handle, byte_index,
             default_value, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
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

  TextRecord joined;
  joined.byte_count = left->byte_count + right->byte_count;
  joined.unit_count = left->unit_count + right->unit_count;
  joined.valid_utf8 = left->valid_utf8 && right->valid_utf8;
  joined.owns_utf8_storage =
      left->owns_utf8_storage && right->owns_utf8_storage;
  if (joined.owns_utf8_storage) {
    joined.utf8_bytes.reserve(static_cast<std::size_t>(joined.byte_count));
    joined.utf8_bytes.insert(joined.utf8_bytes.end(), left->utf8_bytes.begin(),
                             left->utf8_bytes.end());
    joined.utf8_bytes.insert(joined.utf8_bytes.end(),
                             right->utf8_bytes.begin(),
                             right->utf8_bytes.end());
  }
  state.records.push_back(std::move(joined));
  const int handle = static_cast<int>(state.records.size());
  RecordCall(state, state.concat_call_count, handle, left_handle, right_handle,
             0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_text_append_utf8_storage_i32(
    int handle,
    const char *utf8_bytes,
    int byte_count) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *base = FindText(state, handle);
  if (base == nullptr) {
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  if (!base->owns_utf8_storage) {
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE, 0);
    return 0;
  }
  if (byte_count < 0 || (utf8_bytes == nullptr && byte_count != 0) ||
      !SumFitsInt(base->byte_count, byte_count)) {
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE, 0);
    return 0;
  }

  int appended_scalar_count = 0;
  const auto *bytes = reinterpret_cast<const unsigned char *>(utf8_bytes);
  if (!TryCountUtf8Scalars(bytes, byte_count, appended_scalar_count) ||
      !SumFitsInt(base->unit_count, appended_scalar_count)) {
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8, 0);
    return 0;
  }

  TextRecord record;
  record.byte_count = base->byte_count + byte_count;
  record.unit_count = base->unit_count + appended_scalar_count;
  record.valid_utf8 = true;
  record.owns_utf8_storage = true;
  record.utf8_bytes = base->utf8_bytes;
  if (byte_count > 0) {
    record.utf8_bytes.insert(record.utf8_bytes.end(), bytes, bytes + byte_count);
  }
  state.records.push_back(std::move(record));
  const int result = static_cast<int>(state.records.size());
  RecordCall(state, state.storage_create_call_count, result, handle, byte_count,
             appended_scalar_count, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK,
             result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_equal_i32(int left_handle,
                                                   int right_handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *left = FindText(state, left_handle);
  TextRecord *right = FindText(state, right_handle);
  if (left == nullptr || right == nullptr) {
    RecordCall(state, state.storage_query_call_count, 0, left_handle,
               right_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  if (!left->owns_utf8_storage || !right->owns_utf8_storage) {
    RecordCall(state, state.storage_query_call_count, 0, left_handle,
               right_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE, 0);
    return 0;
  }
  const int result = left->utf8_bytes == right->utf8_bytes ? 1 : 0;
  RecordCall(state, state.storage_query_call_count, left_handle, left_handle,
             right_handle, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_last_status_i32(void) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ++state.total_call_count;
  ++state.status_call_count;
  return state.last_status;
}

extern "C" int objc3_runtime_copy_stdlib_text_utf8_bytes_for_testing(
    int handle,
    char *out,
    int capacity) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  TextRecord *record = FindText(state, handle);
  if (record == nullptr) {
    RecordCall(state, state.storage_query_call_count, handle, capacity, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE, 0);
    return 0;
  }
  if (!record->owns_utf8_storage) {
    RecordCall(state, state.storage_query_call_count, handle, capacity, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE, 0);
    return 0;
  }
  if (out == nullptr || capacity < record->byte_count) {
    RecordCall(state, state.storage_query_call_count, handle, capacity,
               record->byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUTPUT_TOO_SMALL, 0);
    return 0;
  }
  if (record->byte_count > 0) {
    std::memcpy(out, record->utf8_bytes.data(),
                static_cast<std::size_t>(record->byte_count));
  }
  RecordCall(state, state.storage_query_call_count, handle, capacity,
             record->byte_count, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK,
             record->byte_count);
  return record->byte_count;
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
  out->storage_create_call_count = state.storage_create_call_count;
  out->storage_query_call_count = state.storage_query_call_count;
  out->status_call_count = state.status_call_count;
  out->text_record_count = static_cast<int>(state.records.size());
  out->owned_storage_record_count = CountOwnedStorageRecords(state);
  out->owned_storage_byte_count = CountOwnedStorageBytes(state);
  out->last_handle = state.last_handle;
  out->last_input_a = state.last_input_a;
  out->last_input_b = state.last_input_b;
  out->last_input_c = state.last_input_c;
  out->last_status = state.last_status;
  out->last_result = state.last_result;
  return 0;
}
