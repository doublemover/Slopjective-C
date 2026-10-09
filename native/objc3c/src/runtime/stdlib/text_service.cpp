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
  std::mutex mutex;
  std::vector<unsigned char> bytes;
  std::vector<std::uint32_t> scalar_offsets;
};
struct TextBuilderV2Value {
  std::mutex mutex;
  std::vector<unsigned char> bytes;
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

std::unique_ptr<TextV2Value> MakeTextV2Value(
    const unsigned char *bytes, std::size_t byte_count) {
  int scalar_count = 0;
  if (!TryDecodeUtf8Scalars(bytes, static_cast<int>(byte_count), scalar_count,
                            nullptr)) return nullptr;
  auto value = std::make_unique<TextV2Value>();
  if (byte_count != 0) value->bytes.assign(bytes, bytes + byte_count);
  value->scalar_offsets.reserve(static_cast<std::size_t>(scalar_count) + 1);
  std::size_t index = 0;
  while (index < byte_count) {
    value->scalar_offsets.push_back(static_cast<std::uint32_t>(index));
    const unsigned char first = bytes[index];
    index += first <= 0x7f ? 1 : first <= 0xdf ? 2 : first <= 0xef ? 3 : 4;
  }
  value->scalar_offsets.push_back(static_cast<std::uint32_t>(byte_count));
  return value;
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
  if (out_text == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  if (byte_count > kTextV2MaxBytes || (bytes == nullptr && byte_count != 0))
    return byte_count > kTextV2MaxBytes ? OBJC3_STDLIB_V2_CAPACITY_EXCEEDED
                                        : OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  const auto *raw = reinterpret_cast<const unsigned char *>(bytes);
  int scalar_count = 0;
  if (!TryDecodeUtf8Scalars(raw, static_cast<int>(byte_count), scalar_count,
                            nullptr)) return OBJC3_STDLIB_V2_INVALID_UTF8;
  try {
    auto value = MakeTextV2Value(raw, static_cast<std::size_t>(byte_count));
    if (!value) return OBJC3_STDLIB_V2_INVALID_UTF8;
    void *handle = TextV2Storage().Store(std::move(value), kTextV2Kind);
    if (handle == nullptr) {
      return TextV2Storage().AtCapacity() ? OBJC3_STDLIB_V2_CAPACITY_EXCEEDED
                                          : OBJC3_STDLIB_V2_OUT_OF_MEMORY;
    }
    *out_text = static_cast<objc3_text_v2_t>(handle);
    return OBJC3_STDLIB_V2_OK;
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
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
  if (out_text == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  std::vector<unsigned char> bytes;
  int status = OBJC3_STDLIB_V2_OK;
  bool found = false;
  try {
    found = TextV2Storage().With(text, kTextV2Kind,
        [&](TextV2Value &value) {
          std::lock_guard<std::mutex> lock(value.mutex);
          const uint64_t available = value.scalar_offsets.size() - 1;
          if (start_scalar > available || scalar_count > available - start_scalar) {
            status = OBJC3_STDLIB_V2_OUT_OF_BOUNDS;
            return;
          }
          const std::size_t first = value.scalar_offsets[start_scalar];
          const std::size_t last = value.scalar_offsets[start_scalar + scalar_count];
          bytes.assign(value.bytes.begin() + static_cast<std::ptrdiff_t>(first),
                       value.bytes.begin() + static_cast<std::ptrdiff_t>(last));
        });
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
  if (!found) return TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
  if (status != OBJC3_STDLIB_V2_OK) return status;
  return objc3_runtime_stdlib_text_v2_create_utf8_checked(
      reinterpret_cast<const char *>(bytes.data()), bytes.size(), out_text);
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

extern "C" int objc3_runtime_stdlib_text_v2_concat(
    objc3_text_v2_t left, objc3_text_v2_t right,
    objc3_text_v2_t *out_text) {
  if (out_text == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  try {
    std::vector<unsigned char> left_bytes;
    std::vector<unsigned char> right_bytes;
    if (!TextV2Storage().With(left, kTextV2Kind, [&](TextV2Value &value) {
          std::lock_guard<std::mutex> lock(value.mutex);
          left_bytes = value.bytes;
        })) return TextV2RegistryStatus(TextV2Storage().Inspect(left, kTextV2Kind));
    if (!TextV2Storage().With(right, kTextV2Kind, [&](TextV2Value &value) {
          std::lock_guard<std::mutex> lock(value.mutex);
          right_bytes = value.bytes;
        })) return TextV2RegistryStatus(TextV2Storage().Inspect(right, kTextV2Kind));
    if (left_bytes.size() > kTextV2MaxBytes - right_bytes.size())
      return OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
    std::vector<unsigned char> combined;
    combined.reserve(left_bytes.size() + right_bytes.size());
    combined.insert(combined.end(), left_bytes.begin(), left_bytes.end());
    combined.insert(combined.end(), right_bytes.begin(), right_bytes.end());
    return objc3_runtime_stdlib_text_v2_create_utf8_checked(
        reinterpret_cast<const char *>(combined.data()), combined.size(), out_text);
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
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
  if (byte_count > kTextV2MaxBytes || (bytes == nullptr && byte_count != 0))
    return byte_count > kTextV2MaxBytes ? OBJC3_STDLIB_V2_CAPACITY_EXCEEDED
                                        : OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  int scalar_count = 0;
  if (!TryDecodeUtf8Scalars(reinterpret_cast<const unsigned char *>(bytes),
                            static_cast<int>(byte_count), scalar_count, nullptr))
    return OBJC3_STDLIB_V2_INVALID_UTF8;
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = TextBuilderV2Storage().With(builder, kTextBuilderV2Kind,
      [&](TextBuilderV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        if (byte_count > kTextV2MaxBytes - value.bytes.size()) {
          status = OBJC3_STDLIB_V2_CAPACITY_EXCEEDED;
          return;
        }
        try {
          value.bytes.reserve(value.bytes.size() +
                              static_cast<std::size_t>(byte_count));
          if (byte_count != 0) {
            const auto *first = reinterpret_cast<const unsigned char *>(bytes);
            value.bytes.insert(value.bytes.end(), first,
                               first + static_cast<std::size_t>(byte_count));
          }
        } catch (const std::bad_alloc &) {
          status = OBJC3_STDLIB_V2_OUT_OF_MEMORY;
        }
      });
  return found ? status : TextBuilderV2RegistryStatus(
      TextBuilderV2Storage().Inspect(builder, kTextBuilderV2Kind));
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_append_text(
    objc3_text_builder_v2_t builder, objc3_text_v2_t text) {
  try {
    std::vector<unsigned char> bytes;
    if (!TextV2Storage().With(text, kTextV2Kind, [&](TextV2Value &value) {
          std::lock_guard<std::mutex> lock(value.mutex);
          bytes = value.bytes;
        })) return TextV2RegistryStatus(TextV2Storage().Inspect(text, kTextV2Kind));
    return objc3_runtime_stdlib_text_builder_v2_append_utf8(
        builder, reinterpret_cast<const char *>(bytes.data()), bytes.size());
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_finish(
    objc3_text_builder_v2_t builder, objc3_text_v2_t *out_text) {
  if (out_text == nullptr) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = nullptr;
  try {
    std::vector<unsigned char> bytes;
    if (!TextBuilderV2Storage().With(builder, kTextBuilderV2Kind,
        [&](TextBuilderV2Value &value) {
          std::lock_guard<std::mutex> lock(value.mutex);
          bytes = value.bytes;
        })) return TextBuilderV2RegistryStatus(
            TextBuilderV2Storage().Inspect(builder, kTextBuilderV2Kind));
    return objc3_runtime_stdlib_text_v2_create_utf8_checked(
        reinterpret_cast<const char *>(bytes.data()), bytes.size(), out_text);
  } catch (const std::bad_alloc &) {
    return OBJC3_STDLIB_V2_OUT_OF_MEMORY;
  }
}

extern "C" int objc3_runtime_stdlib_text_builder_v2_reset(
    objc3_text_builder_v2_t builder) {
  int status = OBJC3_STDLIB_V2_OK;
  const bool found = TextBuilderV2Storage().With(builder, kTextBuilderV2Kind,
      [&](TextBuilderV2Value &value) {
        std::lock_guard<std::mutex> lock(value.mutex);
        std::vector<unsigned char>().swap(value.bytes);
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
