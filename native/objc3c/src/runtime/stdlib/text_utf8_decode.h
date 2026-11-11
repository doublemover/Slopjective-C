#pragma once

#include <vector>
#include <cstdint>
#include <new>

namespace objc3c::runtime::text_utf8 {
#ifdef OBJC3_TEXT_CONSTRUCTION_TESTING
inline thread_local uint64_t decode_calls = 0, decoded_input_bytes = 0;
#endif

inline bool IsContinuation(unsigned char byte) {
  return byte >= 0x80 && byte <= 0xBF;
}


inline bool TryDecodeUtf8Scalars(const unsigned char *bytes,
                          int byte_count,
                          int &scalar_count,
                          std::vector<int> *scalars,
                          int *malformed_offset = nullptr,
                          std::vector<std::uint32_t> *offsets = nullptr) {
#ifdef OBJC3_TEXT_CONSTRUCTION_TESTING
  ++decode_calls;
  if (byte_count > 0) decoded_input_bytes += static_cast<uint64_t>(byte_count);
#endif
  scalar_count = 0;
  bool offset_allocation_failed = false;
  if (offsets) offsets->clear();
  auto append_offset = [&](std::uint32_t offset) {
    if (!offsets || offset_allocation_failed) return;
    try { offsets->push_back(offset); }
    catch (const std::bad_alloc &) { offset_allocation_failed = true; }
  };
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
    append_offset(static_cast<std::uint32_t>(index));
    index += width;
    ++scalar_count;
    if (scalars != nullptr) {
      scalars->push_back(scalar);
    }
  }
  append_offset(static_cast<std::uint32_t>(byte_count));
  // Keep malformed-input precedence even if collecting boundaries ran out of
  // memory earlier in the same pass. Valid input reports the allocation error.
  if (offset_allocation_failed) throw std::bad_alloc();
  return true;
}

}  // namespace objc3c::runtime::text_utf8
