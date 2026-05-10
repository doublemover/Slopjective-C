#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeBootstrapApiReplayKey(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";support_library_core_feature_contract_id="
      << summary.support_library_core_feature_contract_id
      << ";support_library_link_wiring_contract_id="
      << summary.support_library_link_wiring_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";public_header_path=" << summary.public_header_path
      << ";archive_relative_path=" << summary.archive_relative_path
      << ";registration_status_enum_type="
      << summary.registration_status_enum_type
      << ";image_descriptor_type=" << summary.image_descriptor_type
      << ";selector_handle_type=" << summary.selector_handle_type
      << ";registration_snapshot_type="
      << summary.registration_snapshot_type
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";selector_lookup_symbol=" << summary.selector_lookup_symbol
      << ";dispatch_entrypoint_symbol=" << summary.dispatch_entrypoint_symbol
      << ";state_snapshot_symbol=" << summary.state_snapshot_symbol
      << ";reset_for_testing_symbol=" << summary.reset_for_testing_symbol
      << ";registration_result_model=" << summary.registration_result_model
      << ";registration_order_ordinal_model="
      << summary.registration_order_ordinal_model
      << ";runtime_state_locking_model="
      << summary.runtime_state_locking_model
      << ";startup_invocation_model=" << summary.startup_invocation_model
      << ";image_walk_lifecycle_model="
      << summary.image_walk_lifecycle_model
      << ";deterministic_reset_lifecycle_model="
      << summary.deterministic_reset_lifecycle_model
      << ";support_library_core_feature_replay_key="
      << summary.support_library_core_feature_replay_key
      << ";support_library_link_wiring_replay_key="
      << summary.support_library_link_wiring_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapApiSummary BuildRuntimeBootstrapApiSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeBootstrapApiSummary summary;
  summary.fail_closed = true;
  summary.support_library_core_feature_contract_ready =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library);
  summary.support_library_link_wiring_contract_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.api_surface_frozen = true;
  summary.registration_entrypoint_frozen = true;
  summary.selector_lookup_and_dispatch_frozen = true;
  summary.reset_and_snapshot_hooks_frozen = true;
  summary.runtime_probe_required = true;
  summary.image_walk_not_yet_landed = true;
  summary.deterministic_reset_expansion_not_yet_landed = true;
  summary.ready_for_registrar_implementation =
      summary.support_library_core_feature_contract_ready &&
      summary.support_library_link_wiring_contract_ready;
  if (summary.support_library_core_feature_contract_ready) {
    summary.support_library_core_feature_replay_key =
        runtime_support_library.contract_id + ";archive_relative_path=" +
        runtime_support_library.archive_relative_path + ";public_header_path=" +
        runtime_support_library.public_header_path + ";register_image_symbol=" +
        runtime_support_library.register_image_symbol +
        ";lookup_selector_symbol=" +
        runtime_support_library.lookup_selector_symbol +
        ";dispatch_i32_symbol=" + runtime_support_library.dispatch_i32_symbol +
        ";reset_for_testing_symbol=" +
        runtime_support_library.reset_for_testing_symbol;
  }
  if (summary.support_library_link_wiring_contract_ready) {
    summary.support_library_link_wiring_replay_key =
        runtime_support_library_link_wiring.contract_id +
        ";archive_relative_path=" +
        runtime_support_library_link_wiring.archive_relative_path +
        ";runtime_dispatch_symbol=" +
        runtime_support_library_link_wiring.runtime_dispatch_symbol +
        ";driver_link_mode=" +
        runtime_support_library_link_wiring.driver_link_mode;
  }
  if (summary.ready_for_registrar_implementation) {
    summary.replay_key = BuildRuntimeBootstrapApiReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapApiSummary(summary)) {
    summary.failure_reason = "runtime bootstrap api summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapApiSummaryJson(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"support_library_core_feature_contract_id\":\""
      << EscapeJsonString(summary.support_library_core_feature_contract_id)
      << "\",\"support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(summary.support_library_link_wiring_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"public_header_path\":\""
      << EscapeJsonString(summary.public_header_path)
      << "\",\"archive_relative_path\":\""
      << EscapeJsonString(summary.archive_relative_path)
      << "\",\"registration_status_enum_type\":\""
      << EscapeJsonString(summary.registration_status_enum_type)
      << "\",\"image_descriptor_type\":\""
      << EscapeJsonString(summary.image_descriptor_type)
      << "\",\"selector_handle_type\":\""
      << EscapeJsonString(summary.selector_handle_type)
      << "\",\"registration_snapshot_type\":\""
      << EscapeJsonString(summary.registration_snapshot_type)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"selector_lookup_symbol\":\""
      << EscapeJsonString(summary.selector_lookup_symbol)
      << "\",\"dispatch_entrypoint_symbol\":\""
      << EscapeJsonString(summary.dispatch_entrypoint_symbol)
      << "\",\"state_snapshot_symbol\":\""
      << EscapeJsonString(summary.state_snapshot_symbol)
      << "\",\"reset_for_testing_symbol\":\""
      << EscapeJsonString(summary.reset_for_testing_symbol)
      << "\",\"registration_result_model\":\""
      << EscapeJsonString(summary.registration_result_model)
      << "\",\"registration_order_ordinal_model\":\""
      << EscapeJsonString(summary.registration_order_ordinal_model)
      << "\",\"runtime_state_locking_model\":\""
      << EscapeJsonString(summary.runtime_state_locking_model)
      << "\",\"startup_invocation_model\":\""
      << EscapeJsonString(summary.startup_invocation_model)
      << "\",\"image_walk_lifecycle_model\":\""
      << EscapeJsonString(summary.image_walk_lifecycle_model)
      << "\",\"deterministic_reset_lifecycle_model\":\""
      << EscapeJsonString(summary.deterministic_reset_lifecycle_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapApiSummary(summary) ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"support_library_core_feature_contract_ready\":"
      << (summary.support_library_core_feature_contract_ready ? "true"
                                                              : "false")
      << ",\"support_library_link_wiring_contract_ready\":"
      << (summary.support_library_link_wiring_contract_ready ? "true"
                                                             : "false")
      << ",\"api_surface_frozen\":"
      << (summary.api_surface_frozen ? "true" : "false")
      << ",\"registration_entrypoint_frozen\":"
      << (summary.registration_entrypoint_frozen ? "true" : "false")
      << ",\"selector_lookup_and_dispatch_frozen\":"
      << (summary.selector_lookup_and_dispatch_frozen ? "true" : "false")
      << ",\"reset_and_snapshot_hooks_frozen\":"
      << (summary.reset_and_snapshot_hooks_frozen ? "true" : "false")
      << ",\"runtime_probe_required\":"
      << (summary.runtime_probe_required ? "true" : "false")
      << ",\"image_walk_not_yet_landed\":"
      << (summary.image_walk_not_yet_landed ? "true" : "false")
      << ",\"deterministic_reset_expansion_not_yet_landed\":"
      << (summary.deterministic_reset_expansion_not_yet_landed ? "true"
                                                               : "false")
      << ",\"ready_for_registrar_implementation\":"
      << (summary.ready_for_registrar_implementation ? "true" : "false")
      << ",\"support_library_core_feature_replay_key\":\""
      << EscapeJsonString(summary.support_library_core_feature_replay_key)
      << "\",\"support_library_link_wiring_replay_key\":\""
      << EscapeJsonString(summary.support_library_link_wiring_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
