#include "runtime/public/objc3_runtime_api.h"

#include <array>
#include <cstring>
#include <iostream>

namespace {

int Fail(const char *message) {
  std::cerr << "string-text-model-runtime-probe: " << message << "\n";
  return 1;
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  const char mixed[] = {
      'h',
      static_cast<char>(0xC3),
      static_cast<char>(0xA9),
      static_cast<char>(0xF0),
      static_cast<char>(0x9F),
      static_cast<char>(0x9A),
      static_cast<char>(0xA6),
  };
  const int mixed_handle =
      objc3_runtime_stdlib_text_utf8_storage_i32(mixed, sizeof(mixed));
  if (mixed_handle <= 0) {
    return Fail("owned UTF-8 storage literal did not create a handle");
  }
  if (objc3_runtime_stdlib_text_byte_count_i32(mixed_handle) != 7 ||
      objc3_runtime_stdlib_text_unit_count_i32(mixed_handle) != 3 ||
      objc3_runtime_stdlib_text_scalar_count_i32(mixed_handle) != 3 ||
      objc3_runtime_stdlib_text_is_valid_utf8_i32(mixed_handle) != 1) {
    return Fail("owned UTF-8 storage metadata drifted");
  }
  if (objc3_runtime_stdlib_text_byte_at_or_i32(mixed_handle, 1, -1) != 0xC3 ||
      objc3_runtime_stdlib_text_byte_at_or_i32(mixed_handle, 6, -1) != 0xA6) {
    return Fail("owned UTF-8 byte access drifted");
  }
  if (objc3_runtime_stdlib_text_byte_at_or_i32(mixed_handle, 7, 123) != 123 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUT_OF_BOUNDS) {
    return Fail("owned UTF-8 byte access did not fail closed out of bounds");
  }

  const char suffix[] = {' ', 'l', 'a', 'n', 'e'};
  const int appended = objc3_runtime_stdlib_text_append_utf8_storage_i32(
      mixed_handle, suffix, sizeof(suffix));
  if (appended <= 0 ||
      objc3_runtime_stdlib_text_byte_count_i32(appended) != 12 ||
      objc3_runtime_stdlib_text_scalar_count_i32(appended) != 8) {
    return Fail("owned UTF-8 append did not preserve byte/scalar metadata");
  }

  const char expected[] = {
      'h',
      static_cast<char>(0xC3),
      static_cast<char>(0xA9),
      static_cast<char>(0xF0),
      static_cast<char>(0x9F),
      static_cast<char>(0x9A),
      static_cast<char>(0xA6),
      ' ',
      'l',
      'a',
      'n',
      'e',
  };
  const int expected_handle =
      objc3_runtime_stdlib_text_utf8_storage_i32(expected, sizeof(expected));
  if (expected_handle <= 0 ||
      objc3_runtime_stdlib_text_equal_i32(appended, expected_handle) != 1) {
    return Fail("owned UTF-8 equality did not compare stored bytes");
  }

  std::array<char, sizeof(expected)> copied{};
  if (objc3_runtime_copy_stdlib_text_utf8_bytes_for_testing(
          appended, copied.data(), static_cast<int>(copied.size())) !=
      static_cast<int>(sizeof(expected))) {
    return Fail("owned UTF-8 byte copy did not return the byte count");
  }
  if (std::memcmp(copied.data(), expected, sizeof(expected)) != 0) {
    return Fail("owned UTF-8 byte copy did not preserve the payload");
  }
  std::array<char, 4> too_small{};
  if (objc3_runtime_copy_stdlib_text_utf8_bytes_for_testing(
          appended, too_small.data(), static_cast<int>(too_small.size())) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUTPUT_TOO_SMALL) {
    return Fail("owned UTF-8 byte copy did not fail closed for short output");
  }

  const char overlong[] = {static_cast<char>(0xC0), static_cast<char>(0xAF)};
  if (objc3_runtime_stdlib_text_utf8_storage_i32(overlong, sizeof(overlong)) !=
          0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8) {
    return Fail("overlong UTF-8 sequence was not rejected");
  }
  const char surrogate[] = {
      static_cast<char>(0xED),
      static_cast<char>(0xA0),
      static_cast<char>(0x80),
  };
  if (objc3_runtime_stdlib_text_utf8_storage_i32(surrogate,
                                                 sizeof(surrogate)) != 0 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8) {
    return Fail("surrogate UTF-8 sequence was not rejected");
  }

  const int shape_only = objc3_runtime_stdlib_text_utf8_literal_i32(2, 2, 1);
  if (shape_only <= 0 ||
      objc3_runtime_stdlib_text_byte_at_or_i32(shape_only, 0, 77) != 77 ||
      objc3_runtime_stdlib_text_last_status_i32() !=
          OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE) {
    return Fail("shape-only text handle exposed fake storage bytes");
  }

  objc3_runtime_stdlib_text_snapshot snapshot{};
  if (objc3_runtime_copy_stdlib_text_state_for_testing(&snapshot) != 0) {
    return Fail("text snapshot copy failed");
  }
  if (snapshot.storage_create_call_count != 5 ||
      snapshot.storage_query_call_count != 7 ||
      snapshot.text_record_count != 4 ||
      snapshot.owned_storage_record_count != 3 ||
      snapshot.owned_storage_byte_count != 31) {
    return Fail("owned UTF-8 storage counters drifted");
  }

  std::cout << "{"
            << "\"storage_create_call_count\":"
            << snapshot.storage_create_call_count
            << ",\"storage_query_call_count\":"
            << snapshot.storage_query_call_count
            << ",\"text_record_count\":" << snapshot.text_record_count
            << ",\"owned_storage_record_count\":"
            << snapshot.owned_storage_record_count
            << ",\"owned_storage_byte_count\":"
            << snapshot.owned_storage_byte_count
            << ",\"last_status\":" << snapshot.last_status << "}\n";
  return 0;
}
