#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_REPORT_HELPERS_H_

#include "../support/json_probe_writer.h"
#include "runtime_initialization_helpers.h"

#include <iostream>

namespace objc3c::runtime::bootstrap_semantics_probe {

enum ProbeExitCode {
  kUsageError = 2,
  kArgumentParseError = 3,
  kAfterSuccessSnapshotError = 4,
  kAfterDuplicateSnapshotError = 5,
  kAfterOutOfOrderSnapshotError = 6,
  kAfterInvalidSnapshotError = 7,
};

inline void PrintSnapshot(const char *name, const OwnedSnapshot &snapshot,
                          bool emit_comma) {
  using objc3c::runtime::probe::JsonEscape;

  std::cout << "\"" << name << "\":{"
            << "\"registered_image_count\":"
            << snapshot.registered_image_count << ","
            << "\"registered_descriptor_total\":"
            << snapshot.registered_descriptor_total << ","
            << "\"next_expected_registration_order_ordinal\":"
            << snapshot.next_expected_registration_order_ordinal << ","
            << "\"last_successful_registration_order_ordinal\":"
            << snapshot.last_successful_registration_order_ordinal << ","
            << "\"last_registration_status\":"
            << snapshot.last_registration_status << ","
            << "\"last_registered_module_name\":\""
            << JsonEscape(snapshot.last_registered_module_name.c_str()) << "\","
            << "\"last_registered_translation_unit_identity_key\":\""
            << JsonEscape(
                   snapshot.last_registered_translation_unit_identity_key.c_str())
            << "\","
            << "\"last_rejected_module_name\":\""
            << JsonEscape(snapshot.last_rejected_module_name.c_str()) << "\","
            << "\"last_rejected_translation_unit_identity_key\":\""
            << JsonEscape(
                   snapshot.last_rejected_translation_unit_identity_key.c_str())
            << "\","
            << "\"last_rejected_registration_order_ordinal\":"
            << snapshot.last_rejected_registration_order_ordinal << "}";
  if (emit_comma) {
    std::cout << ",";
  }
}

}  // namespace objc3c::runtime::bootstrap_semantics_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_REPORT_HELPERS_H_
