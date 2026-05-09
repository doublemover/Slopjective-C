#include "artifacts/objc3_frontend_artifact_dispatch_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendDispatchMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    const Objc3DispatchDispatchControlLoweringContract
        &dispatch_dispatch_control_lowering_contract,
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &dispatch_dispatch_metadata_interface_preservation_summary) {
  ir_frontend_metadata.lowering_dispatch_dispatch_control_replay_key =
      dispatch_dispatch_control_lowering_replay_key;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_direct_call_candidate_sites =
      dispatch_dispatch_control_lowering_contract.direct_call_candidate_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_direct_members_defaulted_sites =
      dispatch_dispatch_control_lowering_contract.direct_members_defaulted_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_dynamic_opt_out_sites =
      dispatch_dispatch_control_lowering_contract.dynamic_opt_out_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_final_container_sites =
      dispatch_dispatch_control_lowering_contract.final_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_sealed_container_sites =
      dispatch_dispatch_control_lowering_contract.sealed_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_override_legality_sites =
      dispatch_dispatch_control_lowering_contract.override_legality_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites =
      dispatch_dispatch_control_lowering_contract
          .metadata_preserved_callable_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_container_sites =
      dispatch_dispatch_control_lowering_contract
          .metadata_preserved_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_guard_blocked_sites =
      dispatch_dispatch_control_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_contract_violation_sites =
      dispatch_dispatch_control_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_dispatch_dispatch_control_lowering_handoff =
      dispatch_dispatch_control_lowering_contract.deterministic;

  ir_frontend_metadata
      .lowering_dispatch_dispatch_metadata_interface_preservation_key =
      dispatch_dispatch_metadata_interface_preservation_summary.replay_key;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_direct_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_direct_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_final_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_final_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_final_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_sealed_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_sealed_container_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_module_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_module_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_direct_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_direct_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_final_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_final_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_final_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_sealed_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_sealed_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_runtime_import_artifact_ready =
      dispatch_dispatch_metadata_interface_preservation_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_separate_compilation_preservation_ready =
      dispatch_dispatch_metadata_interface_preservation_summary
          .separate_compilation_preservation_ready;
  ir_frontend_metadata
      .deterministic_dispatch_dispatch_metadata_interface_handoff =
      dispatch_dispatch_metadata_interface_preservation_summary.deterministic;
}

}  // namespace objc3::artifacts::frontend
