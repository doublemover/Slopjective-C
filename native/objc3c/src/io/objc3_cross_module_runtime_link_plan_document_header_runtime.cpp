#include "io/objc3_cross_module_runtime_link_plan_document_header_runtime.h"

#include <ostream>
#include <string>

#include "io/objc3_process_internal.h"

namespace {

void EmitStringField(std::ostream &out,
                     const char *name,
                     const std::string &value) {
  out << "  \"" << name << "\": \"" << EscapeJsonString(value) << "\",\n";
}

void EmitLiteralStringField(std::ostream &out,
                            const char *name,
                            const char *value) {
  out << "  \"" << name << "\": \"" << value << "\",\n";
}

}  // namespace

void EmitObjc3CrossModuleRuntimeLinkPlanHeaderRuntimeSurfaces(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs) {
  EmitStringField(
      out,
      "bootstrap_live_registration_contract_id",
      inputs.expected_bootstrap_live_registration_contract_id);
  EmitStringField(
      out,
      "bootstrap_live_restart_hardening_contract_id",
      inputs.expected_bootstrap_live_restart_hardening_contract_id);
  EmitStringField(
      out,
      "bootstrap_replay_registered_images_symbol",
      inputs.expected_bootstrap_replay_registered_images_symbol);
  EmitStringField(
      out,
      "bootstrap_reset_replay_state_snapshot_symbol",
      inputs.expected_bootstrap_reset_replay_state_snapshot_symbol);
  EmitStringField(out,
                  "bootstrap_reset_for_testing_symbol",
                  inputs.expected_bootstrap_reset_for_testing_symbol);
  EmitLiteralStringField(
      out,
      "runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id",
      "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1");
  EmitLiteralStringField(
      out,
      "runtime_object_model_realization_source_surface_contract_id",
      "objc3c.runtime.object.model.realization.source.surface.v1");
  EmitLiteralStringField(
      out,
      "runtime_realization_lowering_reflection_artifact_surface_contract_id",
      "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1");
  EmitLiteralStringField(
      out,
      "runtime_dispatch_table_reflection_record_lowering_surface_contract_id",
      "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1");
  EmitStringField(
      out,
      "runtime_cross_module_block_ownership_artifact_preservation_surface_contract_id",
      inputs.expected_block_ownership_contract_id);
  EmitStringField(out,
                  "runtime_block_arc_lowering_helper_surface_contract_id",
                  inputs.expected_block_ownership_source_contract_id);
  EmitStringField(
      out,
      "block_object_invoke_thunk_lowering_contract_id",
      inputs.expected_block_ownership_object_invoke_thunk_lowering_contract_id);
  EmitStringField(out,
                  "block_byref_helper_lowering_contract_id",
                  inputs.expected_block_ownership_byref_helper_lowering_contract_id);
  EmitStringField(
      out,
      "block_escape_runtime_hook_lowering_contract_id",
      inputs.expected_block_ownership_escape_runtime_hook_lowering_contract_id);
  EmitStringField(
      out,
      "block_runtime_support_library_link_wiring_contract_id",
      inputs
          .expected_block_ownership_runtime_support_library_link_wiring_contract_id);
  EmitStringField(
      out,
      "runtime_cross_module_storage_reflection_artifact_preservation_surface_contract_id",
      inputs.expected_storage_reflection_contract_id);
  EmitStringField(
      out,
      "runtime_property_ivar_storage_accessor_source_surface_contract_id",
      inputs.expected_storage_reflection_source_contract_id);
  EmitStringField(
      out,
      "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
      inputs
          .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id);
  EmitStringField(
      out,
      "executable_property_accessor_layout_lowering_contract_id",
      inputs
          .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id);
  EmitStringField(
      out,
      "executable_ivar_layout_emission_contract_id",
      inputs.expected_storage_reflection_executable_ivar_layout_emission_contract_id);
  EmitStringField(
      out,
      "executable_synthesized_accessor_property_lowering_contract_id",
      inputs
          .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id);
  EmitLiteralStringField(
      out,
      "realized_metadata_replay_preservation_model",
      "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests");
  EmitLiteralStringField(
      out,
      "block_ownership_artifact_preservation_model",
      "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission");
  EmitLiteralStringField(
      out,
      "storage_reflection_artifact_preservation_model",
      "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission");
  out << "  \"imported_live_registration_replay_ready\": true,\n"
      << "  \"imported_live_restart_hardening_ready\": true,\n"
      << "  \"block_ownership_cross_module_preservation_ready\": true,\n"
      << "  \"storage_reflection_cross_module_preservation_ready\": true,\n";
}
