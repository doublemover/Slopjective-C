#include "runtime/stdlib/opaque_service_storage.h"
#include "runtime/stdlib/stdlib_service_contract.h"
#include "runtime/stdlib/text_utf8_decode.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <utility>
#include <vector>

namespace {
namespace storage = objc3c::runtime::stdlib_runtime;
using objc3c::runtime::text_utf8::TryDecodeUtf8Scalars;
struct TextV2Value {
  // Published text bytes and boundaries are immutable. Registry With leases
  // keep the complete value alive while trusted operations read either table.
  std::mutex mutex;
  std::vector<unsigned char> bytes;
  std::vector<std::uint32_t> scalar_offsets;
};
struct TextBuilderV2Value {
  std::mutex mutex;
  std::vector<unsigned char> bytes;
  std::vector<std::uint32_t> scalar_offsets{0};
};
storage::OpaqueServiceStorage<TextV2Value> &TextV2Storage() {
  static storage::OpaqueServiceStorage<TextV2Value> storage;
  return storage;
}
storage::OpaqueServiceStorage<TextBuilderV2Value> &TextBuilderV2Storage() {
  static storage::OpaqueServiceStorage<TextBuilderV2Value> storage;
  return storage;
}
constexpr std::uint32_t kTextV2Kind = 1;
constexpr std::uint32_t kTextBuilderV2Kind = 2;
constexpr std::size_t kTextV2MaxBytes = 1U << 20;

int TextV2RegistryStatus(
    storage::OpaqueServiceStorage<TextV2Value>::Status status) {
  using RegistryStatus = storage::OpaqueServiceStorage<TextV2Value>::Status;
  switch (status) {
    case RegistryStatus::Ok: return OBJC3_STDLIB_V2_OK;
    case RegistryStatus::WrongKind: return OBJC3_STDLIB_V2_WRONG_KIND;
    case RegistryStatus::StaleHandle: return OBJC3_STDLIB_V2_STALE_HANDLE;
    case RegistryStatus::InvalidHandle:
    default: return OBJC3_STDLIB_V2_INVALID_HANDLE;
  }
}

int TextBuilderV2RegistryStatus(
    storage::OpaqueServiceStorage<TextBuilderV2Value>::Status status) {
  using RegistryStatus =
      storage::OpaqueServiceStorage<TextBuilderV2Value>::Status;
  switch (status) {
    case RegistryStatus::Ok: return OBJC3_STDLIB_V2_OK;
    case RegistryStatus::WrongKind: return OBJC3_STDLIB_V2_WRONG_KIND;
    case RegistryStatus::StaleHandle: return OBJC3_STDLIB_V2_STALE_HANDLE;
    case RegistryStatus::InvalidHandle:
    default: return OBJC3_STDLIB_V2_INVALID_HANDLE;
  }
}

#ifdef OBJC3_TEXT_CONSTRUCTION_TESTING
thread_local uint64_t text_copied_bytes = 0;
#endif
void RecordByteCopy(std::size_t bytes) {
#ifdef OBJC3_TEXT_CONSTRUCTION_TESTING
  text_copied_bytes += bytes;
#else
  (void)bytes;
#endif
}
template <class T>
void ReserveTextGrowth(std::vector<T> &values, std::size_t required,
                        std::size_t limit) {
  if (required <= values.capacity()) return;
  const auto grown = std::min(limit, std::max<std::size_t>(16, values.capacity() * 2));
  values.reserve(std::max(required, grown));
}
// Accept only complete, validated owned values. No unchecked caller bytes can
// reach publication without going through the authoritative decoder.
int PublishValidatedText(std::unique_ptr<TextV2Value> value, objc3_text_v2_t *out) {
  void *handle = TextV2Storage().Store(std::move(value), kTextV2Kind);
  if (!handle) return TextV2Storage().AtCapacity() ? OBJC3_STDLIB_V2_CAPACITY_EXCEEDED
                                                : OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  *out = static_cast<objc3_text_v2_t>(handle);
  return OBJC3_STDLIB_V2_OK;
}
int AppendValidatedText(objc3_text_builder_v2_t builder,
                        const unsigned char *bytes, std::size_t byte_count,
                        const std::uint32_t *offsets, std::size_t offset_count) {
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = TextBuilderV2Storage().With(builder, kTextBuilderV2Kind,
      [&](TextBuilderV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        const auto old_size = value.bytes.size();
        if (byte_count > kTextV2MaxBytes - old_size) {
          status = OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
          return;
        }
        // Reserve both tables before changing either logical sequence. A
        // failed reservation changes capacity only, never builder contents.
        const auto old_capacity = value.bytes.capacity();
        ReserveTextGrowth(value.bytes, old_size + byte_count, kTextV2MaxBytes);
        if (old_capacity != value.bytes.capacity()) RecordByteCopy(old_size);
        ReserveTextGrowth(value.scalar_offsets, value.scalar_offsets.size() + offset_count - 1,
                           kTextV2MaxBytes + 1);
        if (byte_count) value.bytes.insert(value.bytes.end(), bytes, bytes + byte_count);
        RecordByteCopy(byte_count);
        for (std::size_t i = 1; i < offset_count; ++i)
          value.scalar_offsets.push_back(static_cast<std::uint32_t>(old_size + offsets[i]));
      });
  return found ? status : TextBuilderV2RegistryStatus(
      TextBuilderV2Storage().Inspect(builder, kTextBuilderV2Kind));
}


}  // namespace

extern "C" objc3_text_v2_t objc3_runtime_stdlib_text_v2_create_utf8(
    const char *bytes, uint64_t count) {
  objc3_text_v2_t result = nullptr;
  if (objc3_runtime_stdlib_text_v2_create_utf8_checked(bytes, count,
                                                        &result) !=
      OBJC3_STDLIB_V2_OK) return nullptr;
  return result;
}

extern "C" objc3_text_v2_t objc3_runtime_stdlib_text_v2_retain(
    objc3_text_v2_t text) {
  return static_cast<objc3_text_v2_t>(
      TextV2Storage().Retain(text, kTextV2Kind));
}

extern "C" void objc3_runtime_stdlib_text_v2_release(objc3_text_v2_t text) {
  (void)TextV2Storage().Release(text, kTextV2Kind);
}

extern "C" int objc3_runtime_stdlib_text_v2_byte_count(
    objc3_text_v2_t text, uint64_t *out_count) {
  if (out_count == nullptr) return 0;
  return TextV2Storage().With(text, kTextV2Kind, [&](TextV2Value &value) {
    std::lock_guard<std::mutex> lock(value.mutex);
    *out_count = static_cast<uint64_t>(value.bytes.size());
  });
}

extern "C" int objc3_runtime_stdlib_text_v2_byte_at(
    objc3_text_v2_t text, uint64_t index, uint8_t *out_byte) {
  if (out_byte == nullptr) return 0;
  bool found = false;
  const bool valid = TextV2Storage().With(text, kTextV2Kind,
      [&](TextV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        if (index < value.bytes.size()) {
          *out_byte = value.bytes[static_cast<std::size_t>(index)];
          found = true;
        }
      });
  return valid && found;
}

extern "C" int objc3_runtime_stdlib_text_v2_create_utf8_checked(
    const char *bytes, uint64_t byte_count, objc3_text_v2_t *out_text) {
  if (!out_text) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  if (byte_count > kTextV2MaxBytes || (!bytes && byte_count))
    return byte_count > kTextV2MaxBytes ? OBJC3_STDLIB_V2_CAPACITY_EXCEEDED
                                      : OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  try {
    std::vector<std::uint32_t> offsets;
    int scalar_count = 0;
    const auto *raw = reinterpret_cast<const unsigned char *>(bytes);
    if (!TryDecodeUtf8Scalars(raw, static_cast<int>(byte_count), scalar_count,
                               nullptr, nullptr, &offsets))
      return OBJC3_STDLIB_V2_INVALID_UTF8;
    auto value = std::make_unique<TextV2Value>();
    if (byte_count) value->bytes.assign(raw, raw + byte_count);
    RecordByteCopy(byte_count);
    value->scalar_offsets = std::move(offsets);
    return PublishValidatedText(std::move(value), out_text);
  } catch (const std::bad_alloc &) { return OBJC3_STDLIB_V2_OUT_OF_MEMORY; }
}

extern "C" int objc3_runtime_stdlib_text_v2_retain_checked(
    objc3_text_v2_t text, objc3_text_v2_t *out_text) {
  if (out_text == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  if (TextV2Storage().Retain(text, kTextV2Kind) == nullptr) {
    return TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
  }
  *out_text = text;
  return OBJC3_STDLIB_V2_OK;
}

extern "C" int objc3_runtime_stdlib_text_v2_release_checked(
    objc3_text_v2_t text) {
  if (!TextV2Storage().Release(text, kTextV2Kind)) {
    return TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
  }
  return OBJC3_STDLIB_V2_OK;
}

extern "C" int objc3_runtime_stdlib_text_v2_byte_count_checked(
    objc3_text_v2_t text, uint64_t *out_count) {
  if (out_count == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_count = 0;
  const bool found = TextV2Storage().With(text, kTextV2Kind,
      [&](TextV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        *out_count = value.bytes.size();
      });
  return found ? OBJC3_STDLIB_V2_OK :
      TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
}

extern "C" int objc3_runtime_stdlib_text_v2_byte_at_checked(
    objc3_text_v2_t text, uint64_t index, uint8_t *out_byte) {
  if (out_byte == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_byte = 0;
  bool in_bounds = false;
  const bool found = TextV2Storage().With(text, kTextV2Kind,
      [&](TextV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        if (index < value.bytes.size()) {
          *out_byte = value.bytes[static_cast<std::size_t>(index)];
          in_bounds = true;
        }
      });
  if (!found) return TextV2RegistryStatus(
      TextV2Storage().Inspect(text, kTextV2Kind));
  return in_bounds ? OBJC3_STDLIB_V2_OK : OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
}

extern "C" int objc3_runtime_stdlib_text_v2_scalar_count_checked(
    objc3_text_v2_t text, uint64_t *out_count) {
  if (out_count == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_count = 0;
  const bool found = TextV2Storage().With(text, kTextV2Kind,
      [&](TextV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        *out_count = static_cast<uint64_t>(value.scalar_offsets.size() - 1);
      });
  return found ? OBJC3_STDLIB_V2_OK :
      TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
}

extern "C" int objc3_runtime_stdlib_text_v2_slice_scalars(
    objc3_text_v2_t text, uint64_t start_scalar, uint64_t scalar_count,
    objc3_text_v2_t *out_text) {
  if (!out_text) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  try {
    std::unique_ptr<TextV2Value> result;
    int status = OBJC3_STDLIB_V2_OK;
    if (!TextV2Storage().With(text, kTextV2Kind, [&](TextV2Value &value) {
      const auto available = value.scalar_offsets.size() - 1;
      if (start_scalar > available || scalar_count > available - start_scalar) {
        status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
        return;
      }
      const auto first = value.scalar_offsets[start_scalar];
      const auto last = value.scalar_offsets[start_scalar + scalar_count];
      result = std::make_unique<TextV2Value>();
      result->bytes.assign(value.bytes.begin() + first, value.bytes.begin() + last);
      RecordByteCopy(last - first);
      result->scalar_offsets.reserve(scalar_count + 1);
      for (uint64_t i = 0; i <= scalar_count; ++i)
        result->scalar_offsets.push_back(value.scalar_offsets[start_scalar + i] - first);
    })) return TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
    if (status != OBJC3_STDLIB_V2_OK) return status;
    return PublishValidatedText(std::move(result), out_text);
  } catch (const std::bad_alloc &) { return OBJC3_STDLIB_V2_OUT_OF_MEMORY; }
}

extern "C" int objc3_runtime_stdlib_text_v2_find_bytes(
    objc3_text_v2_t text, const char *needle, uint64_t needle_byte_count,
    uint64_t start_byte, int *out_found, uint64_t *out_byte_index) {
  if (out_found == nullptr || out_byte_index == nullptr ||
      (needle == nullptr && needle_byte_count != 0))
    return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_found = 0;
  *out_byte_index = 0;
  if (needle_byte_count > kTextV2MaxBytes)
    return OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
  int needle_scalars = 0;
  if (!TryDecodeUtf8Scalars(reinterpret_cast<const unsigned char *>(needle),
                            static_cast<int>(needle_byte_count),
                            needle_scalars, nullptr))
    return OBJC3_STDLIB_V2_INVALID_UTF8;
  bool result_found = false;
  int status = OBJC3_STDLIB_V2_OK;
  const bool found_handle = TextV2Storage().With(text, kTextV2Kind,
      [&](TextV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        if (start_byte > value.bytes.size()) {
          status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
          return;
        }
        const auto first = value.bytes.begin() +
                           static_cast<std::ptrdiff_t>(start_byte);
        if (needle_byte_count == 0) {
          result_found = true;
          *out_byte_index = start_byte;
          return;
        }
        const auto needle_first = reinterpret_cast<const unsigned char *>(needle);
        const auto needle_last = needle_first +
                                 static_cast<std::size_t>(needle_byte_count);
        const auto match = std::search(first, value.bytes.end(), needle_first,
                                       needle_last);
        if (match != value.bytes.end()) {
          result_found = true;
          *out_byte_index = static_cast<uint64_t>(
              std::distance(value.bytes.begin(), match));
        }
      });
  if (!found_handle) return TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
  if (status != OBJC3_STDLIB_V2_OK) return status;
  *out_found = result_found;
  return OBJC3_STDLIB_V2_OK;
}

extern "C" int objc3_runtime_stdlib_text_v2_find_scalars(
    objc3_text_v2_t text, const char *needle, uint64_t needle_byte_count,
    uint64_t start_scalar, int *out_found, uint64_t *out_scalar_index) {
  if (out_found == nullptr || out_scalar_index == nullptr ||
      (needle == nullptr && needle_byte_count != 0))
    return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_found = 0;
  *out_scalar_index = 0;
  if (needle_byte_count > kTextV2MaxBytes)
    return OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;

  int needle_scalar_count = 0;
  if (!TryDecodeUtf8Scalars(
          reinterpret_cast<const unsigned char *>(needle),
          static_cast<int>(needle_byte_count), needle_scalar_count, nullptr))
    return OBJC3_STDLIB_V2_INVALID_UTF8;

  int status = OBJC3_STDLIB_V2_OK;
  const bool found_handle = TextV2Storage().With(text, kTextV2Kind,
      [&](TextV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        const auto &haystack = value.bytes;
        const auto &scalar_offsets = value.scalar_offsets;
        const uint64_t scalar_count = scalar_offsets.size() - 1;
        if (start_scalar > scalar_count) {
          status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
          return;
        }
        if (needle_byte_count == 0) {
          *out_found = 1;
          *out_scalar_index = start_scalar;
          return;
        }

        const auto *needle_bytes =
            reinterpret_cast<const unsigned char *>(needle);
        auto search_begin = haystack.begin() + scalar_offsets[start_scalar];
        while (search_begin != haystack.end()) {
          const auto match = std::search(
              search_begin, haystack.end(), needle_bytes,
              needle_bytes + static_cast<std::size_t>(needle_byte_count));
          if (match == haystack.end())
            break;
          const std::size_t first_byte =
              static_cast<std::size_t>(std::distance(haystack.begin(), match));
          const std::size_t last_byte = first_byte +
              static_cast<std::size_t>(needle_byte_count);
          const auto first_scalar = std::lower_bound(
              scalar_offsets.begin() + static_cast<std::ptrdiff_t>(start_scalar),
              scalar_offsets.end(), first_byte);
          const auto last_scalar = std::lower_bound(
              scalar_offsets.begin(), scalar_offsets.end(), last_byte);
          if (first_scalar != scalar_offsets.end() && *first_scalar == first_byte &&
              last_scalar != scalar_offsets.end() && *last_scalar == last_byte) {
            *out_found = 1;
            *out_scalar_index = static_cast<uint64_t>(
                std::distance(scalar_offsets.begin(), first_scalar));
            return;
          }
          search_begin = match + 1;
        }
      });
  return found_handle ? status :
      TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
}

extern "C" int objc3_runtime_stdlib_text_v2_concat(
    objc3_text_v2_t left, objc3_text_v2_t right, objc3_text_v2_t *out_text) {
  if (!out_text) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  try {
    std::unique_ptr<TextV2Value> result;
    int status = OBJC3_STDLIB_V2_OK;
    bool right_found = false;
    if (!TextV2Storage().With(left, kTextV2Kind, [&](TextV2Value &lhs) {
      // Nested registry leases protect both immutable operands, including
      // concat(x,x), without taking two arbitrarily ordered object mutexes.
      right_found = TextV2Storage().With(right, kTextV2Kind, [&](TextV2Value &rhs) {
        const auto prefix = lhs.bytes.size();
        if (rhs.bytes.size() > kTextV2MaxBytes - prefix) {
          status = OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
          return;
        }
        result = std::make_unique<TextV2Value>();
        result->bytes.reserve(prefix + rhs.bytes.size());
        result->bytes.insert(result->bytes.end(), lhs.bytes.begin(), lhs.bytes.end());
        result->bytes.insert(result->bytes.end(), rhs.bytes.begin(), rhs.bytes.end());
        RecordByteCopy(prefix + rhs.bytes.size());
        result->scalar_offsets.reserve(lhs.scalar_offsets.size() + rhs.scalar_offsets.size() - 1);
        result->scalar_offsets.insert(result->scalar_offsets.end(), lhs.scalar_offsets.begin(), lhs.scalar_offsets.end());
        for (std::size_t i = 1; i < rhs.scalar_offsets.size(); ++i)
          result->scalar_offsets.push_back(static_cast<std::uint32_t>(prefix + rhs.scalar_offsets[i]));
      });
    })) return TextV2RegistryStatus(TextV2Storage().Inspect(left, kTextV2Kind));
    if (!right_found) return TextV2RegistryStatus(TextV2Storage().Inspect(right, kTextV2Kind));
    if (status != OBJC3_STDLIB_V2_OK) return status;
    return PublishValidatedText(std::move(result), out_text);
  } catch (const std::bad_alloc &) { return OBJC3_STDLIB_V2_OUT_OF_MEMORY; }
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_create(
    objc3_text_builder_v2_t *out_builder) {
  if (out_builder == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_builder = nullptr;
  try {
    auto value = std::make_unique<TextBuilderV2Value>();
    void *handle = TextBuilderV2Storage().Store(std::move(value),
                                               kTextBuilderV2Kind);
    if (handle == nullptr) {
      return TextBuilderV2Storage().AtCapacity()
          ? OBJC3_STDLIB_V2_CAPACITY_EXCEEDED : OBJC3_STDLIB_V2_OUT_OF_MEMORY;
    }
    *out_builder = static_cast<objc3_text_builder_v2_t>(handle);
    return OBJC3_STDLIB_V2_OK;
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_append_utf8(
    objc3_text_builder_v2_t builder, const char *bytes, uint64_t byte_count) {
  if (byte_count > kTextV2MaxBytes || (!bytes && byte_count))
    return byte_count > kTextV2MaxBytes ? OBJC3_STDLIB_V2_CAPACITY_EXCEEDED
                                      : OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  try {
    std::vector<std::uint32_t> offsets;
    int scalar_count = 0;
    const auto *raw = reinterpret_cast<const unsigned char *>(bytes);
    if (!TryDecodeUtf8Scalars(raw, static_cast<int>(byte_count), scalar_count,
                               nullptr, nullptr, &offsets))
      return OBJC3_STDLIB_V2_INVALID_UTF8;
    return AppendValidatedText(builder, raw, byte_count, offsets.data(), offsets.size());
  } catch (const std::bad_alloc &) { return OBJC3_STDLIB_V2_OUT_OF_MEMORY; }
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_append_scalar(
    objc3_text_builder_v2_t builder, uint32_t scalar) {
  if (scalar > 0x10ffff || (scalar >= 0xd800 && scalar <= 0xdfff))
    return OBJC3_STDLIB_V2_INVALID_ARGUMENT;

  char bytes[4];
  std::size_t byte_count = 0;
  if (scalar <= 0x7f) {
    bytes[byte_count++] = static_cast<char>(scalar);
  } else if (scalar <= 0x7ff) {
    bytes[byte_count++] = static_cast<char>(0xc0 | (scalar >> 6));
    bytes[byte_count++] = static_cast<char>(0x80 | (scalar & 0x3f));
  } else if (scalar <= 0xffff) {
    bytes[byte_count++] = static_cast<char>(0xe0 | (scalar >> 12));
    bytes[byte_count++] = static_cast<char>(0x80 | ((scalar >> 6) & 0x3f));
    bytes[byte_count++] = static_cast<char>(0x80 | (scalar & 0x3f));
  } else {
    bytes[byte_count++] = static_cast<char>(0xf0 | (scalar >> 18));
    bytes[byte_count++] = static_cast<char>(0x80 | ((scalar >> 12) & 0x3f));
    bytes[byte_count++] = static_cast<char>(0x80 | ((scalar >> 6) & 0x3f));
    bytes[byte_count++] = static_cast<char>(0x80 | (scalar & 0x3f));
  }
  const std::uint32_t offsets[] = {0, static_cast<std::uint32_t>(byte_count)};
  try {
    return AppendValidatedText(builder, reinterpret_cast<const unsigned char *>(bytes),
                               byte_count, offsets, 2);
  } catch (const std::bad_alloc &) { return OBJC3_STDLIB_V2_OUT_OF_MEMORY; }
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_append_text(
    objc3_text_builder_v2_t builder, objc3_text_v2_t text) {
  try {
    int status = OBJC3_STDLIB_V2_OK;
    if (!TextV2Storage().With(text, kTextV2Kind, [&](TextV2Value &value) {
      status = AppendValidatedText(builder, value.bytes.data(), value.bytes.size(),
                                    value.scalar_offsets.data(), value.scalar_offsets.size());
    })) return TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
    return status;
  } catch (const std::bad_alloc &) { return OBJC3_STDLIB_V2_OUT_OF_MEMORY; }
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_finish(
    objc3_text_builder_v2_t builder, objc3_text_v2_t *out_text) {
  if (!out_text) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  try {
    std::unique_ptr<TextV2Value> result;
    if (!TextBuilderV2Storage().With(builder, kTextBuilderV2Kind, [&](TextBuilderV2Value &value) {
      std::lock_guard<std::mutex> lock(value.mutex);
      result = std::make_unique<TextV2Value>();
      result->bytes = value.bytes;
      RecordByteCopy(value.bytes.size());
      result->scalar_offsets = value.scalar_offsets;
    })) return TextBuilderV2RegistryStatus(TextBuilderV2Storage().Inspect(builder, kTextBuilderV2Kind));
    return PublishValidatedText(std::move(result), out_text);
  } catch (const std::bad_alloc &) { return OBJC3_STDLIB_V2_OUT_OF_MEMORY; }
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_reset(
    objc3_text_builder_v2_t builder) {
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = TextBuilderV2Storage().With(builder, kTextBuilderV2Kind,
      [&](TextBuilderV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        std::vector<unsigned char>().swap(value.bytes);
        value.scalar_offsets.resize(1);
        value.scalar_offsets[0] = 0;
      });
  if (!found) status = TextBuilderV2RegistryStatus(
      TextBuilderV2Storage().Inspect(builder, kTextBuilderV2Kind));
  return status;
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_release(
    objc3_text_builder_v2_t builder) {
  if (!TextBuilderV2Storage().Release(builder, kTextBuilderV2Kind)) {
    return TextBuilderV2RegistryStatus(
        TextBuilderV2Storage().Inspect(builder, kTextBuilderV2Kind));
  }
  return OBJC3_STDLIB_V2_OK;
}

#ifdef OBJC3_TEXT_CONSTRUCTION_TESTING
extern "C" void objc3_test_text_construction_reset() {
  text_copied_bytes = 0;
  objc3c::runtime::text_utf8::decode_calls = 0;
  objc3c::runtime::text_utf8::decoded_input_bytes = 0;
}
extern "C" uint64_t objc3_test_text_copied_bytes() { return text_copied_bytes; }
extern "C" uint64_t objc3_test_text_decode_calls() { return objc3c::runtime::text_utf8::decode_calls; }
extern "C" uint64_t objc3_test_text_decoded_input_bytes() { return objc3c::runtime::text_utf8::decoded_input_bytes; }
#endif
