#include "runtime/public/objc3_runtime_api.h"
#include "support/dispatch_expectations.h"
#include "support/json_probe_writer.h"

#include <cstdint>
#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::ExpectedDispatch;

using objc3c::runtime::probe::PrintJsonStringOrNull;

} // namespace

int main() {
  objc3_runtime_reset_for_testing();

  objc3_runtime_registration_state_snapshot initial_snapshot{};
  const int initial_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&initial_snapshot);

  const objc3_runtime_image_descriptor image{
      "runtimeBootstrapApiProbe",
      "runtime-bootstrap-api-probe::translation-unit",
      1,
      1,
      1,
      1,
      1,
      1,
  };
  const int register_status = objc3_runtime_register_image(&image);

  const char *const selector = "bootstrap:ready:";
  const objc3_runtime_selector_handle *selector_handle =
      objc3_runtime_lookup_selector(selector);
  const objc3_runtime_dispatch_i32_result dispatch =
      objc3_runtime_dispatch_i32_checked(5, selector, 1, 2, 3, 4);
  const int dispatch_result = dispatch.value;

  objc3_runtime_registration_state_snapshot post_register_snapshot{};
  const int post_register_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &post_register_snapshot);
  const std::string post_register_module_name =
      post_register_snapshot.last_registered_module_name != nullptr
          ? post_register_snapshot.last_registered_module_name
          : "";
  const std::string post_register_translation_unit_identity_key =
      post_register_snapshot.last_registered_translation_unit_identity_key !=
              nullptr
          ? post_register_snapshot.last_registered_translation_unit_identity_key
          : "";

  objc3_runtime_reset_for_testing();

  objc3_runtime_registration_state_snapshot post_reset_snapshot{};
  const int post_reset_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_reset_snapshot);
  const objc3_runtime_selector_handle *selector_after_reset =
      objc3_runtime_lookup_selector(selector);

  std::printf("{");
  std::printf("\"initial_copy_status\":%d,", initial_copy_status);
  std::printf("\"register_status\":%d,", register_status);
  std::printf("\"post_register_copy_status\":%d,", post_register_copy_status);
  std::printf("\"selector_stable_id\":%llu,",
              static_cast<unsigned long long>(
                  selector_handle != nullptr ? selector_handle->stable_id : 0));
  std::printf("\"dispatch_result\":%d,", dispatch_result);
  std::printf("\"expected_dispatch_result\":%d,",
              ExpectedDispatch(5, selector, 1, 2, 3, 4));
  std::printf("\"post_register_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_register_snapshot.registered_image_count));
  std::printf("\"post_register_registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(
                  post_register_snapshot.registered_descriptor_total));
  std::printf(
      "\"post_register_next_expected_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(
          post_register_snapshot.next_expected_registration_order_ordinal));
  std::printf("\"post_register_last_registration_status\":%d,",
              post_register_snapshot.last_registration_status);
  std::printf("\"post_register_last_registered_module_name\":");
  PrintJsonStringOrNull(post_register_module_name.c_str());
  std::printf(
      ",\"post_register_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(post_register_translation_unit_identity_key.c_str());
  std::printf(
      ",\"post_register_last_successful_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(
          post_register_snapshot.last_successful_registration_order_ordinal));
  std::printf("\"post_reset_copy_status\":%d,", post_reset_copy_status);
  std::printf("\"post_reset_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_snapshot.registered_image_count));
  std::printf("\"post_reset_registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_snapshot.registered_descriptor_total));
  std::printf(
      "\"post_reset_next_expected_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(
          post_reset_snapshot.next_expected_registration_order_ordinal));
  std::printf("\"post_reset_last_registration_status\":%d,",
              post_reset_snapshot.last_registration_status);
  std::printf("\"post_reset_last_registered_module_name\":");
  PrintJsonStringOrNull(post_reset_snapshot.last_registered_module_name);
  std::printf(",\"post_reset_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_reset_snapshot.last_registered_translation_unit_identity_key);
  std::printf(
      ",\"post_reset_last_successful_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(
          post_reset_snapshot.last_successful_registration_order_ordinal));
  std::printf(
      "\"selector_after_reset_stable_id\":%llu",
      static_cast<unsigned long long>(selector_after_reset != nullptr
                                          ? selector_after_reset->stable_id
                                          : 0));
  std::printf("}\n");

  return 0;
}
