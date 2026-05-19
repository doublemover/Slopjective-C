#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_REPORT_ERROR_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_REPORT_ERROR_HELPERS_H_

#include "cross_module_runtime_packaging_probe/probe_result.h"
#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe::cross_module_runtime_packaging {

inline void PrintStartupPackageModuleFields(
    const StartupPackageModuleState &state) {
  PrintIntField("startup_registration_copy_status",
                state.registration_copy_status);
  PrintUint64Field("startup_registered_image_count",
                   state.registration.registered_image_count);
  PrintUint64Field("startup_next_expected_registration_order_ordinal",
                   state.registration
                       .next_expected_registration_order_ordinal);
  PrintStringField(
      "startup_last_registered_translation_unit_identity_key",
      NullableCopiedCString(state.last_registered_translation_unit_identity_key));
  PrintIntField("imported_entry_status", state.imported_entry_status);
  PrintIntField("local_entry_status", state.local_entry_status);
  PrintIntField("imported_entry_found", state.imported_entry.found);
  PrintIntField("local_entry_found", state.local_entry.found);
  PrintUint64Field("imported_registration_order_ordinal",
                   state.imported_entry.registration_order_ordinal);
  PrintUint64Field("local_registration_order_ordinal",
                   state.local_entry.registration_order_ordinal);
}

inline void PrintCrossModuleRuntimeDispatchFields(
    const CrossModuleRuntimeDispatchValues &values,
    const char *imported_provider_class_field,
    const char *imported_provider_protocol_field,
    const char *local_consumer_class_field,
    bool trailing_comma = true) {
  PrintIntField(imported_provider_class_field,
                values.imported_provider_class_value);
  PrintIntField(imported_provider_protocol_field,
                values.imported_provider_protocol_value);
  PrintIntField(local_consumer_class_field, values.local_consumer_class_value,
                trailing_comma);
}

inline void PrintStartupRuntimeAssertionFields(
    const CrossModuleRuntimeAssertions &assertions) {
  PrintCrossModuleRuntimeDispatchFields(
      assertions.dispatch_values, "imported_provider_class_value",
      "imported_provider_protocol_value", "local_consumer_class_value");
  PrintIntField("imported_worker_query_status",
                assertions.imported_worker_query_status);
  PrintIntField("imported_worker_conforms",
                assertions.imported_worker_query.conforms);
}

inline void PrintPostResetRuntimeFields(const PostResetRuntimeState &state,
                                        int replay_status) {
  PrintIntField("post_reset_registration_copy_status",
                state.registration_copy_status);
  PrintIntField("post_reset_replay_copy_status", state.replay_copy_status);
  PrintUint64Field("post_reset_registered_image_count",
                   state.registration.registered_image_count);
  PrintUint64Field("post_reset_retained_bootstrap_image_count",
                   state.replay.retained_bootstrap_image_count);
  PrintIntField("replay_status", replay_status);
}

inline void PrintPostReplayPackageModuleFields(
    const PostReplayPackageModuleState &state) {
  PrintIntField("post_replay_registration_copy_status",
                state.registration_copy_status);
  PrintIntField("post_replay_replay_copy_status", state.replay_copy_status);
  PrintUint64Field("post_replay_registered_image_count",
                   state.registration.registered_image_count);
  PrintUint64Field("post_replay_next_expected_registration_order_ordinal",
                   state.registration
                       .next_expected_registration_order_ordinal);
  PrintStringField(
      "post_replay_last_registered_translation_unit_identity_key",
      NullableCopiedCString(state.last_registered_translation_unit_identity_key));
  PrintStringField(
      "post_replay_last_replayed_translation_unit_identity_key",
      NullableCopiedCString(state.last_replayed_translation_unit_identity_key));
  PrintIntField("post_replay_imported_entry_status",
                state.imported_entry_status);
  PrintIntField("post_replay_local_entry_status", state.local_entry_status);
  PrintIntField("post_replay_imported_entry_found",
                state.imported_entry.found);
  PrintIntField("post_replay_local_entry_found", state.local_entry.found);
}

inline void PrintProbeReport(const ProbeResult &result) {
  std::printf("{");
  PrintStartupPackageModuleFields(result.startup);
  PrintStartupRuntimeAssertionFields(result.startup_assertions);
  PrintPostResetRuntimeFields(result.post_reset, result.replay_status);
  PrintPostReplayPackageModuleFields(result.post_replay);
  PrintCrossModuleRuntimeDispatchFields(
      result.post_replay_dispatch_values,
      "post_replay_imported_provider_class_value",
      "post_replay_imported_provider_protocol_value",
      "post_replay_local_consumer_class_value", false);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::cross_module_runtime_packaging

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_REPORT_ERROR_HELPERS_H_
