#include "artifacts/objc3_frontend_artifact_interop_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendInteropMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &interop_interop_lowering_replay_key,
    const Objc3InteropInteropLoweringContract &interop_interop_lowering_contract,
    const std::string &interop_foreign_call_lifetime_lowering_replay_key,
    const Objc3InteropForeignCallLifetimeLoweringContract
        &interop_foreign_call_lifetime_lowering_contract,
    const std::string &interop_ffi_metadata_interface_preservation_replay_key,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &interop_ffi_metadata_interface_preservation_contract,
    const Objc3InteropHeaderModuleBridgeGenerationSummary
        &interop_header_module_bridge_generation_summary) {
  ir_frontend_metadata.lowering_interop_interop_replay_key =
      interop_interop_lowering_replay_key;
  ir_frontend_metadata.interop_interop_lowering_foreign_callable_sites =
      interop_interop_lowering_contract.foreign_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_c_foreign_callable_sites =
      interop_interop_lowering_contract.c_foreign_callable_sites;
  ir_frontend_metadata
      .interop_interop_lowering_objc_runtime_parity_callable_sites =
      interop_interop_lowering_contract.objc_runtime_parity_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_ownership_bridge_callable_sites =
      interop_interop_lowering_contract.ownership_bridge_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_error_surface_sites =
      interop_interop_lowering_contract.error_surface_sites;
  ir_frontend_metadata.interop_interop_lowering_async_boundary_sites =
      interop_interop_lowering_contract.async_boundary_sites;
  ir_frontend_metadata.interop_interop_lowering_swift_concurrency_metadata_sites =
      interop_interop_lowering_contract.swift_concurrency_metadata_sites;
  ir_frontend_metadata
      .interop_interop_lowering_interface_preserved_foreign_callable_sites =
      interop_interop_lowering_contract
          .interface_preserved_foreign_callable_sites;
  ir_frontend_metadata
      .interop_interop_lowering_interface_preserved_metadata_annotation_sites =
      interop_interop_lowering_contract
          .interface_preserved_metadata_annotation_sites;
  ir_frontend_metadata.interop_interop_lowering_guard_blocked_sites =
      interop_interop_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.interop_interop_lowering_contract_violation_sites =
      interop_interop_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_interop_interop_lowering_handoff =
      interop_interop_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_interop_foreign_call_lifetime_replay_key =
      interop_foreign_call_lifetime_lowering_replay_key;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_foreign_callable_sites =
      interop_foreign_call_lifetime_lowering_contract.foreign_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_c_foreign_callable_sites =
      interop_foreign_call_lifetime_lowering_contract.c_foreign_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_objc_runtime_parity_callable_sites =
      interop_foreign_call_lifetime_lowering_contract
          .objc_runtime_parity_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_ownership_bridge_sites =
      interop_foreign_call_lifetime_lowering_contract.ownership_bridge_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_lifetime_bridge_sites =
      interop_foreign_call_lifetime_lowering_contract.lifetime_bridge_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_metadata_preservation_sites =
      interop_foreign_call_lifetime_lowering_contract
          .metadata_preservation_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_guard_blocked_sites =
      interop_foreign_call_lifetime_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_contract_violation_sites =
      interop_foreign_call_lifetime_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_interop_foreign_call_lifetime_lowering_handoff =
      interop_foreign_call_lifetime_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_interop_ffi_metadata_interface_preservation_key =
      interop_ffi_metadata_interface_preservation_replay_key;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_foreign_callable_count =
      interop_ffi_metadata_interface_preservation_contract
          .local_foreign_callable_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_metadata_preservation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .local_metadata_preservation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_interface_annotation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .local_interface_annotation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_module_count =
      interop_ffi_metadata_interface_preservation_contract.imported_module_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_foreign_callable_count =
      interop_ffi_metadata_interface_preservation_contract
          .imported_foreign_callable_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_metadata_preservation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .imported_metadata_preservation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_interface_annotation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .imported_interface_annotation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_runtime_import_artifact_ready =
      interop_ffi_metadata_interface_preservation_contract
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_separate_compilation_preservation_ready =
      interop_ffi_metadata_interface_preservation_contract
          .separate_compilation_preservation_ready;
  ir_frontend_metadata
      .deterministic_interop_ffi_metadata_interface_preservation_handoff =
      interop_ffi_metadata_interface_preservation_contract.deterministic;
  ir_frontend_metadata.lowering_interop_header_module_bridge_generation_key =
      interop_header_module_bridge_generation_summary.replay_key;
}

}  // namespace objc3::artifacts::frontend
