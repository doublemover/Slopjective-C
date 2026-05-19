#include "ir/objc3_ir_lowering_extension_metadata_publication_dispatch.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRActorDispatchControlMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!97 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_interface_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_method_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_metadata_record_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_nonisolated_entry_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_executor_affinity_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_hop_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_isolation_thunk_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_replay_proof_dependency_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_race_guard_dependency_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_task_handoff_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_actor_lowering_metadata_handoff ? 1 : 0)
      << "}\n\n";
  out << "!102 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_direct_call_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_direct_members_defaulted_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_dynamic_opt_out_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_final_container_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_sealed_container_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_override_legality_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .dispatch_dispatch_control_lowering_metadata_preserved_container_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_dispatch_dispatch_control_lowering_handoff ? 1 : 0)
      << "}\n\n";
}

void EmitObjc3IRDispatchMetadataPreservationNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!103 = !{!\""
      << EscapeCStringLiteral(
             metadata.lowering_dispatch_dispatch_metadata_interface_preservation_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_local_direct_callable_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_local_final_callable_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_local_final_container_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_local_sealed_container_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_module_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_direct_callable_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_final_callable_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_final_container_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_sealed_container_record_count)
      << ", i1 "
      << (metadata.dispatch_dispatch_metadata_runtime_import_artifact_ready ? 1 : 0)
      << ", i1 "
      << (metadata.dispatch_dispatch_metadata_separate_compilation_preservation_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata.deterministic_dispatch_dispatch_metadata_interface_handoff ? 1 : 0)
      << "}\n\n";
}
