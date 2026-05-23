#include "runtime/stdlib/text_runtime_contract.h"

#include "runtime/stdlib/stdlib_runtime_storage.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <mutex>
#include <utility>
#include <vector>

namespace {

namespace storage = objc3c::runtime::stdlib_runtime;

struct TextRecord {
  storage::RecordHeader header;
  int byte_count = 0;
  int unit_count = 0;
  bool valid_utf8 = false;
  std::vector<unsigned char> utf8_bytes;
  std::vector<int> scalar_values;
  int iterator_position = 0;
  int iterator_source_handle = 0;
  storage::DescriptorKind iterator_source_kind =
      storage::DescriptorKind::Unknown;
  std::uint64_t expected_mutation_generation = 0;
};

struct RuntimeStdlibTextState {
  std::mutex mutex;
  storage::HandleTable<TextRecord> records{storage::HandleTableOwner::Text};
  std::uint64_t total_call_count = 0;
  std::uint64_t literal_call_count = 0;
  std::uint64_t query_call_count = 0;
  std::uint64_t concat_call_count = 0;
  std::uint64_t storage_create_call_count = 0;
  std::uint64_t storage_query_call_count = 0;
  std::uint64_t status_call_count = 0;
  std::uint64_t scalar_query_call_count = 0;
  std::uint64_t scalar_iterator_call_count = 0;
  std::uint64_t builder_create_call_count = 0;
  std::uint64_t builder_append_call_count = 0;
  std::uint64_t builder_finalize_call_count = 0;
  std::uint64_t interpolation_call_count = 0;
  std::uint64_t equality_call_count = 0;
  std::uint64_t compare_call_count = 0;
  std::uint64_t format_call_count = 0;
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
  int last_status = OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK;
  int last_result = 0;
  int last_malformed_offset = -1;
};

RuntimeStdlibTextState &State() {
  static RuntimeStdlibTextState state;
  return state;
}

bool IsContinuation(unsigned char byte) {
  return byte >= 0x80 && byte <= 0xBF;
}

void RecordStatusCounter(RuntimeStdlibTextState &state, int status) {
  switch (status) {
    case OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE:
      ++state.invalid_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CROSS_KIND_HANDLE:
      ++state.cross_kind_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STALE_HANDLE:
      ++state.stale_handle_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_DESCRIPTOR:
      ++state.malformed_descriptor_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED:
      ++state.capacity_failure_count;
      break;
    case OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MUTATED_DURING_ITERATION:
      ++state.iterator_invalidation_count;
      break;
    default:
      break;
  }
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
  RecordStatusCounter(state, status);
}

int StatusForLookup(storage::LookupStatus status) {
  switch (status) {
    case storage::LookupStatus::Ok:
      return OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK;
    case storage::LookupStatus::CrossKind:
      return OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CROSS_KIND_HANDLE;
    case storage::LookupStatus::StaleHandle:
      return OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STALE_HANDLE;
    case storage::LookupStatus::InvalidHandle:
    default:
      return OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE;
  }
}

bool HasValidTextShape(int byte_count, int unit_count) {
  return byte_count >= 0 && unit_count >= 0 && unit_count <= byte_count;
}

bool TryDecodeUtf8Scalars(const unsigned char *bytes,
                          int byte_count,
                          int &scalar_count,
                          std::vector<int> *scalars,
                          int *malformed_offset = nullptr) {
  scalar_count = 0;
  if (malformed_offset != nullptr) {
    *malformed_offset = -1;
  }
  if (byte_count < 0 || (bytes == nullptr && byte_count != 0)) {
    if (malformed_offset != nullptr) {
      *malformed_offset = 0;
    }
    return false;
  }
  if (scalars != nullptr) {
    scalars->clear();
  }

  int index = 0;
  while (index < byte_count) {
    const unsigned char first = bytes[index];
    int width = 0;
    int scalar = 0;
    if (first <= 0x7F) {
      width = 1;
      scalar = first;
    } else if (first >= 0xC2 && first <= 0xDF) {
      if (index + 1 >= byte_count || !IsContinuation(bytes[index + 1])) {
        if (malformed_offset != nullptr) {
          *malformed_offset = index;
        }
        return false;
      }
      width = 2;
      scalar = ((first & 0x1F) << 6) | (bytes[index + 1] & 0x3F);
    } else if (first == 0xE0) {
      if (index + 2 >= byte_count || bytes[index + 1] < 0xA0 ||
          bytes[index + 1] > 0xBF || !IsContinuation(bytes[index + 2])) {
        if (malformed_offset != nullptr) {
          *malformed_offset = index;
        }
        return false;
      }
      width = 3;
      scalar = ((first & 0x0F) << 12) | ((bytes[index + 1] & 0x3F) << 6) |
               (bytes[index + 2] & 0x3F);
    } else if ((first >= 0xE1 && first <= 0xEC) ||
               (first >= 0xEE && first <= 0xEF)) {
      if (index + 2 >= byte_count || !IsContinuation(bytes[index + 1]) ||
          !IsContinuation(bytes[index + 2])) {
        if (malformed_offset != nullptr) {
          *malformed_offset = index;
        }
        return false;
      }
      width = 3;
      scalar = ((first & 0x0F) << 12) | ((bytes[index + 1] & 0x3F) << 6) |
               (bytes[index + 2] & 0x3F);
    } else if (first == 0xED) {
      if (index + 2 >= byte_count || bytes[index + 1] < 0x80 ||
          bytes[index + 1] > 0x9F || !IsContinuation(bytes[index + 2])) {
        if (malformed_offset != nullptr) {
          *malformed_offset = index;
        }
        return false;
      }
      width = 3;
      scalar = ((first & 0x0F) << 12) | ((bytes[index + 1] & 0x3F) << 6) |
               (bytes[index + 2] & 0x3F);
    } else if (first == 0xF0) {
      if (index + 3 >= byte_count || bytes[index + 1] < 0x90 ||
          bytes[index + 1] > 0xBF || !IsContinuation(bytes[index + 2]) ||
          !IsContinuation(bytes[index + 3])) {
        if (malformed_offset != nullptr) {
          *malformed_offset = index;
        }
        return false;
      }
      width = 4;
      scalar = ((first & 0x07) << 18) | ((bytes[index + 1] & 0x3F) << 12) |
               ((bytes[index + 2] & 0x3F) << 6) |
               (bytes[index + 3] & 0x3F);
    } else if (first >= 0xF1 && first <= 0xF3) {
      if (index + 3 >= byte_count || !IsContinuation(bytes[index + 1]) ||
          !IsContinuation(bytes[index + 2]) ||
          !IsContinuation(bytes[index + 3])) {
        if (malformed_offset != nullptr) {
          *malformed_offset = index;
        }
        return false;
      }
      width = 4;
      scalar = ((first & 0x07) << 18) | ((bytes[index + 1] & 0x3F) << 12) |
               ((bytes[index + 2] & 0x3F) << 6) |
               (bytes[index + 3] & 0x3F);
    } else if (first == 0xF4) {
      if (index + 3 >= byte_count || bytes[index + 1] < 0x80 ||
          bytes[index + 1] > 0x8F || !IsContinuation(bytes[index + 2]) ||
          !IsContinuation(bytes[index + 3])) {
        if (malformed_offset != nullptr) {
          *malformed_offset = index;
        }
        return false;
      }
      width = 4;
      scalar = ((first & 0x07) << 18) | ((bytes[index + 1] & 0x3F) << 12) |
               ((bytes[index + 2] & 0x3F) << 6) |
               (bytes[index + 3] & 0x3F);
    } else {
      if (malformed_offset != nullptr) {
        *malformed_offset = index;
      }
      return false;
    }
    index += width;
    ++scalar_count;
    if (scalars != nullptr) {
      scalars->push_back(scalar);
    }
  }
  return true;
}

TextRecord MakeLiteralRecord(int byte_count, int unit_count) {
  TextRecord record;
  record.byte_count = byte_count;
  record.unit_count = unit_count;
  record.valid_utf8 = true;
  return record;
}

TextRecord MakeOwnedUtf8Record(const unsigned char *bytes,
                               int byte_count,
                               int scalar_count,
                               std::vector<int> scalars) {
  TextRecord record;
  record.byte_count = byte_count;
  record.unit_count = scalar_count;
  record.valid_utf8 = true;
  record.scalar_values = std::move(scalars);
  if (byte_count > 0) {
    record.utf8_bytes.assign(bytes, bytes + byte_count);
  }
  return record;
}

TextRecord MakeBuilderRecord() {
  TextRecord record;
  record.valid_utf8 = true;
  return record;
}

bool HasUtf8Storage(const TextRecord &record) {
  return record.header.descriptor_kind == storage::DescriptorKind::TextOwnedUtf8 ||
         record.header.descriptor_kind == storage::DescriptorKind::TextBuilder;
}

bool TryEncodeScalarUtf8(int scalar, std::array<unsigned char, 4> &bytes,
                         int &byte_count) {
  byte_count = 0;
  if (scalar < 0 || scalar > 0x10FFFF ||
      (scalar >= 0xD800 && scalar <= 0xDFFF)) {
    return false;
  }
  if (scalar <= 0x7F) {
    bytes[0] = static_cast<unsigned char>(scalar);
    byte_count = 1;
    return true;
  }
  if (scalar <= 0x7FF) {
    bytes[0] = static_cast<unsigned char>(0xC0 | (scalar >> 6));
    bytes[1] = static_cast<unsigned char>(0x80 | (scalar & 0x3F));
    byte_count = 2;
    return true;
  }
  if (scalar <= 0xFFFF) {
    bytes[0] = static_cast<unsigned char>(0xE0 | (scalar >> 12));
    bytes[1] = static_cast<unsigned char>(0x80 | ((scalar >> 6) & 0x3F));
    bytes[2] = static_cast<unsigned char>(0x80 | (scalar & 0x3F));
    byte_count = 3;
    return true;
  }
  bytes[0] = static_cast<unsigned char>(0xF0 | (scalar >> 18));
  bytes[1] = static_cast<unsigned char>(0x80 | ((scalar >> 12) & 0x3F));
  bytes[2] = static_cast<unsigned char>(0x80 | ((scalar >> 6) & 0x3F));
  bytes[3] = static_cast<unsigned char>(0x80 | (scalar & 0x3F));
  byte_count = 4;
  return true;
}

int AppendDecodedUtf8ToBuilder(RuntimeStdlibTextState &state,
                               TextRecord &builder,
                               const unsigned char *bytes,
                               int byte_count,
                               int call_handle,
                               int input_b,
                               int input_c,
                               std::uint64_t &family_count) {
  if (byte_count < 0 ||
      storage::AddWouldOverflowInt(builder.byte_count, byte_count)) {
    RecordCall(state, family_count, call_handle, input_b, input_c, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE, 0);
    return 0;
  }
  if (storage::CountExceedsStorageCapacity(builder.byte_count + byte_count)) {
    RecordCall(state, family_count, call_handle, input_b, input_c, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  if (bytes == nullptr && byte_count != 0) {
    RecordCall(state, family_count, call_handle, input_b, input_c, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_DESCRIPTOR, 0);
    return 0;
  }

  int scalar_count = 0;
  std::vector<int> scalars;
  int malformed_offset = -1;
  if (!TryDecodeUtf8Scalars(bytes, byte_count, scalar_count, &scalars,
                            &malformed_offset) ||
      storage::AddWouldOverflowInt(builder.unit_count, scalar_count)) {
    state.last_malformed_offset = malformed_offset;
    RecordCall(state, family_count, call_handle, input_b, input_c, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8, 0);
    return 0;
  }
  if (byte_count > 0) {
    builder.utf8_bytes.insert(builder.utf8_bytes.end(), bytes,
                              bytes + byte_count);
    builder.scalar_values.insert(builder.scalar_values.end(), scalars.begin(),
                                 scalars.end());
  }
  builder.byte_count += byte_count;
  builder.unit_count += scalar_count;
  builder.valid_utf8 = true;
  ++state.mutation_generation;
  builder.header.mutation_generation = state.mutation_generation;
  RecordCall(state, family_count, call_handle, input_b, input_c, scalar_count,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, builder.byte_count);
  return builder.byte_count;
}

int CountOwnedStorageRecords(const RuntimeStdlibTextState &state) {
  return state.records.RecordCount(storage::DescriptorKind::TextOwnedUtf8);
}

int CountOwnedStorageBytes(const RuntimeStdlibTextState &state) {
  int byte_total = 0;
  state.records.ForEachLiveRecord([&byte_total](const TextRecord &record) {
    if (record.header.descriptor_kind ==
        storage::DescriptorKind::TextOwnedUtf8) {
      if (storage::AddWouldOverflowInt(byte_total, record.byte_count)) {
        byte_total = std::numeric_limits<int>::max();
        return;
      }
      byte_total += record.byte_count;
    }
  });
  return byte_total;
}

int CreateOwnedText(RuntimeStdlibTextState &state,
                    const unsigned char *bytes,
                    int byte_count,
                    int scalar_count,
                    std::vector<int> scalars) {
  if (!state.records.CanAllocate()) {
    return 0;
  }
  return state.records.Store(
      storage::DescriptorKind::TextOwnedUtf8,
      MakeOwnedUtf8Record(bytes, byte_count, scalar_count, std::move(scalars)));
}

}  // namespace

namespace objc3c::runtime {

void ResetRuntimeStdlibTextStateForTesting() {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.total_call_count = 0;
  state.literal_call_count = 0;
  state.query_call_count = 0;
  state.concat_call_count = 0;
  state.storage_create_call_count = 0;
  state.storage_query_call_count = 0;
  state.status_call_count = 0;
  state.scalar_query_call_count = 0;
  state.scalar_iterator_call_count = 0;
  state.builder_create_call_count = 0;
  state.builder_append_call_count = 0;
  state.builder_finalize_call_count = 0;
  state.interpolation_call_count = 0;
  state.equality_call_count = 0;
  state.compare_call_count = 0;
  state.format_call_count = 0;
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
  state.last_status = OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK;
  state.last_result = 0;
  state.last_malformed_offset = -1;
  state.records.Reset();
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
  if (!state.records.CanAllocate()) {
    RecordCall(state, state.literal_call_count, 0, byte_count, unit_count,
               valid_utf8, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED,
               0);
    return 0;
  }
  const int handle = state.records.Store(
      storage::DescriptorKind::TextLiteral,
      MakeLiteralRecord(byte_count, unit_count));
  RecordCall(state, state.literal_call_count, handle, byte_count, unit_count,
             valid_utf8, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_text_utf8_storage_i32(
    const char *utf8_bytes,
    int byte_count) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (byte_count < 0) {
    RecordCall(state, state.storage_create_call_count, 0, byte_count, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE, 0);
    return 0;
  }
  if (storage::CountExceedsStorageCapacity(byte_count)) {
    RecordCall(state, state.storage_create_call_count, 0, byte_count, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  if (utf8_bytes == nullptr && byte_count != 0) {
    RecordCall(state, state.storage_create_call_count, 0, byte_count, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_DESCRIPTOR, 0);
    return 0;
  }

  int scalar_count = 0;
  std::vector<int> scalars;
  int malformed_offset = -1;
  const auto *bytes = reinterpret_cast<const unsigned char *>(utf8_bytes);
  if (!TryDecodeUtf8Scalars(bytes, byte_count, scalar_count, &scalars,
                            &malformed_offset)) {
    state.last_malformed_offset = malformed_offset;
    RecordCall(state, state.storage_create_call_count, 0, byte_count, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8, 0);
    return 0;
  }
  const int handle =
      CreateOwnedText(state, bytes, byte_count, scalar_count, std::move(scalars));
  if (handle == 0) {
    RecordCall(state, state.storage_create_call_count, 0, byte_count,
               scalar_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.storage_create_call_count, handle, byte_count,
             scalar_count, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_text_byte_count_i32(int handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.query_call_count, handle, handle, 0, 0, status, 0);
    return 0;
  }
  RecordCall(state, state.query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, lookup.record->byte_count);
  return lookup.record->byte_count;
}

extern "C" int objc3_runtime_stdlib_text_unit_count_i32(int handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.query_call_count, handle, handle, 0, 0, status, 0);
    return 0;
  }
  RecordCall(state, state.query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, lookup.record->unit_count);
  return lookup.record->unit_count;
}

extern "C" int objc3_runtime_stdlib_text_scalar_count_i32(int handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.scalar_query_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  RecordCall(state, state.scalar_query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, lookup.record->unit_count);
  return lookup.record->unit_count;
}

extern "C" int objc3_runtime_stdlib_text_scalar_at_or_i32(
    int handle,
    int scalar_index,
    int default_value) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.scalar_query_call_count, handle, scalar_index,
               default_value, 0, status, default_value);
    return default_value;
  }
  if (!HasUtf8Storage(*lookup.record)) {
    RecordCall(state, state.scalar_query_call_count, handle, scalar_index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE,
               default_value);
    return default_value;
  }
  if (scalar_index < 0 ||
      scalar_index >= static_cast<int>(lookup.record->scalar_values.size())) {
    RecordCall(state, state.scalar_query_call_count, handle, scalar_index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUT_OF_BOUNDS, default_value);
    return default_value;
  }
  const int result =
      lookup.record->scalar_values[static_cast<std::size_t>(scalar_index)];
  RecordCall(state, state.scalar_query_call_count, handle, scalar_index,
             default_value, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_is_valid_utf8_i32(int handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.query_call_count, handle, handle, 0, 0, status, 0);
    return 0;
  }
  const int result = lookup.record->valid_utf8 ? 1 : 0;
  RecordCall(state, state.query_call_count, handle, handle, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_byte_at_or_i32(int handle,
                                                        int byte_index,
                                                        int default_value) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.storage_query_call_count, handle, byte_index,
               default_value, 0, status, default_value);
    return default_value;
  }
  if (!HasUtf8Storage(*lookup.record)) {
    RecordCall(state, state.storage_query_call_count, handle, byte_index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE,
               default_value);
    return default_value;
  }
  if (byte_index < 0 || byte_index >= lookup.record->byte_count) {
    RecordCall(state, state.storage_query_call_count, handle, byte_index,
               default_value, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUT_OF_BOUNDS, default_value);
    return default_value;
  }
  const int result = static_cast<int>(
      lookup.record->utf8_bytes[static_cast<std::size_t>(byte_index)]);
  RecordCall(state, state.storage_query_call_count, handle, byte_index,
             default_value, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_prefix_units_i32(
    int handle,
    int requested_units) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.query_call_count, handle, requested_units, 0, 0,
               status, 0);
    return 0;
  }
  const int result =
      std::min(lookup.record->unit_count, std::max(requested_units, 0));
  RecordCall(state, state.query_call_count, handle, requested_units, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_concat_i32(int left_handle,
                                                    int right_handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto left_lookup = state.records.Lookup(
      left_handle, {storage::DescriptorKind::TextLiteral,
                    storage::DescriptorKind::TextOwnedUtf8,
                    storage::DescriptorKind::TextBuilder});
  auto right_lookup = state.records.Lookup(
      right_handle, {storage::DescriptorKind::TextLiteral,
                     storage::DescriptorKind::TextOwnedUtf8,
                     storage::DescriptorKind::TextBuilder});
  if (left_lookup.status != storage::LookupStatus::Ok ||
      right_lookup.status != storage::LookupStatus::Ok) {
    const int status = left_lookup.status != storage::LookupStatus::Ok
                           ? StatusForLookup(left_lookup.status)
                           : StatusForLookup(right_lookup.status);
    RecordCall(state, state.concat_call_count, 0, left_handle, right_handle, 0,
               status, 0);
    return 0;
  }
  if (storage::AddWouldOverflowInt(left_lookup.record->byte_count,
                                   right_lookup.record->byte_count) ||
      storage::AddWouldOverflowInt(left_lookup.record->unit_count,
                                   right_lookup.record->unit_count)) {
    RecordCall(state, state.concat_call_count, 0, left_handle, right_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OVERFLOW, 0);
    return 0;
  }

  TextRecord joined;
  joined.byte_count =
      left_lookup.record->byte_count + right_lookup.record->byte_count;
  joined.unit_count =
      left_lookup.record->unit_count + right_lookup.record->unit_count;
  joined.valid_utf8 =
      left_lookup.record->valid_utf8 && right_lookup.record->valid_utf8;
  if (HasUtf8Storage(*left_lookup.record) && HasUtf8Storage(*right_lookup.record)) {
    joined.utf8_bytes = left_lookup.record->utf8_bytes;
    joined.utf8_bytes.insert(joined.utf8_bytes.end(),
                             right_lookup.record->utf8_bytes.begin(),
                             right_lookup.record->utf8_bytes.end());
    joined.scalar_values = left_lookup.record->scalar_values;
    joined.scalar_values.insert(joined.scalar_values.end(),
                                right_lookup.record->scalar_values.begin(),
                                right_lookup.record->scalar_values.end());
  }
  const storage::DescriptorKind kind =
      joined.utf8_bytes.empty() && joined.byte_count != 0
          ? storage::DescriptorKind::TextLiteral
          : storage::DescriptorKind::TextOwnedUtf8;
  const int handle = state.records.Store(kind, std::move(joined));
  if (handle == 0) {
    RecordCall(state, state.concat_call_count, 0, left_handle, right_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
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
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               status, 0);
    return 0;
  }
  if (byte_count < 0 ||
      storage::AddWouldOverflowInt(lookup.record->byte_count, byte_count)) {
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE, 0);
    return 0;
  }
  if (storage::CountExceedsStorageCapacity(lookup.record->byte_count + byte_count)) {
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  if (utf8_bytes == nullptr && byte_count != 0) {
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_DESCRIPTOR, 0);
    return 0;
  }

  int appended_scalar_count = 0;
  std::vector<int> appended_scalars;
  int malformed_offset = -1;
  const auto *bytes = reinterpret_cast<const unsigned char *>(utf8_bytes);
  if (!TryDecodeUtf8Scalars(bytes, byte_count, appended_scalar_count,
                            &appended_scalars, &malformed_offset) ||
      storage::AddWouldOverflowInt(lookup.record->unit_count,
                                   appended_scalar_count)) {
    state.last_malformed_offset = malformed_offset;
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8, 0);
    return 0;
  }

  TextRecord record;
  record.byte_count = lookup.record->byte_count + byte_count;
  record.unit_count = lookup.record->unit_count + appended_scalar_count;
  record.valid_utf8 = true;
  record.utf8_bytes = lookup.record->utf8_bytes;
  record.scalar_values = lookup.record->scalar_values;
  if (byte_count > 0) {
    record.utf8_bytes.insert(record.utf8_bytes.end(), bytes, bytes + byte_count);
    record.scalar_values.insert(record.scalar_values.end(),
                                appended_scalars.begin(),
                                appended_scalars.end());
  }
  const int result =
      state.records.Store(storage::DescriptorKind::TextOwnedUtf8,
                          std::move(record));
  if (result == 0) {
    RecordCall(state, state.storage_create_call_count, 0, handle, byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.storage_create_call_count, result, handle, byte_count,
             appended_scalar_count, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK,
             result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_builder_i32(void) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int handle =
      state.records.Store(storage::DescriptorKind::TextBuilder,
                          MakeBuilderRecord());
  if (handle == 0) {
    RecordCall(state, state.builder_create_call_count, 0, 0, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.builder_create_call_count, handle, 0, 0, 0,
             OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_text_builder_append_utf8_i32(
    int builder_handle,
    const char *utf8_bytes,
    int byte_count) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      builder_handle, {storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.builder_append_call_count, builder_handle,
               builder_handle, byte_count, 0, status, 0);
    return 0;
  }
  const auto *bytes = reinterpret_cast<const unsigned char *>(utf8_bytes);
  return AppendDecodedUtf8ToBuilder(state, *lookup.record, bytes, byte_count,
                                    builder_handle, builder_handle, byte_count,
                                    state.builder_append_call_count);
}

extern "C" int objc3_runtime_stdlib_text_builder_append_text_i32(
    int builder_handle,
    int text_handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto builder_lookup = state.records.Lookup(
      builder_handle, {storage::DescriptorKind::TextBuilder});
  if (builder_lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(builder_lookup.status);
    RecordCall(state, state.interpolation_call_count, builder_handle,
               builder_handle, text_handle, 0, status, 0);
    return 0;
  }
  auto text_lookup = state.records.Lookup(
      text_handle, {storage::DescriptorKind::TextLiteral,
                    storage::DescriptorKind::TextOwnedUtf8,
                    storage::DescriptorKind::TextBuilder});
  if (text_lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(text_lookup.status);
    RecordCall(state, state.interpolation_call_count, builder_handle,
               builder_handle, text_handle, 0, status, 0);
    return 0;
  }
  if (!HasUtf8Storage(*text_lookup.record)) {
    RecordCall(state, state.interpolation_call_count, builder_handle,
               builder_handle, text_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE, 0);
    return 0;
  }
  return AppendDecodedUtf8ToBuilder(
      state, *builder_lookup.record, text_lookup.record->utf8_bytes.data(),
      text_lookup.record->byte_count, builder_handle, builder_handle,
      text_handle, state.interpolation_call_count);
}

extern "C" int objc3_runtime_stdlib_text_builder_append_i32_i32(
    int builder_handle,
    int value) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      builder_handle, {storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.interpolation_call_count, builder_handle,
               builder_handle, value, 0, status, 0);
    return 0;
  }
  std::array<char, 16> buffer{};
  const int written =
      std::snprintf(buffer.data(), buffer.size(), "%d", value);
  if (written <= 0 || written >= static_cast<int>(buffer.size())) {
    RecordCall(state, state.interpolation_call_count, builder_handle,
               builder_handle, value, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUTPUT_TOO_SMALL, 0);
    return 0;
  }
  return AppendDecodedUtf8ToBuilder(
      state, *lookup.record,
      reinterpret_cast<const unsigned char *>(buffer.data()), written,
      builder_handle, builder_handle, value, state.interpolation_call_count);
}

extern "C" int objc3_runtime_stdlib_text_builder_append_scalar_i32(
    int builder_handle,
    int scalar) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      builder_handle, {storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.builder_append_call_count, builder_handle,
               builder_handle, scalar, 0, status, 0);
    return 0;
  }
  std::array<unsigned char, 4> bytes{};
  int byte_count = 0;
  if (!TryEncodeScalarUtf8(scalar, bytes, byte_count)) {
    state.last_malformed_offset = 0;
    RecordCall(state, state.builder_append_call_count, builder_handle,
               builder_handle, scalar, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8, 0);
    return 0;
  }
  return AppendDecodedUtf8ToBuilder(state, *lookup.record, bytes.data(),
                                    byte_count, builder_handle,
                                    builder_handle, scalar,
                                    state.builder_append_call_count);
}

extern "C" int objc3_runtime_stdlib_text_builder_build_i32(
    int builder_handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      builder_handle, {storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.builder_finalize_call_count, builder_handle,
               builder_handle, 0, 0, status, 0);
    return 0;
  }
  TextRecord record;
  record.byte_count = lookup.record->byte_count;
  record.unit_count = lookup.record->unit_count;
  record.valid_utf8 = true;
  record.utf8_bytes = lookup.record->utf8_bytes;
  record.scalar_values = lookup.record->scalar_values;
  const int handle =
      state.records.Store(storage::DescriptorKind::TextOwnedUtf8,
                          std::move(record));
  if (handle == 0) {
    RecordCall(state, state.builder_finalize_call_count, builder_handle,
               builder_handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.builder_finalize_call_count, handle, builder_handle, 0,
             0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
}

extern "C" int objc3_runtime_stdlib_text_scalar_iterator_i32(int handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.scalar_iterator_call_count, handle, handle, 0, 0,
               status, 0);
    return 0;
  }
  if (!HasUtf8Storage(*lookup.record)) {
    RecordCall(state, state.scalar_iterator_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE, 0);
    return 0;
  }
  TextRecord iterator;
  iterator.scalar_values = lookup.record->scalar_values;
  iterator.iterator_source_handle = handle;
  iterator.iterator_source_kind = lookup.record->header.descriptor_kind;
  iterator.expected_mutation_generation =
      lookup.record->header.mutation_generation;
  const int iterator_handle =
      state.records.Store(storage::DescriptorKind::TextScalarIterator,
                          std::move(iterator));
  if (iterator_handle == 0) {
    RecordCall(state, state.scalar_iterator_call_count, handle, handle, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.scalar_iterator_call_count, iterator_handle, handle, 0,
             0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, iterator_handle);
  return iterator_handle;
}

extern "C" int objc3_runtime_stdlib_text_scalar_iterator_next_or_i32(
    int iterator_handle,
    int default_value) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto iterator_lookup = state.records.Lookup(
      iterator_handle, {storage::DescriptorKind::TextScalarIterator});
  if (iterator_lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(iterator_lookup.status);
    RecordCall(state, state.scalar_iterator_call_count, iterator_handle,
               default_value, 0, 0, status, default_value);
    return default_value;
  }
  TextRecord &iterator = *iterator_lookup.record;
  if (iterator.iterator_source_kind == storage::DescriptorKind::TextBuilder) {
    auto source_lookup = state.records.Lookup(
        iterator.iterator_source_handle,
        {storage::DescriptorKind::TextBuilder});
    if (source_lookup.status != storage::LookupStatus::Ok) {
      const int status = StatusForLookup(source_lookup.status);
      RecordCall(state, state.scalar_iterator_call_count, iterator_handle,
                 default_value, 0, 0, status, default_value);
      return default_value;
    }
    if (source_lookup.record->header.mutation_generation !=
        iterator.expected_mutation_generation) {
      RecordCall(
          state, state.scalar_iterator_call_count, iterator_handle, default_value,
          0, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MUTATED_DURING_ITERATION,
          default_value);
      return default_value;
    }
  }
  if (iterator.iterator_position >=
      static_cast<int>(iterator.scalar_values.size())) {
    RecordCall(state, state.scalar_iterator_call_count, iterator_handle,
               default_value, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUT_OF_BOUNDS, default_value);
    return default_value;
  }
  const int result =
      iterator.scalar_values[static_cast<std::size_t>(
          iterator.iterator_position)];
  ++iterator.iterator_position;
  RecordCall(state, state.scalar_iterator_call_count, iterator_handle,
             default_value, 0, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_equal_i32(int left_handle,
                                                   int right_handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto left_lookup = state.records.Lookup(
      left_handle, {storage::DescriptorKind::TextLiteral,
                    storage::DescriptorKind::TextOwnedUtf8,
                    storage::DescriptorKind::TextBuilder});
  auto right_lookup = state.records.Lookup(
      right_handle, {storage::DescriptorKind::TextLiteral,
                     storage::DescriptorKind::TextOwnedUtf8,
                     storage::DescriptorKind::TextBuilder});
  if (left_lookup.status != storage::LookupStatus::Ok ||
      right_lookup.status != storage::LookupStatus::Ok) {
    const int status = left_lookup.status != storage::LookupStatus::Ok
                           ? StatusForLookup(left_lookup.status)
                           : StatusForLookup(right_lookup.status);
    RecordCall(state, state.equality_call_count, 0, left_handle,
               right_handle, 0, status, 0);
    return 0;
  }
  if (!HasUtf8Storage(*left_lookup.record) ||
      !HasUtf8Storage(*right_lookup.record)) {
    RecordCall(state, state.equality_call_count, 0, left_handle,
               right_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE, 0);
    return 0;
  }
  const int result =
      left_lookup.record->utf8_bytes == right_lookup.record->utf8_bytes ? 1 : 0;
  RecordCall(state, state.equality_call_count, left_handle, left_handle,
             right_handle, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_text_compare_i32(int left_handle,
                                                     int right_handle) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  auto left_lookup = state.records.Lookup(
      left_handle, {storage::DescriptorKind::TextLiteral,
                    storage::DescriptorKind::TextOwnedUtf8,
                    storage::DescriptorKind::TextBuilder});
  auto right_lookup = state.records.Lookup(
      right_handle, {storage::DescriptorKind::TextLiteral,
                     storage::DescriptorKind::TextOwnedUtf8,
                     storage::DescriptorKind::TextBuilder});
  if (left_lookup.status != storage::LookupStatus::Ok ||
      right_lookup.status != storage::LookupStatus::Ok) {
    const int status = left_lookup.status != storage::LookupStatus::Ok
                           ? StatusForLookup(left_lookup.status)
                           : StatusForLookup(right_lookup.status);
    RecordCall(state, state.compare_call_count, 0, left_handle, right_handle, 0,
               status, 0);
    return 0;
  }
  if (!HasUtf8Storage(*left_lookup.record) ||
      !HasUtf8Storage(*right_lookup.record)) {
    RecordCall(state, state.compare_call_count, 0, left_handle, right_handle, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE, 0);
    return 0;
  }
  const auto &left = left_lookup.record->utf8_bytes;
  const auto &right = right_lookup.record->utf8_bytes;
  const int cmp = std::lexicographical_compare(left.begin(), left.end(),
                                               right.begin(), right.end())
                      ? -1
                      : (std::lexicographical_compare(right.begin(),
                                                      right.end(),
                                                      left.begin(), left.end())
                             ? 1
                             : 0);
  RecordCall(state, state.compare_call_count, left_handle, left_handle,
             right_handle, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, cmp);
  return cmp;
}

extern "C" int objc3_runtime_stdlib_text_format_i32_i32(int value) {
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  std::array<char, 16> buffer{};
  const int written =
      std::snprintf(buffer.data(), buffer.size(), "%d", value);
  if (written <= 0 || written >= static_cast<int>(buffer.size())) {
    RecordCall(state, state.format_call_count, 0, value, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUTPUT_TOO_SMALL, 0);
    return 0;
  }
  int scalar_count = 0;
  std::vector<int> scalars;
  const auto *bytes = reinterpret_cast<const unsigned char *>(buffer.data());
  if (!TryDecodeUtf8Scalars(bytes, written, scalar_count, &scalars)) {
    RecordCall(state, state.format_call_count, 0, value, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8, 0);
    return 0;
  }
  const int handle =
      CreateOwnedText(state, bytes, written, scalar_count, std::move(scalars));
  if (handle == 0) {
    RecordCall(state, state.format_call_count, 0, value, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED, 0);
    return 0;
  }
  RecordCall(state, state.format_call_count, handle, value, written,
             scalar_count, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK, handle);
  return handle;
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
  auto lookup = state.records.Lookup(
      handle, {storage::DescriptorKind::TextLiteral,
               storage::DescriptorKind::TextOwnedUtf8,
               storage::DescriptorKind::TextBuilder});
  if (lookup.status != storage::LookupStatus::Ok) {
    const int status = StatusForLookup(lookup.status);
    RecordCall(state, state.storage_query_call_count, handle, capacity, 0, 0,
               status, 0);
    return 0;
  }
  if (!HasUtf8Storage(*lookup.record)) {
    RecordCall(state, state.storage_query_call_count, handle, capacity, 0, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE, 0);
    return 0;
  }
  if (out == nullptr || capacity < lookup.record->byte_count) {
    RecordCall(state, state.storage_query_call_count, handle, capacity,
               lookup.record->byte_count, 0,
               OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUTPUT_TOO_SMALL, 0);
    return 0;
  }
  if (lookup.record->byte_count > 0) {
    std::memcpy(out, lookup.record->utf8_bytes.data(),
                static_cast<std::size_t>(lookup.record->byte_count));
  }
  RecordCall(state, state.storage_query_call_count, handle, capacity,
             lookup.record->byte_count, 0, OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK,
             lookup.record->byte_count);
  return lookup.record->byte_count;
}

extern "C" int objc3_runtime_copy_stdlib_text_state_for_testing(
    objc3_runtime_stdlib_text_snapshot *out) {
  if (out == nullptr) {
    return -1;
  }
  RuntimeStdlibTextState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  out->reset_generation = state.records.reset_generation();
  out->total_call_count = state.total_call_count;
  out->literal_call_count = state.literal_call_count;
  out->query_call_count = state.query_call_count;
  out->concat_call_count = state.concat_call_count;
  out->storage_create_call_count = state.storage_create_call_count;
  out->storage_query_call_count = state.storage_query_call_count;
  out->status_call_count = state.status_call_count;
  out->scalar_query_call_count = state.scalar_query_call_count;
  out->scalar_iterator_call_count = state.scalar_iterator_call_count;
  out->builder_create_call_count = state.builder_create_call_count;
  out->builder_append_call_count = state.builder_append_call_count;
  out->builder_finalize_call_count = state.builder_finalize_call_count;
  out->interpolation_call_count = state.interpolation_call_count;
  out->equality_call_count = state.equality_call_count;
  out->compare_call_count = state.compare_call_count;
  out->format_call_count = state.format_call_count;
  out->text_record_count = state.records.LiveRecordCount();
  out->owned_storage_record_count = CountOwnedStorageRecords(state);
  out->owned_storage_byte_count = CountOwnedStorageBytes(state);
  out->last_handle = state.last_handle;
  out->last_input_a = state.last_input_a;
  out->last_input_b = state.last_input_b;
  out->last_input_c = state.last_input_c;
  out->last_status = state.last_status;
  out->last_result = state.last_result;
  out->last_malformed_offset = state.last_malformed_offset;
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
  out->literal_record_count =
      state.records.RecordCount(storage::DescriptorKind::TextLiteral);
  out->builder_record_count =
      state.records.RecordCount(storage::DescriptorKind::TextBuilder);
  out->scalar_iterator_record_count =
      state.records.RecordCount(storage::DescriptorKind::TextScalarIterator);
  out->malformed_record_count =
      state.records.RecordCount(storage::DescriptorKind::Malformed);
  out->stale_record_count =
      static_cast<int>(state.records.stale_record_count());
  out->reserved_normalized_record_count =
      state.records.RecordCount(storage::DescriptorKind::ReservedNormalizedText);
  out->reserved_collated_record_count =
      state.records.RecordCount(storage::DescriptorKind::ReservedCollatedText);
  return 0;
}
