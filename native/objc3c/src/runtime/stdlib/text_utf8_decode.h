#pragma once

#include <vector>

namespace objc3c::runtime::text_utf8 {

inline bool IsContinuation(unsigned char byte) {
  return byte >= 0x80 && byte <= 0xBF;
}


inline bool TryDecodeUtf8Scalars(const unsigned char *bytes,
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

}  // namespace objc3c::runtime::text_utf8
