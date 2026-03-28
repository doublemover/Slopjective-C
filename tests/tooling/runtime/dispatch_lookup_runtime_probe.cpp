#include "runtime/objc3_runtime.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>

namespace {

constexpr std::int64_t kDispatchModulus = 2147483629LL;

std::int64_t ComputeSelectorScore(const char *selector) {
  if (selector == nullptr) {
    return 0;
  }

  std::int64_t score = 0;
  std::int64_t index = 1;
  const unsigned char *cursor =
      reinterpret_cast<const unsigned char *>(selector);
  while (*cursor != 0U) {
    score = (score + (static_cast<std::int64_t>(*cursor) * index)) %
            kDispatchModulus;
    ++cursor;
    ++index;
  }
  return score;
}

int ExpectedDispatch(int receiver, const char *selector, int a0, int a1, int a2,
                     int a3) {
  if (receiver == 0) {
    return 0;
  }

  std::int64_t value = 41;
  value += static_cast<std::int64_t>(receiver) * 97;
  value += static_cast<std::int64_t>(a0) * 7;
  value += static_cast<std::int64_t>(a1) * 11;
  value += static_cast<std::int64_t>(a2) * 13;
  value += static_cast<std::int64_t>(a3) * 17;
  value += ComputeSelectorScore(selector) * 19;
  value %= kDispatchModulus;
  if (value < 0) {
    value += kDispatchModulus;
  }
  return static_cast<int>(value);
}

void PrintJsonStringOrNull(const char *value) {
  if (value == nullptr) {
    std::printf("null");
    return;
  }
  std::printf("\"");
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(value);
       *cursor != 0U; ++cursor) {
    switch (*cursor) {
      case '\\':
        std::printf("\\\\");
        break;
      case '"':
        std::printf("\\\"");
        break;
      case '\n':
        std::printf("\\n");
        break;
      case '\r':
        std::printf("\\r");
        break;
      case '\t':
        std::printf("\\t");
        break;
      default:
        std::printf("%c", static_cast<char>(*cursor));
        break;
    }
  }
  std::printf("\"");
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  const objc3_runtime_image_descriptor image{
      "dispatch_runtime-d001-runtime-probe",
      "dispatch_runtime-d001::translation-unit",
      1,
      1,
      0,
      0,
      0,
      0,
  };

  const int register_status = objc3_runtime_register_image(&image);
  const objc3_runtime_selector_handle *null_selector =
      objc3_runtime_lookup_selector(nullptr);
  const objc3_runtime_selector_handle *copy_first =
      objc3_runtime_lookup_selector("copy");
  const objc3_runtime_selector_handle *copy_second =
      objc3_runtime_lookup_selector("copy");
  const objc3_runtime_selector_handle *gamma =
      objc3_runtime_lookup_selector("gamma");
  const unsigned long long copy_selector_stable_id =
      static_cast<unsigned long long>(
          copy_first != nullptr ? copy_first->stable_id : 0);
  const unsigned long long gamma_selector_stable_id =
      static_cast<unsigned long long>(gamma != nullptr ? gamma->stable_id : 0);
  const bool copy_selector_reused =
      copy_first != nullptr && copy_first == copy_second;
  const bool copy_selector_spelling_matches =
      copy_first != nullptr && std::strcmp(copy_first->selector, "copy") == 0;

  const int dispatch_result =
      objc3_runtime_dispatch_i32(7, "copy", 1, 2, 3, 4);
  const int expected_dispatch_result = ExpectedDispatch(7, "copy", 1, 2, 3, 4);
  const int nil_dispatch_result =
      objc3_runtime_dispatch_i32(0, "copy", 1, 2, 3, 4);

  objc3_runtime_registration_state_snapshot snapshot{};
  const int snapshot_status =
      objc3_runtime_copy_registration_state_for_testing(&snapshot);
  const std::string last_registered_module_name =
      snapshot.last_registered_module_name != nullptr
          ? snapshot.last_registered_module_name
          : "";
  const std::string last_registered_translation_unit_identity_key =
      snapshot.last_registered_translation_unit_identity_key != nullptr
          ? snapshot.last_registered_translation_unit_identity_key
          : "";

  objc3_runtime_reset_for_testing();
  const objc3_runtime_selector_handle *copy_after_reset =
      objc3_runtime_lookup_selector("copy");

  std::printf("{");
  std::printf("\"register_status\":%d,", register_status);
  std::printf("\"lookup_null_is_null\":%s,",
              null_selector == nullptr ? "true" : "false");
  std::printf("\"copy_selector_reused\":%s,",
              copy_selector_reused ? "true" : "false");
  std::printf("\"copy_selector_stable_id\":%llu,",
              copy_selector_stable_id);
  std::printf("\"gamma_selector_stable_id\":%llu,",
              gamma_selector_stable_id);
  std::printf("\"copy_selector_spelling_matches\":%s,",
              copy_selector_spelling_matches ? "true" : "false");
  std::printf("\"dispatch_result\":%d,", dispatch_result);
  std::printf("\"expected_dispatch_result\":%d,", expected_dispatch_result);
  std::printf("\"nil_dispatch_result\":%d,", nil_dispatch_result);
  std::printf("\"snapshot_status\":%d,", snapshot_status);
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf("\"registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.next_expected_registration_order_ordinal));
  std::printf("\"last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_successful_registration_order_ordinal));
  std::printf("\"last_registration_status\":%d,", snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(last_registered_module_name.c_str());
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(last_registered_translation_unit_identity_key.c_str());
  std::printf(",\"copy_after_reset_stable_id\":%llu",
              static_cast<unsigned long long>(
                  copy_after_reset != nullptr ? copy_after_reset->stable_id : 0));
  std::printf("}\n");

  return 0;
}
