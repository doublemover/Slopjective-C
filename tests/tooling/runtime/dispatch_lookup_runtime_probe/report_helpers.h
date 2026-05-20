#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_REPORT_HELPERS_H_

#include "probe_state.h"
#include "../support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline unsigned long long JsonU64(std::uint64_t value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintDispatchLookupProbeReport(
    const DispatchLookupProbeResult &result) {
  using ::objc3c::runtime::probe::PrintJsonStringOrNull;

  const objc3_runtime_registration_state_snapshot &snapshot =
      result.registration.snapshot;

  std::printf("{");
  std::printf("\"register_status\":%d,", result.register_status);
  std::printf("\"lookup_null_is_null\":%s,",
              result.selectors.lookup_null_is_null ? "true" : "false");
  std::printf("\"copy_selector_reused\":%s,",
              result.selectors.copy_selector_reused ? "true" : "false");
  std::printf("\"copy_selector_stable_id\":%llu,",
              JsonU64(result.selectors.copy_selector_stable_id));
  std::printf("\"gamma_selector_stable_id\":%llu,",
              JsonU64(result.selectors.gamma_selector_stable_id));
  std::printf("\"copy_selector_spelling_matches\":%s,",
              result.selectors.copy_selector_spelling_matches ? "true"
                                                              : "false");
  std::printf("\"dispatch_result\":%d,", result.dispatch.dispatch_result);
  std::printf("\"expected_dispatch_result\":%d,",
              result.dispatch.expected_dispatch_result);
  std::printf("\"nil_dispatch_result\":%d,",
              result.dispatch.nil_dispatch_result);
  std::printf("\"typed_super_status\":%d,",
              result.from_class.typed_super_status);
  std::printf("\"typed_super_value\":%d,",
              result.from_class.typed_super_value);
  std::printf("\"typed_super_return_kind\":%d,",
              result.from_class.typed_super_return_kind);
  std::printf("\"i32_super_status\":%d,",
              result.from_class.i32_super_status);
  std::printf("\"i32_super_value\":%d,", result.from_class.i32_super_value);
  std::printf("\"i32_super_return_kind\":%d,",
              result.from_class.i32_super_return_kind);
  std::printf("\"typed_self_status\":%d,",
              result.from_class.typed_self_status);
  std::printf("\"typed_self_value\":%d,",
              result.from_class.typed_self_value);
  std::printf("\"i32_self_status\":%d,", result.from_class.i32_self_status);
  std::printf("\"i32_self_value\":%d,", result.from_class.i32_self_value);
  std::printf("\"null_lookup_start_status\":%d,",
              result.from_class.null_lookup_start_status);
  std::printf("\"empty_lookup_start_status\":%d,",
              result.from_class.empty_lookup_start_status);
  std::printf("\"missing_lookup_start_status\":%d,",
              result.from_class.missing_lookup_start_status);
  std::printf("\"unreachable_lookup_start_status\":%d,",
              result.from_class.unreachable_lookup_start_status);
  std::printf("\"snapshot_status\":%d,",
              result.registration.snapshot_status);
  std::printf("\"registered_image_count\":%llu,",
              JsonU64(snapshot.registered_image_count));
  std::printf("\"registered_descriptor_total\":%llu,",
              JsonU64(snapshot.registered_descriptor_total));
  std::printf("\"next_expected_registration_order_ordinal\":%llu,",
              JsonU64(snapshot.next_expected_registration_order_ordinal));
  std::printf("\"last_successful_registration_order_ordinal\":%llu,",
              JsonU64(snapshot.last_successful_registration_order_ordinal));
  std::printf("\"last_registration_status\":%d,",
              snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(result.registration.last_registered_module_name.c_str());
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      result.registration.last_registered_translation_unit_identity_key
          .c_str());
  std::printf(",\"copy_after_reset_stable_id\":%llu",
              JsonU64(result.reset_lookup.copy_after_reset_stable_id));
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_REPORT_HELPERS_H_
