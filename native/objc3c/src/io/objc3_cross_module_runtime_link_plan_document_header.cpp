#include "io/objc3_cross_module_runtime_link_plan_document_header.h"

#include <ostream>
#include <string>

#include "io/objc3_process_internal.h"
#include "io/objc3_process_json_helpers.h"

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

void EmitReadyField(std::ostream &out, const char *name, bool ready) {
  out << "  \"" << name << "\": " << (ready ? "true" : "false") << ",\n";
}

}  // namespace

void EmitObjc3CrossModuleRuntimeLinkPlanDocumentHeader(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const Objc3CrossModuleRuntimeLinkPlanSections &sections) {
  out << "{\n";
  EmitStringField(out, "contract_id", inputs.contract_id);
  EmitStringField(out,
                  "source_orchestration_contract_id",
                  inputs.source_orchestration_contract_id);
  EmitStringField(out, "import_surface_contract_id",
                  inputs.import_surface_contract_id);
  EmitStringField(out,
                  "registration_manifest_contract_id",
                  inputs.registration_manifest_contract_id);
  EmitStringField(out, "payload_model", inputs.payload_model);
  EmitStringField(out, "artifact", inputs.artifact_relative_path);
  EmitStringField(out,
                  "linker_response_artifact",
                  inputs.linker_response_artifact_relative_path);
  EmitStringField(out, "authority_model", inputs.authority_model);
  EmitStringField(out, "packaging_model", inputs.packaging_model);
  EmitStringField(out,
                  "registration_scope_model",
                  inputs.registration_scope_model);
  EmitStringField(out, "link_object_order_model",
                  inputs.link_object_order_model);
  EmitStringField(out,
                  "expected_error_handling_contract_id",
                  inputs.expected_error_handling_contract_id);
  EmitStringField(out,
                  "expected_error_handling_source_contract_id",
                  inputs.expected_error_handling_source_contract_id);
  EmitStringField(out,
                  "expected_concurrency_actor_contract_id",
                  inputs.expected_concurrency_actor_contract_id);
  EmitStringField(out,
                  "expected_concurrency_actor_source_contract_id",
                  inputs.expected_concurrency_actor_source_contract_id);
  EmitStringField(out,
                  "expected_interop_ffi_contract_id",
                  inputs.expected_interop_ffi_contract_id);
  EmitStringField(out,
                  "expected_interop_ffi_source_contract_id",
                  inputs.expected_interop_ffi_source_contract_id);
  EmitStringField(out,
                  "expected_interop_ffi_preservation_contract_id",
                  inputs.expected_interop_ffi_preservation_contract_id);
  EmitStringField(out,
                  "expected_interop_header_module_bridge_contract_id",
                  inputs.expected_interop_header_module_bridge_contract_id);
  EmitStringField(
      out,
      "expected_interop_header_module_bridge_source_contract_id",
      inputs.expected_interop_header_module_bridge_source_contract_id);
  EmitStringField(
      out,
      "expected_interop_header_module_bridge_preservation_contract_id",
      inputs.expected_interop_header_module_bridge_preservation_contract_id);
  EmitStringField(
      out,
      "expected_interop_bridge_header_artifact_relative_path",
      inputs.expected_interop_bridge_header_artifact_relative_path);
  EmitStringField(
      out,
      "expected_interop_bridge_module_artifact_relative_path",
      inputs.expected_interop_bridge_module_artifact_relative_path);
  EmitStringField(out,
                  "expected_interop_bridge_artifact_relative_path",
                  inputs.expected_interop_bridge_artifact_relative_path);
  EmitStringField(out,
                  "expected_metaprogramming_host_cache_contract_id",
                  inputs.expected_metaprogramming_host_cache_contract_id);
  EmitStringField(
      out,
      "expected_metaprogramming_host_cache_source_contract_id",
      inputs.expected_metaprogramming_host_cache_source_contract_id);
  EmitStringField(
      out,
      "expected_metaprogramming_host_cache_executable_relative_path",
      inputs.expected_metaprogramming_host_cache_executable_relative_path);
  EmitStringField(
      out,
      "expected_metaprogramming_host_cache_root_relative_path",
      inputs.expected_metaprogramming_host_cache_root_relative_path);
  EmitStringField(out,
                  "expected_block_ownership_contract_id",
                  inputs.expected_block_ownership_contract_id);
  EmitStringField(out,
                  "expected_block_ownership_source_contract_id",
                  inputs.expected_block_ownership_source_contract_id);
  EmitStringField(
      out,
      "expected_block_ownership_object_invoke_thunk_lowering_contract_id",
      inputs.expected_block_ownership_object_invoke_thunk_lowering_contract_id);
  EmitStringField(
      out,
      "expected_block_ownership_byref_helper_lowering_contract_id",
      inputs.expected_block_ownership_byref_helper_lowering_contract_id);
  EmitStringField(
      out,
      "expected_block_ownership_escape_runtime_hook_lowering_contract_id",
      inputs.expected_block_ownership_escape_runtime_hook_lowering_contract_id);
  EmitStringField(
      out,
      "expected_block_ownership_runtime_support_library_link_wiring_contract_id",
      inputs
          .expected_block_ownership_runtime_support_library_link_wiring_contract_id);

  out << "  \"module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(sections.module_names_lexicographic,
                                      "    ")
      << ",\n"
      << "  \"module_image_count\": "
      << sections.module_names_lexicographic.size() << ",\n"
      << "  \"direct_import_input_count\": "
      << sections.direct_import_surface_artifact_paths.size() << ",\n"
      << "  \"error_handling_imported_module_count\": "
      << sections.imported_error_handling_module_names_lexicographic.size()
      << ",\n"
      << "  \"concurrency_actor_imported_module_count\": "
      << sections.imported_concurrency_actor_module_names_lexicographic.size()
      << ",\n"
      << "  \"interop_ffi_imported_module_count\": "
      << sections.imported_interop_ffi_module_names_lexicographic.size()
      << ",\n"
      << "  \"interop_header_module_bridge_imported_module_count\": "
      << sections
             .imported_interop_header_module_bridge_module_names_lexicographic
             .size()
      << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_count\": "
      << sections.imported_metaprogramming_host_cache_module_names_lexicographic
             .size()
      << ",\n"
      << "  \"direct_import_surface_artifact_paths\": "
      << BuildIndentedStringArrayJson(
             sections.direct_import_surface_artifact_paths, "    ")
      << ",\n"
      << "  \"error_handling_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_error_handling_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"concurrency_actor_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_concurrency_actor_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"interop_ffi_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_interop_ffi_module_names_lexicographic, "    ")
      << ",\n"
      << "  \"interop_header_module_bridge_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections
                 .imported_interop_header_module_bridge_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections
                 .imported_metaprogramming_host_cache_module_names_lexicographic,
             "    ")
      << ",\n";

  EmitReadyField(out,
                 "error_handling_cross_module_preservation_ready",
                 !sections.imported_error_handling_module_names_lexicographic
                      .empty());
  EmitReadyField(
      out,
      "concurrency_actor_cross_module_isolation_ready",
      !sections.imported_concurrency_actor_module_names_lexicographic.empty());
  EmitReadyField(out,
                 "interop_ffi_cross_module_packaging_ready",
                 !sections.imported_interop_ffi_module_names_lexicographic
                      .empty());
  EmitReadyField(
      out,
      "interop_header_module_bridge_cross_module_packaging_ready",
      !sections.imported_interop_header_module_bridge_module_names_lexicographic
           .empty());
  EmitReadyField(
      out,
      "metaprogramming_host_cache_cross_module_preservation_ready",
      !sections.imported_metaprogramming_host_cache_module_names_lexicographic
           .empty());

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
