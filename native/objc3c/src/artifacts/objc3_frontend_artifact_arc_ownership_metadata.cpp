#include "artifacts/objc3_frontend_artifact_arc_ownership_metadata.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/lowering_arc_contracts.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendArcOwnershipMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &ownership_qualifier_lowering_replay_key,
    const Objc3OwnershipQualifierLoweringContract
        &ownership_qualifier_lowering_contract,
    const std::string &retain_release_operation_lowering_replay_key,
    const Objc3RetainReleaseOperationLoweringContract
        &retain_release_operation_lowering_contract,
    const std::string &autoreleasepool_scope_lowering_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract
        &autoreleasepool_scope_lowering_contract,
    const std::string &weak_unowned_semantics_lowering_replay_key,
    const Objc3WeakUnownedSemanticsLoweringContract
        &weak_unowned_semantics_lowering_contract,
    const std::string &arc_diagnostics_fixit_lowering_replay_key,
    const Objc3ArcDiagnosticsFixitLoweringContract
        &arc_diagnostics_fixit_lowering_contract) {
  ir_frontend_metadata.lowering_ownership_qualifier_replay_key =
      ownership_qualifier_lowering_replay_key;
  ir_frontend_metadata.ownership_qualifier_lowering_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata
      .ownership_qualifier_lowering_invalid_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites;
  ir_frontend_metadata
      .ownership_qualifier_lowering_object_pointer_type_annotation_sites =
      ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites;
  ir_frontend_metadata.deterministic_ownership_qualifier_lowering_handoff =
      ownership_qualifier_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_retain_release_operation_replay_key =
      retain_release_operation_lowering_replay_key;
  ir_frontend_metadata
      .retain_release_operation_lowering_ownership_qualified_sites =
      retain_release_operation_lowering_contract.ownership_qualified_sites;
  ir_frontend_metadata.retain_release_operation_lowering_retain_insertion_sites =
      retain_release_operation_lowering_contract.retain_insertion_sites;
  ir_frontend_metadata
      .retain_release_operation_lowering_release_insertion_sites =
      retain_release_operation_lowering_contract.release_insertion_sites;
  ir_frontend_metadata
      .retain_release_operation_lowering_autorelease_insertion_sites =
      retain_release_operation_lowering_contract.autorelease_insertion_sites;
  ir_frontend_metadata
      .retain_release_operation_lowering_contract_violation_sites =
      retain_release_operation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_retain_release_operation_lowering_handoff =
      retain_release_operation_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_autoreleasepool_scope_replay_key =
      autoreleasepool_scope_lowering_replay_key;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_sites =
      autoreleasepool_scope_lowering_contract.scope_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_symbolized_sites =
      autoreleasepool_scope_lowering_contract.scope_symbolized_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_max_scope_depth =
      autoreleasepool_scope_lowering_contract.max_scope_depth;
  ir_frontend_metadata
      .autoreleasepool_scope_lowering_scope_entry_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_entry_transition_sites;
  ir_frontend_metadata
      .autoreleasepool_scope_lowering_scope_exit_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_exit_transition_sites;
  ir_frontend_metadata
      .autoreleasepool_scope_lowering_contract_violation_sites =
      autoreleasepool_scope_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_autoreleasepool_scope_lowering_handoff =
      autoreleasepool_scope_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_weak_unowned_semantics_replay_key =
      weak_unowned_semantics_lowering_replay_key;
  ir_frontend_metadata
      .weak_unowned_semantics_lowering_ownership_candidate_sites =
      weak_unowned_semantics_lowering_contract.ownership_candidate_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_weak_reference_sites =
      weak_unowned_semantics_lowering_contract.weak_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_reference_sites;
  ir_frontend_metadata
      .weak_unowned_semantics_lowering_unowned_safe_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_conflict_sites =
      weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites;
  ir_frontend_metadata
      .weak_unowned_semantics_lowering_contract_violation_sites =
      weak_unowned_semantics_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_weak_unowned_semantics_lowering_handoff =
      weak_unowned_semantics_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_arc_diagnostics_fixit_replay_key =
      arc_diagnostics_fixit_lowering_replay_key;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites =
      arc_diagnostics_fixit_lowering_contract
          .ownership_arc_diagnostic_candidate_sites;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites =
      arc_diagnostics_fixit_lowering_contract
          .ownership_arc_fixit_available_sites;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites =
      arc_diagnostics_fixit_lowering_contract
          .ownership_arc_weak_unowned_conflict_diagnostic_sites;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites =
      arc_diagnostics_fixit_lowering_contract
          .ownership_arc_empty_fixit_hint_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_contract_violation_sites =
      arc_diagnostics_fixit_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_arc_diagnostics_fixit_lowering_handoff =
      arc_diagnostics_fixit_lowering_contract.deterministic;
}

}  // namespace objc3::artifacts::frontend
