#include "io/objc3_cross_module_runtime_link_plan_document_header.h"

#include <ostream>
#include <string>

#include "io/objc3_cross_module_runtime_link_plan_document_header_modules.h"
#include "io/objc3_cross_module_runtime_link_plan_document_header_runtime.h"
#include "io/objc3_process_internal.h"

namespace {

void EmitStringField(std::ostream &out,
                     const char *name,
                     const std::string &value) {
  out << "  \"" << name << "\": \"" << EscapeJsonString(value) << "\",\n";
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
  EmitStringField(
      out,
      "expected_block_ownership_retain_release_operation_lowering_contract_id",
      inputs
          .expected_block_ownership_retain_release_operation_lowering_contract_id);
  EmitStringField(
      out,
      "expected_block_ownership_autoreleasepool_scope_lowering_contract_id",
      inputs
          .expected_block_ownership_autoreleasepool_scope_lowering_contract_id);
  EmitObjc3CrossModuleRuntimeLinkPlanHeaderModuleSections(out, sections);
  EmitObjc3CrossModuleRuntimeLinkPlanHeaderRuntimeSurfaces(out, inputs);
}
