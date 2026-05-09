#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"
#include "lower/contracts/runtime_bootstrap_lowering_contracts.h"
#include "lower/contracts/runtime_metadata_emission_contracts.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

Objc3RuntimeSupportLibraryContractSummary
BuildRuntimeSupportLibraryContractSummary() {
  Objc3RuntimeSupportLibraryContractSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.target_name_frozen = true;
  summary.exported_entrypoints_frozen = true;
  summary.ownership_boundaries_frozen = true;
  summary.build_constraints_frozen = true;
  summary.strict_dispatch_errors_required = true;
  summary.native_runtime_library_present = false;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_runtime_library_skeleton = true;
  return summary;
}

Objc3RuntimeSupportLibraryCoreFeatureSummary
BuildRuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library) {
  Objc3RuntimeSupportLibraryCoreFeatureSummary summary;
  summary.support_library_contract_id = runtime_support_library.contract_id;
  summary.metadata_publication_contract_id =
      runtime_support_library.metadata_publication_contract_id;
  summary.fail_closed = true;
  summary.native_runtime_library_sources_present = true;
  summary.native_runtime_library_header_present = true;
  summary.native_runtime_library_archive_build_enabled = true;
  summary.native_runtime_library_entrypoints_implemented = true;
  summary.selector_lookup_stateful = true;
  summary.deterministic_dispatch_formula_matches_runtime_test_helper = true;
  summary.reset_for_testing_supported = true;
  summary.strict_dispatch_errors_required = true;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_driver_link_wiring =
      IsReadyObjc3RuntimeSupportLibraryContractSummary(runtime_support_library);
  return summary;
}

Objc3RuntimeSupportLibraryLinkWiringSummary
BuildRuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature) {
  Objc3RuntimeSupportLibraryLinkWiringSummary summary;
  summary.support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  summary.fail_closed = true;
  summary.runtime_library_archive_available =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  summary.driver_emits_runtime_link_contract = true;
  summary.execution_smoke_consumes_runtime_library = true;
  summary.strict_dispatch_errors_required = true;
  summary.ready_for_runtime_library_consumption =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library_core_feature);
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationContractReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";binary_boundary_contract_id=" << summary.binary_boundary_contract_id
      << ";archive_static_link_contract_id="
      << summary.archive_static_link_contract_id
      << ";object_emission_closeout_contract_id="
      << summary.object_emission_closeout_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";registration_surface_path=" << summary.registration_surface_path
      << ";registration_payload_model="
      << summary.registration_payload_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";constructor_emission_mode=" << summary.constructor_emission_mode
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";binary_boundary_replay_key=" << summary.binary_boundary_replay_key;
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationContractSummary
BuildRuntimeTranslationUnitRegistrationContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        &runtime_ingest_binary_boundary,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeTranslationUnitRegistrationContractSummary summary;
  summary.fail_closed = true;
  summary.boundary_frozen = true;
  summary.binary_boundary_ready =
      IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          runtime_ingest_binary_boundary);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  const std::string archive_static_link_summary =
      Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary();
  summary.archive_static_link_surface_ready =
      archive_static_link_summary.find(
          std::string("contract=") +
          kObjc3RuntimeArchiveStaticLinkDiscoveryContractId) !=
          std::string::npos &&
      archive_static_link_summary.find(
          std::string("translation_unit_identity_model=") +
          summary.translation_unit_identity_model) != std::string::npos;
  const std::string object_emission_closeout_summary =
      Objc3RuntimeMetadataObjectEmissionCloseoutSummary();
  summary.object_emission_closeout_surface_ready =
      object_emission_closeout_summary.find(
          std::string("contract=") +
          kObjc3RuntimeMetadataObjectEmissionCloseoutContractId) !=
          std::string::npos &&
      object_emission_closeout_summary.find(
          "non_goals=no-startup-registration-or-runtime-bootstrap") !=
          std::string::npos;
  summary.runtime_owned_payload_inventory_published = true;
  summary.constructor_root_reserved_not_emitted = true;
  summary.startup_registration_not_yet_landed = true;
  summary.runtime_bootstrap_not_yet_landed = true;
  summary.explicit_non_goals_published = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  if (summary.binary_boundary_ready) {
    summary.binary_boundary_replay_key = runtime_ingest_binary_boundary.replay_key;
  }
  summary.ready_for_registration_manifest_implementation =
      summary.binary_boundary_ready &&
      summary.archive_static_link_surface_ready &&
      summary.object_emission_closeout_surface_ready &&
      summary.runtime_support_library_link_wiring_ready;
  if (summary.ready_for_registration_manifest_implementation) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationContractReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration surface contract is incomplete";
  }
  return summary;
}

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

std::string BuildRuntimeTranslationUnitRegistrationManifestReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";translation_unit_registration_contract_id="
      << summary.translation_unit_registration_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";manifest_surface_path=" << summary.manifest_surface_path
      << ";manifest_payload_model=" << summary.manifest_payload_model
      << ";manifest_artifact_relative_path="
      << summary.manifest_artifact_relative_path
      << ";runtime_support_library_archive_relative_path="
      << summary.runtime_support_library_archive_relative_path
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";constructor_init_stub_ownership_model="
      << summary.constructor_init_stub_ownership_model
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";class_descriptor_count=" << summary.class_descriptor_count
      << ";protocol_descriptor_count=" << summary.protocol_descriptor_count
      << ";category_descriptor_count=" << summary.category_descriptor_count
      << ";property_descriptor_count=" << summary.property_descriptor_count
      << ";ivar_descriptor_count=" << summary.ivar_descriptor_count
      << ";total_descriptor_count=" << summary.total_descriptor_count
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";translation_unit_registration_replay_key="
      << summary.translation_unit_registration_replay_key;
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationManifestSummary
BuildRuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary
        &registration_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    std::uint64_t translation_unit_registration_order_ordinal) {
  Objc3RuntimeTranslationUnitRegistrationManifestSummary summary;
  summary.fail_closed = true;
  summary.translation_unit_registration_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(
          registration_contract);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.runtime_manifest_template_published = true;
  summary.constructor_root_manifest_authoritative = true;
  summary.constructor_root_reserved_for_lowering = true;
  summary.init_stub_emission_deferred_to_lowering = true;
  summary.runtime_registration_artifact_emitted_by_driver = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  summary.class_descriptor_count =
      runtime_metadata_section_publication.class_descriptor_count;
  summary.protocol_descriptor_count =
      runtime_metadata_section_publication.protocol_descriptor_count;
  summary.category_descriptor_count =
      runtime_metadata_section_publication.category_descriptor_count;
  summary.property_descriptor_count =
      runtime_metadata_section_publication.property_descriptor_count;
  summary.ivar_descriptor_count =
      runtime_metadata_section_publication.ivar_descriptor_count;
  summary.total_descriptor_count =
      runtime_metadata_section_publication.total_descriptor_count;
  if (translation_unit_registration_order_ordinal > 0) {
    summary.translation_unit_registration_order_ordinal =
        translation_unit_registration_order_ordinal;
  }
  if (summary.translation_unit_registration_contract_ready) {
    summary.translation_unit_registration_replay_key =
        registration_contract.replay_key;
  }
  summary.ready_for_lowering_init_stub_emission =
      summary.translation_unit_registration_contract_ready &&
      summary.runtime_support_library_link_wiring_ready;
  summary.launch_integration_ready =
      summary.ready_for_lowering_init_stub_emission;
  if (summary.ready_for_lowering_init_stub_emission) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationManifestReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration manifest summary is incomplete";
  }
  return summary;
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
