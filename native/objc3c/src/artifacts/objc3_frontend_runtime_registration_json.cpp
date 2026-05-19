#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"

#include <cstddef>
#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeTranslationUnitRegistrationContractSummaryJson(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"binary_boundary_contract_id\":\""
      << EscapeJsonString(summary.binary_boundary_contract_id)
      << "\",\"archive_static_link_contract_id\":\""
      << EscapeJsonString(summary.archive_static_link_contract_id)
      << "\",\"object_emission_closeout_contract_id\":\""
      << EscapeJsonString(summary.object_emission_closeout_contract_id)
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(summary.runtime_support_library_link_wiring_contract_id)
      << "\",\"registration_surface_path\":\""
      << EscapeJsonString(summary.registration_surface_path)
      << "\",\"registration_payload_model\":\""
      << EscapeJsonString(summary.registration_payload_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(summary)
              ? "true"
              : "false")
      << ",\"boundary_frozen\":"
      << (summary.boundary_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"binary_boundary_ready\":"
      << (summary.binary_boundary_ready ? "true" : "false")
      << ",\"archive_static_link_surface_ready\":"
      << (summary.archive_static_link_surface_ready ? "true" : "false")
      << ",\"object_emission_closeout_surface_ready\":"
      << (summary.object_emission_closeout_surface_ready ? "true" : "false")
      << ",\"runtime_support_library_link_wiring_ready\":"
      << (summary.runtime_support_library_link_wiring_ready ? "true" : "false")
      << ",\"runtime_owned_payload_inventory_published\":"
      << (summary.runtime_owned_payload_inventory_published ? "true" : "false")
      << ",\"constructor_root_reserved_not_emitted\":"
      << (summary.constructor_root_reserved_not_emitted ? "true" : "false")
      << ",\"startup_registration_not_yet_landed\":"
      << (summary.startup_registration_not_yet_landed ? "true" : "false")
      << ",\"runtime_bootstrap_not_yet_landed\":"
      << (summary.runtime_bootstrap_not_yet_landed ? "true" : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"ready_for_registration_manifest_implementation\":"
      << (summary.ready_for_registration_manifest_implementation ? "true"
                                                                : "false")
      << ",\"runtime_owned_payload_artifact_count\":"
      << summary.runtime_owned_payload_artifact_count
      << ",\"runtime_owned_payload_artifacts\":[";
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << "\""
        << EscapeJsonString(summary.runtime_owned_payload_artifacts[index])
        << "\"";
  }
  out << "],\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_root_ownership_model\":\""
      << EscapeJsonString(summary.constructor_root_ownership_model)
      << "\",\"constructor_emission_mode\":\""
      << EscapeJsonString(summary.constructor_emission_mode)
      << "\",\"constructor_priority_policy\":\""
      << EscapeJsonString(summary.constructor_priority_policy)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"binary_boundary_replay_key\":\""
      << EscapeJsonString(summary.binary_boundary_replay_key)
      << "\",\"replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeTranslationUnitRegistrationManifestSummaryJson(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"launch_integration_contract_id\":\""
      << EscapeJsonString(summary.launch_integration_contract_id)
      << "\",\"translation_unit_registration_contract_id\":\""
      << EscapeJsonString(summary.translation_unit_registration_contract_id)
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(
             summary.runtime_support_library_link_wiring_contract_id)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"manifest_payload_model\":\""
      << EscapeJsonString(summary.manifest_payload_model)
      << "\",\"manifest_artifact_relative_path\":\""
      << EscapeJsonString(summary.manifest_artifact_relative_path)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"translation_unit_registration_contract_ready\":"
      << (summary.translation_unit_registration_contract_ready ? "true"
                                                              : "false")
      << ",\"runtime_support_library_link_wiring_ready\":"
      << (summary.runtime_support_library_link_wiring_ready ? "true"
                                                            : "false")
      << ",\"runtime_manifest_template_published\":"
      << (summary.runtime_manifest_template_published ? "true" : "false")
      << ",\"constructor_root_manifest_authoritative\":"
      << (summary.constructor_root_manifest_authoritative ? "true" : "false")
      << ",\"constructor_root_reserved_for_lowering\":"
      << (summary.constructor_root_reserved_for_lowering ? "true" : "false")
      << ",\"init_stub_emission_deferred_to_lowering\":"
      << (summary.init_stub_emission_deferred_to_lowering ? "true" : "false")
      << ",\"runtime_registration_artifact_emitted_by_driver\":"
      << (summary.runtime_registration_artifact_emitted_by_driver ? "true"
                                                                 : "false")
      << ",\"ready_for_lowering_init_stub_emission\":"
      << (summary.ready_for_lowering_init_stub_emission ? "true" : "false")
      << ",\"launch_integration_ready\":"
      << (summary.launch_integration_ready ? "true" : "false")
      << ",\"runtime_owned_payload_artifact_count\":"
      << summary.runtime_owned_payload_artifact_count
      << ",\"runtime_owned_payload_artifacts\":[";
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << "\""
        << EscapeJsonString(summary.runtime_owned_payload_artifacts[index])
        << "\"";
  }
  out << "],\"runtime_support_library_archive_relative_path\":\""
      << EscapeJsonString(summary.runtime_support_library_archive_relative_path)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_root_ownership_model\":\""
      << EscapeJsonString(summary.constructor_root_ownership_model)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"constructor_init_stub_symbol_prefix\":\""
      << EscapeJsonString(summary.constructor_init_stub_symbol_prefix)
      << "\",\"constructor_init_stub_ownership_model\":\""
      << EscapeJsonString(summary.constructor_init_stub_ownership_model)
      << "\",\"constructor_priority_policy\":\""
      << EscapeJsonString(summary.constructor_priority_policy)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"runtime_library_resolution_model\":\""
      << EscapeJsonString(summary.runtime_library_resolution_model)
      << "\",\"driver_linker_flag_consumption_model\":\""
      << EscapeJsonString(summary.driver_linker_flag_consumption_model)
      << "\",\"compile_wrapper_command_surface\":\""
      << EscapeJsonString(summary.compile_wrapper_command_surface)
      << "\",\"compile_proof_command_surface\":\""
      << EscapeJsonString(summary.compile_proof_command_surface)
      << "\",\"execution_smoke_command_surface\":\""
      << EscapeJsonString(summary.execution_smoke_command_surface)
      << "\",\"class_descriptor_count\":"
      << summary.class_descriptor_count
      << ",\"protocol_descriptor_count\":"
      << summary.protocol_descriptor_count
      << ",\"category_descriptor_count\":"
      << summary.category_descriptor_count
      << ",\"property_descriptor_count\":"
      << summary.property_descriptor_count
      << ",\"ivar_descriptor_count\":"
      << summary.ivar_descriptor_count
      << ",\"total_descriptor_count\":"
      << summary.total_descriptor_count
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"translation_unit_registration_replay_key\":\""
      << EscapeJsonString(summary.translation_unit_registration_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
