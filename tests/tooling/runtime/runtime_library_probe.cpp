#include "runtime/public/objc3_runtime_api.h"
#include "support/dispatch_expectations.h"

#include <cstdint>
#include <cstring>

namespace {

using objc3c::runtime::probe::ExpectedDispatch;

} // namespace

int main() {
  objc3_runtime_reset_for_testing();

  const objc3_runtime_image_descriptor image{
      "probe-module", "probe-tu-identity", 1, 1, 2, 3, 4, 5,
  };
  if (objc3_runtime_register_image(&image) != 0) {
    return 10;
  }

  const objc3_runtime_selector_handle *first =
      objc3_runtime_lookup_selector("alpha:beta:");
  const objc3_runtime_selector_handle *second =
      objc3_runtime_lookup_selector("alpha:beta:");
  const objc3_runtime_selector_handle *third =
      objc3_runtime_lookup_selector("gamma");
  if (first == nullptr || second == nullptr || third == nullptr) {
    return 11;
  }
  if (first != second) {
    return 12;
  }
  if (first->stable_id != 1 || third->stable_id != 2) {
    return 13;
  }
  if (std::strcmp(first->selector, "alpha:beta:") != 0) {
    return 14;
  }

  const objc3_runtime_dispatch_i32_result dispatch =
      objc3_runtime_dispatch_i32_checked(5, "alpha:beta:", 1, 2, 3, 4);
  const int actual = dispatch.value;
  const int expected = ExpectedDispatch(5, "alpha:beta:", 1, 2, 3, 4);
  if (actual != expected) {
    return 15;
  }

  objc3_runtime_registration_state_snapshot snapshot{};
  if (objc3_runtime_copy_registration_state_for_testing(&snapshot) != 0) {
    return 16;
  }
  if (snapshot.registered_image_count != 1 ||
      snapshot.registered_descriptor_total != 15 ||
      snapshot.next_expected_registration_order_ordinal != 2 ||
      snapshot.last_successful_registration_order_ordinal != 1 ||
      snapshot.last_registration_status !=
          OBJC3_RUNTIME_REGISTRATION_STATUS_OK) {
    return 17;
  }
  if (snapshot.last_registered_module_name == nullptr ||
      std::strcmp(snapshot.last_registered_module_name, "probe-module") != 0 ||
      snapshot.last_registered_translation_unit_identity_key == nullptr ||
      std::strcmp(snapshot.last_registered_translation_unit_identity_key,
                  "probe-tu-identity") != 0) {
    return 18;
  }

  objc3_runtime_reset_for_testing();
  const objc3_runtime_selector_handle *after_reset =
      objc3_runtime_lookup_selector("alpha:beta:");
  if (after_reset == nullptr || after_reset->stable_id != 1) {
    return 19;
  }

  return 0;
}
