#pragma once

#include <sstream>
#include <string>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend::interop_semantic_json_detail {

using objc3::io::EscapeJsonString;

inline void AppendInteropInteropLoweringContractJsonFields(
    std::ostringstream &out,
    const Objc3InteropInteropSemanticModelSummary &semantic_summary,
    const Objc3InteropInteropRuntimeParitySummary &runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropSwiftInteropIsolationSummary &swift_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropInteropLoweringContract &contract,
    const std::string &replay_key,
    bool ready_for_ir_emission) {
  out << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"runtime_parity_contract_id\":\""
      << EscapeJsonString(runtime_parity_summary.contract_id)
      << "\",\"cpp_interaction_contract_id\":\""
      << EscapeJsonString(cpp_summary.contract_id)
      << "\",\"swift_isolation_contract_id\":\""
      << EscapeJsonString(swift_summary.contract_id)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(preservation_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"foreign_callable_sites\":" << contract.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << contract.c_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << contract.objc_runtime_parity_callable_sites
      << ",\"ownership_bridge_callable_sites\":"
      << contract.ownership_bridge_callable_sites
      << ",\"error_surface_sites\":" << contract.error_surface_sites
      << ",\"async_boundary_sites\":" << contract.async_boundary_sites
      << ",\"swift_concurrency_metadata_sites\":"
      << contract.swift_concurrency_metadata_sites
      << ",\"interface_preserved_foreign_callable_sites\":"
      << contract.interface_preserved_foreign_callable_sites
      << ",\"interface_preserved_metadata_annotation_sites\":"
      << contract.interface_preserved_metadata_annotation_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false");
}

inline void AppendInteropForeignCallLifetimeLoweringContractJsonFields(
    std::ostringstream &out,
    const Objc3InteropInteropLoweringContract &dependency_contract,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropForeignCallLifetimeLoweringContract &contract,
    const std::string &replay_key,
    bool ready_for_ir_emission) {
  out << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringSurfacePath)
      << "\",\"interop_contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringContractId)
      << "\",\"bridge_dependency_contract_id\":\""
      << EscapeJsonString(kObjc3InteropCppInteropInteractionSummaryContractId)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(preservation_summary.contract_id)
      << "\",\"dependency_replay_key\":\""
      << EscapeJsonString(Objc3InteropInteropLoweringReplayKey(
             dependency_contract))
      << "\",\"cpp_dependency_replay_key\":\""
      << EscapeJsonString(cpp_summary.replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringDeferredModel)
      << "\",\"foreign_callable_sites\":" << contract.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << contract.c_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << contract.objc_runtime_parity_callable_sites
      << ",\"ownership_bridge_sites\":" << contract.ownership_bridge_sites
      << ",\"lifetime_bridge_sites\":" << contract.lifetime_bridge_sites
      << ",\"metadata_preservation_sites\":"
      << contract.metadata_preservation_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false");
}

inline void AppendInteropFfiMetadataInterfacePreservationContractJsonFields(
    std::ostringstream &out,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract,
    const std::string &replay_key) {
  out << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropFfiMetadataInterfacePreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationSourceContractId)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(
             kObjc3InteropForeignSurfaceInterfacePreservationContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationSurfacePath)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName)
      << "\",\"source_model\":\""
      << EscapeJsonString(kObjc3InteropFfiMetadataInterfacePreservationSourceModel)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationPreservationModel)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationFailClosedModel)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(lowering_replay_key)
      << "\",\"preservation_replay_key\":\""
      << EscapeJsonString(preservation_summary.replay_key)
      << "\",\"local_foreign_callable_count\":"
      << contract.local_foreign_callable_count
      << ",\"local_metadata_preservation_sites\":"
      << contract.local_metadata_preservation_sites
      << ",\"local_interface_annotation_sites\":"
      << contract.local_interface_annotation_sites
      << ",\"imported_module_count\":" << contract.imported_module_count
      << ",\"imported_foreign_callable_count\":"
      << contract.imported_foreign_callable_count
      << ",\"imported_metadata_preservation_sites\":"
      << contract.imported_metadata_preservation_sites
      << ",\"imported_interface_annotation_sites\":"
      << contract.imported_interface_annotation_sites
      << ",\"runtime_import_artifact_ready\":"
      << (contract.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (contract.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"";
}

}  // namespace objc3::artifacts::frontend::interop_semantic_json_detail
