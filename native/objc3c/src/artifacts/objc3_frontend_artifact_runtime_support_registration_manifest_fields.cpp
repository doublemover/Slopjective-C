#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {

using objc3::io::EscapeJsonString;

void AppendObjc3FrontendArtifactRuntimeSupportRegistrationManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan) {
  const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library =
      runtime_registration_plan.runtime_support_library;
  const Objc3RuntimeSupportLibraryCoreFeatureSummary
      &runtime_support_library_core_feature =
          runtime_registration_plan.runtime_support_library_core_feature;
  const Objc3RuntimeSupportLibraryLinkWiringSummary
      &runtime_support_library_link_wiring =
          runtime_registration_plan.runtime_support_library_link_wiring;
  const Objc3RuntimeTranslationUnitRegistrationContractSummary
      &runtime_translation_unit_registration_contract =
          runtime_registration_plan
              .runtime_translation_unit_registration_contract;
  const Objc3RuntimeTranslationUnitRegistrationManifestSummary
      &runtime_translation_unit_registration_manifest =
          runtime_registration_plan
              .runtime_translation_unit_registration_manifest;

  manifest
      << ",\"runtime_support_library_contract_id\":\""
      << runtime_support_library.contract_id
      << "\",\"runtime_support_library_metadata_publication_contract_id\":\""
      << runtime_support_library.metadata_publication_contract_id
      << "\",\"runtime_support_library_boundary_frozen\":"
      << (runtime_support_library.boundary_frozen ? "true" : "false")
      << ",\"runtime_support_library_fail_closed\":"
      << (runtime_support_library.fail_closed ? "true" : "false")
      << ",\"runtime_support_library_target_name_frozen\":"
      << (runtime_support_library.target_name_frozen ? "true" : "false")
      << ",\"runtime_support_library_exported_entrypoints_frozen\":"
      << (runtime_support_library.exported_entrypoints_frozen ? "true"
                                                              : "false")
      << ",\"runtime_support_library_ownership_boundaries_frozen\":"
      << (runtime_support_library.ownership_boundaries_frozen ? "true"
                                                              : "false")
      << ",\"runtime_support_library_build_constraints_frozen\":"
      << (runtime_support_library.build_constraints_frozen ? "true" : "false")
      << ",\"runtime_support_library_strict_dispatch_errors_required\":"
      << (runtime_support_library.strict_dispatch_errors_required ? "true"
                                                                  : "false")
      << ",\"runtime_support_library_native_library_present\":"
      << (runtime_support_library.native_runtime_library_present ? "true"
                                                                 : "false")
      << ",\"runtime_support_library_driver_link_wiring_pending\":"
      << (runtime_support_library.driver_link_wiring_pending ? "true" : "false")
      << ",\"runtime_support_library_ready_for_skeleton\":"
      << (runtime_support_library.ready_for_runtime_library_skeleton ? "true"
                                                                     : "false")
      << ",\"runtime_support_library_target_name\":\""
      << runtime_support_library.cmake_target_name
      << "\",\"runtime_support_library_public_header_path\":\""
      << runtime_support_library.public_header_path
      << "\",\"runtime_support_library_source_root\":\""
      << runtime_support_library.source_root
      << "\",\"runtime_support_library_library_kind\":\""
      << runtime_support_library.library_kind
      << "\",\"runtime_support_library_archive_basename\":\""
      << runtime_support_library.archive_basename
      << "\",\"runtime_support_library_register_image_symbol\":\""
      << runtime_support_library.register_image_symbol
      << "\",\"runtime_support_library_lookup_selector_symbol\":\""
      << runtime_support_library.lookup_selector_symbol
      << "\",\"runtime_support_library_dispatch_i32_symbol\":\""
      << runtime_support_library.dispatch_i32_symbol
      << "\",\"runtime_support_library_reset_for_testing_symbol\":\""
      << runtime_support_library.reset_for_testing_symbol
      << "\",\"runtime_support_library_driver_link_mode\":\""
      << runtime_support_library.driver_link_mode
      << "\",\"runtime_support_library_compiler_ownership_boundary\":\""
      << runtime_support_library.compiler_ownership_boundary
      << "\",\"runtime_support_library_runtime_ownership_boundary\":\""
      << runtime_support_library.runtime_ownership_boundary
      << "\",\"runtime_support_library_failure_reason\":\""
      << runtime_support_library.failure_reason
      << "\",\"runtime_support_library_core_feature_contract_id\":\""
      << runtime_support_library_core_feature.contract_id
      << "\",\"runtime_support_library_core_feature_support_library_contract_id\":\""
      << runtime_support_library_core_feature.support_library_contract_id
      << "\",\"runtime_support_library_core_feature_metadata_publication_contract_id\":\""
      << runtime_support_library_core_feature.metadata_publication_contract_id
      << "\",\"runtime_support_library_core_feature_fail_closed\":"
      << (runtime_support_library_core_feature.fail_closed ? "true" : "false")
      << ",\"runtime_support_library_core_feature_sources_present\":"
      << (runtime_support_library_core_feature
                  .native_runtime_library_sources_present
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_header_present\":"
      << (runtime_support_library_core_feature.native_runtime_library_header_present
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_archive_build_enabled\":"
      << (runtime_support_library_core_feature
                  .native_runtime_library_archive_build_enabled
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_entrypoints_implemented\":"
      << (runtime_support_library_core_feature
                  .native_runtime_library_entrypoints_implemented
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_selector_lookup_stateful\":"
      << (runtime_support_library_core_feature.selector_lookup_stateful ? "true"
                                                                        : "false")
      << ",\"runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper\":"
      << (runtime_support_library_core_feature
                  .deterministic_dispatch_formula_matches_runtime_test_helper
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_reset_for_testing_supported\":"
      << (runtime_support_library_core_feature.reset_for_testing_supported
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_strict_dispatch_errors_required\":"
      << (runtime_support_library_core_feature.strict_dispatch_errors_required
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_driver_link_wiring_pending\":"
      << (runtime_support_library_core_feature.driver_link_wiring_pending
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_ready_for_driver_link_wiring\":"
      << (runtime_support_library_core_feature.ready_for_driver_link_wiring
              ? "true"
              : "false")
      << ",\"runtime_support_library_core_feature_target_name\":\""
      << runtime_support_library_core_feature.cmake_target_name
      << "\",\"runtime_support_library_core_feature_public_header_path\":\""
      << runtime_support_library_core_feature.public_header_path
      << "\",\"runtime_support_library_core_feature_source_root\":\""
      << runtime_support_library_core_feature.source_root
      << "\",\"runtime_support_library_core_feature_implementation_source_path\":\""
      << runtime_support_library_core_feature.implementation_source_path
      << "\",\"runtime_support_library_core_feature_library_kind\":\""
      << runtime_support_library_core_feature.library_kind
      << "\",\"runtime_support_library_core_feature_archive_basename\":\""
      << runtime_support_library_core_feature.archive_basename
      << "\",\"runtime_support_library_core_feature_archive_relative_path\":\""
      << runtime_support_library_core_feature.archive_relative_path
      << "\",\"runtime_support_library_core_feature_probe_source_path\":\""
      << runtime_support_library_core_feature.probe_source_path
      << "\",\"runtime_support_library_core_feature_register_image_symbol\":\""
      << runtime_support_library_core_feature.register_image_symbol
      << "\",\"runtime_support_library_core_feature_lookup_selector_symbol\":\""
      << runtime_support_library_core_feature.lookup_selector_symbol
      << "\",\"runtime_support_library_core_feature_dispatch_i32_symbol\":\""
      << runtime_support_library_core_feature.dispatch_i32_symbol
      << "\",\"runtime_support_library_core_feature_reset_for_testing_symbol\":\""
      << runtime_support_library_core_feature.reset_for_testing_symbol
      << "\",\"runtime_support_library_core_feature_driver_link_mode\":\""
      << runtime_support_library_core_feature.driver_link_mode
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << runtime_support_library_link_wiring.contract_id
      << "\",\"runtime_support_library_link_wiring_core_feature_contract_id\":\""
      << runtime_support_library_link_wiring
             .support_library_core_feature_contract_id
      << "\",\"runtime_support_library_link_wiring_fail_closed\":"
      << (runtime_support_library_link_wiring.fail_closed ? "true" : "false")
      << ",\"runtime_support_library_link_wiring_archive_available\":"
      << (runtime_support_library_link_wiring.runtime_library_archive_available
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_driver_emits_runtime_link_contract\":"
      << (runtime_support_library_link_wiring.driver_emits_runtime_link_contract
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library\":"
      << (runtime_support_library_link_wiring
                  .execution_smoke_consumes_runtime_library
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_strict_dispatch_errors_required\":"
      << (runtime_support_library_link_wiring.strict_dispatch_errors_required
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_ready_for_runtime_library_consumption\":"
      << (runtime_support_library_link_wiring
                  .ready_for_runtime_library_consumption
              ? "true"
              : "false")
      << ",\"runtime_support_library_link_wiring_archive_relative_path\":\""
      << runtime_support_library_link_wiring.archive_relative_path
      << "\",\"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
      << runtime_support_library_link_wiring.runtime_dispatch_symbol
      << "\",\"runtime_support_library_link_wiring_execution_smoke_script_path\":\""
      << runtime_support_library_link_wiring.execution_smoke_script_path
      << "\",\"runtime_support_library_link_wiring_driver_link_mode\":\""
      << runtime_support_library_link_wiring.driver_link_mode
      << "\",\"runtime_support_library_link_wiring_failure_reason\":\""
      << runtime_support_library_link_wiring.failure_reason
      << "\",\"runtime_support_library_core_feature_failure_reason\":\""
      << runtime_support_library_core_feature.failure_reason
      << "\",\"runtime_translation_unit_registration_contract_id\":\""
      << runtime_translation_unit_registration_contract.contract_id
      << "\",\"runtime_translation_unit_registration_binary_boundary_contract_id\":\""
      << runtime_translation_unit_registration_contract
             .binary_boundary_contract_id
      << "\",\"runtime_translation_unit_registration_archive_static_link_contract_id\":\""
      << runtime_translation_unit_registration_contract
             .archive_static_link_contract_id
      << "\",\"runtime_translation_unit_registration_object_emission_closeout_contract_id\":\""
      << runtime_translation_unit_registration_contract
             .object_emission_closeout_contract_id
      << "\",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_contract_id\":\""
      << runtime_translation_unit_registration_contract
             .runtime_support_library_link_wiring_contract_id
      << "\",\"runtime_translation_unit_registration_payload_model\":\""
      << runtime_translation_unit_registration_contract.registration_payload_model
      << "\",\"runtime_translation_unit_registration_runtime_owned_payload_artifact_count\":"
      << runtime_translation_unit_registration_contract
             .runtime_owned_payload_artifact_count
      << ",\"runtime_translation_unit_registration_payload_artifact_relative_path\":\""
      << runtime_translation_unit_registration_contract
             .runtime_owned_payload_artifacts[0]
      << "\",\"runtime_translation_unit_registration_linker_response_artifact_relative_path\":\""
      << runtime_translation_unit_registration_contract
             .runtime_owned_payload_artifacts[1]
      << "\",\"runtime_translation_unit_registration_discovery_artifact_relative_path\":\""
      << runtime_translation_unit_registration_contract
             .runtime_owned_payload_artifacts[2]
      << "\",\"runtime_translation_unit_registration_constructor_root_symbol\":\""
      << runtime_translation_unit_registration_contract.constructor_root_symbol
      << "\",\"runtime_translation_unit_registration_constructor_root_ownership_model\":\""
      << runtime_translation_unit_registration_contract
             .constructor_root_ownership_model
      << "\",\"runtime_translation_unit_registration_constructor_emission_mode\":\""
      << runtime_translation_unit_registration_contract.constructor_emission_mode
      << "\",\"runtime_translation_unit_registration_constructor_priority_policy\":\""
      << runtime_translation_unit_registration_contract.constructor_priority_policy
      << "\",\"runtime_translation_unit_registration_entrypoint_symbol\":\""
      << runtime_translation_unit_registration_contract
             .registration_entrypoint_symbol
      << "\",\"runtime_translation_unit_registration_translation_unit_identity_model\":\""
      << runtime_translation_unit_registration_contract
             .translation_unit_identity_model
      << "\",\"runtime_translation_unit_registration_boundary_frozen\":"
      << (runtime_translation_unit_registration_contract.boundary_frozen ? "true"
                                                                        : "false")
      << ",\"runtime_translation_unit_registration_fail_closed\":"
      << (runtime_translation_unit_registration_contract.fail_closed ? "true"
                                                                    : "false")
      << ",\"runtime_translation_unit_registration_binary_boundary_ready\":"
      << (runtime_translation_unit_registration_contract.binary_boundary_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_archive_static_link_surface_ready\":"
      << (runtime_translation_unit_registration_contract
                  .archive_static_link_surface_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_object_emission_closeout_surface_ready\":"
      << (runtime_translation_unit_registration_contract
                  .object_emission_closeout_surface_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_ready\":"
      << (runtime_translation_unit_registration_contract
                  .runtime_support_library_link_wiring_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_runtime_owned_payload_inventory_published\":"
      << (runtime_translation_unit_registration_contract
                  .runtime_owned_payload_inventory_published
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_constructor_root_reserved_not_emitted\":"
      << (runtime_translation_unit_registration_contract
                  .constructor_root_reserved_not_emitted
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_startup_registration_not_yet_landed\":"
      << (runtime_translation_unit_registration_contract
                  .startup_registration_not_yet_landed
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_runtime_bootstrap_not_yet_landed\":"
      << (runtime_translation_unit_registration_contract
                  .runtime_bootstrap_not_yet_landed
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_explicit_non_goals_published\":"
      << (runtime_translation_unit_registration_contract
                  .explicit_non_goals_published
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_ready_for_manifest_implementation\":"
      << (runtime_translation_unit_registration_contract
                  .ready_for_registration_manifest_implementation
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_binary_boundary_replay_key\":\""
      << EscapeJsonString(runtime_translation_unit_registration_contract
                              .binary_boundary_replay_key)
      << "\",\"runtime_translation_unit_registration_replay_key\":\""
      << EscapeJsonString(runtime_translation_unit_registration_contract.replay_key)
      << "\",\"runtime_translation_unit_registration_failure_reason\":\""
      << EscapeJsonString(
             runtime_translation_unit_registration_contract.failure_reason)
      << "\""
      << ",\"runtime_translation_unit_registration_manifest_contract_id\":\""
      << runtime_translation_unit_registration_manifest.contract_id
      << "\",\"runtime_translation_unit_registration_manifest_payload_model\":\""
      << runtime_translation_unit_registration_manifest.manifest_payload_model
      << "\",\"runtime_translation_unit_registration_manifest_artifact_relative_path\":\""
      << runtime_translation_unit_registration_manifest
             .manifest_artifact_relative_path
      << "\",\"runtime_translation_unit_registration_manifest_runtime_owned_payload_artifact_count\":"
      << runtime_translation_unit_registration_manifest
             .runtime_owned_payload_artifact_count
      << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_archive_relative_path\":\""
      << runtime_translation_unit_registration_manifest
             .runtime_support_library_archive_relative_path
      << "\",\"runtime_translation_unit_registration_manifest_constructor_root_symbol\":\""
      << runtime_translation_unit_registration_manifest.constructor_root_symbol
      << "\",\"runtime_translation_unit_registration_manifest_constructor_root_ownership_model\":\""
      << runtime_translation_unit_registration_manifest
             .constructor_root_ownership_model
      << "\",\"runtime_translation_unit_registration_manifest_authority_model\":\""
      << runtime_translation_unit_registration_manifest.manifest_authority_model
      << "\",\"runtime_translation_unit_registration_manifest_init_stub_symbol_prefix\":\""
      << runtime_translation_unit_registration_manifest
             .constructor_init_stub_symbol_prefix
      << "\",\"runtime_translation_unit_registration_manifest_init_stub_ownership_model\":\""
      << runtime_translation_unit_registration_manifest
             .constructor_init_stub_ownership_model
      << "\",\"runtime_translation_unit_registration_manifest_constructor_priority_policy\":\""
      << runtime_translation_unit_registration_manifest.constructor_priority_policy
      << "\",\"runtime_translation_unit_registration_manifest_registration_entrypoint_symbol\":\""
      << runtime_translation_unit_registration_manifest.registration_entrypoint_symbol
      << "\",\"runtime_translation_unit_registration_manifest_translation_unit_identity_model\":\""
      << runtime_translation_unit_registration_manifest
             .translation_unit_identity_model
      << "\",\"runtime_translation_unit_registration_manifest_launch_integration_contract_id\":\""
      << runtime_translation_unit_registration_manifest
             .launch_integration_contract_id
      << "\",\"runtime_translation_unit_registration_manifest_runtime_library_resolution_model\":\""
      << runtime_translation_unit_registration_manifest
             .runtime_library_resolution_model
      << "\",\"runtime_translation_unit_registration_manifest_driver_linker_flag_consumption_model\":\""
      << runtime_translation_unit_registration_manifest
             .driver_linker_flag_consumption_model
      << "\",\"runtime_translation_unit_registration_manifest_compile_wrapper_command_surface\":\""
      << runtime_translation_unit_registration_manifest
             .compile_wrapper_command_surface
      << "\",\"runtime_translation_unit_registration_manifest_compile_proof_command_surface\":\""
      << runtime_translation_unit_registration_manifest
             .compile_proof_command_surface
      << "\",\"runtime_translation_unit_registration_manifest_execution_smoke_command_surface\":\""
      << runtime_translation_unit_registration_manifest
             .execution_smoke_command_surface
      << "\",\"runtime_translation_unit_registration_manifest_class_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.class_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_protocol_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.protocol_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_category_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.category_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_property_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.property_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_ivar_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.ivar_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_total_descriptor_count\":"
      << runtime_translation_unit_registration_manifest.total_descriptor_count
      << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_order_ordinal\":"
      << runtime_translation_unit_registration_manifest
             .translation_unit_registration_order_ordinal
      << ",\"runtime_translation_unit_registration_manifest_fail_closed\":"
      << (runtime_translation_unit_registration_manifest.fail_closed ? "true"
                                                                    : "false")
      << ",\"runtime_translation_unit_registration_manifest_contract_ready\":"
      << (runtime_translation_unit_registration_manifest
                  .translation_unit_registration_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_link_wiring_ready\":"
      << (runtime_translation_unit_registration_manifest
                  .runtime_support_library_link_wiring_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_template_published\":"
      << (runtime_translation_unit_registration_manifest
                  .runtime_manifest_template_published
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_constructor_root_manifest_authoritative\":"
      << (runtime_translation_unit_registration_manifest
                  .constructor_root_manifest_authoritative
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_constructor_root_reserved_for_lowering\":"
      << (runtime_translation_unit_registration_manifest
                  .constructor_root_reserved_for_lowering
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_init_stub_emission_deferred_to_lowering\":"
      << (runtime_translation_unit_registration_manifest
                  .init_stub_emission_deferred_to_lowering
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_artifact_emitted_by_driver\":"
      << (runtime_translation_unit_registration_manifest
                  .runtime_registration_artifact_emitted_by_driver
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_ready_for_lowering_init_stub_emission\":"
      << (runtime_translation_unit_registration_manifest
                  .ready_for_lowering_init_stub_emission
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_launch_integration_ready\":"
      << (runtime_translation_unit_registration_manifest.launch_integration_ready
              ? "true"
              : "false")
      << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_replay_key\":\""
      << EscapeJsonString(runtime_translation_unit_registration_manifest
                              .translation_unit_registration_replay_key)
      << "\",\"runtime_translation_unit_registration_manifest_replay_key\":\""
      << EscapeJsonString(runtime_translation_unit_registration_manifest.replay_key)
      << "\",\"runtime_translation_unit_registration_manifest_failure_reason\":\""
      << EscapeJsonString(runtime_translation_unit_registration_manifest.failure_reason)
      << "\"";
}

}  // namespace objc3::artifacts::frontend
