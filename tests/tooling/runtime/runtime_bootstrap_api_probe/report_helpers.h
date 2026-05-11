#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_REPORT_HELPERS_H_

#include "probe_state.h"
#include "../support/json_probe_writer.h"

#include <cstdint>
#include <cstdio>

namespace objc3c::runtime::probe::runtime_bootstrap_api {

inline unsigned long long JsonU64(std::uint64_t value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintPostRegisterReportFields(
    const RegistrationStateObservation &registration) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  const objc3_runtime_registration_state_snapshot &snapshot =
      registration.snapshot;

  PrintUint64Field("post_register_registered_image_count",
                   JsonU64(snapshot.registered_image_count));
  PrintUint64Field("post_register_registered_descriptor_total",
                   JsonU64(snapshot.registered_descriptor_total));
  PrintUint64Field(
      "post_register_next_expected_registration_order_ordinal",
      JsonU64(snapshot.next_expected_registration_order_ordinal));
  PrintIntField("post_register_last_registration_status",
                snapshot.last_registration_status);
  PrintStringField("post_register_last_registered_module_name",
                   registration.last_registered_module_name.value.c_str());
  PrintStringField(
      "post_register_last_registered_translation_unit_identity_key",
      registration.last_registered_translation_unit_identity_key.value.c_str());
  PrintUint64Field(
      "post_register_last_successful_registration_order_ordinal",
      JsonU64(snapshot.last_successful_registration_order_ordinal));
}

inline void PrintPostResetReportFields(
    const RegistrationStateObservation &registration) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  const objc3_runtime_registration_state_snapshot &snapshot =
      registration.snapshot;

  PrintIntField("post_reset_copy_status", registration.copy_status);
  PrintUint64Field("post_reset_registered_image_count",
                   JsonU64(snapshot.registered_image_count));
  PrintUint64Field("post_reset_registered_descriptor_total",
                   JsonU64(snapshot.registered_descriptor_total));
  PrintUint64Field(
      "post_reset_next_expected_registration_order_ordinal",
      JsonU64(snapshot.next_expected_registration_order_ordinal));
  PrintIntField("post_reset_last_registration_status",
                snapshot.last_registration_status);
  PrintStringField("post_reset_last_registered_module_name",
                   registration.last_registered_module_name.c_str_or_null());
  PrintStringField(
      "post_reset_last_registered_translation_unit_identity_key",
      registration.last_registered_translation_unit_identity_key.c_str_or_null());
  PrintUint64Field(
      "post_reset_last_successful_registration_order_ordinal",
      JsonU64(snapshot.last_successful_registration_order_ordinal));
}

inline void PrintBootstrapApiProbeReport(
    const BootstrapApiProbeResult &result) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  const BootstrapApiScenario &bootstrap = result.bootstrap;
  const ResetScenario &reset = result.reset;

  std::printf("{");
  PrintIntField("initial_copy_status", bootstrap.initial_copy_status);
  PrintIntField("register_status", bootstrap.register_status);
  PrintIntField("post_register_copy_status",
                bootstrap.post_register_registration.copy_status);
  PrintUint64Field("selector_stable_id",
                   JsonU64(bootstrap.selector_stable_id));
  PrintIntField("dispatch_result", bootstrap.dispatch_result);
  PrintIntField("expected_dispatch_result",
                bootstrap.expected_dispatch_result);
  PrintPostRegisterReportFields(bootstrap.post_register_registration);
  PrintPostResetReportFields(reset.post_reset_registration);
  PrintUint64Field("selector_after_reset_stable_id",
                   JsonU64(reset.selector_after_reset_stable_id), false);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_REPORT_HELPERS_H_
