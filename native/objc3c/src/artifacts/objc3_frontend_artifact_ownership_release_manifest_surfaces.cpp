#include "artifacts/objc3_frontend_artifact_ownership_release_manifest_surfaces.h"

#include <ostream>

#include "lower/contracts/lowering_arc_contracts.h"

namespace objc3::artifacts::frontend {

void WriteOwnershipReleaseManifestSurfaces(
    std::ostream &manifest,
    const Objc3OwnershipQualifierLoweringContract
        &ownership_qualifier_lowering_contract,
    const std::string &ownership_qualifier_lowering_replay_key,
    const Objc3RetainReleaseOperationLoweringContract
        &retain_release_operation_lowering_contract,
    const std::string &retain_release_operation_lowering_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract
        &autoreleasepool_scope_lowering_contract,
    const std::string &autoreleasepool_scope_lowering_replay_key,
    const Objc3WeakUnownedSemanticsLoweringContract
        &weak_unowned_semantics_lowering_contract,
    const std::string &weak_unowned_semantics_lowering_replay_key,
    const Objc3ArcDiagnosticsFixitLoweringContract
        &arc_diagnostics_fixit_lowering_contract,
    const std::string &arc_diagnostics_fixit_lowering_replay_key) {
  manifest
      << ",\"objc_ownership_qualifier_lowering_surface\":{\"ownership_qualifier_sites\":"
      << ownership_qualifier_lowering_contract.ownership_qualifier_sites
      << ",\"invalid_ownership_qualifier_sites\":"
      << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
      << ",\"object_pointer_type_annotation_sites\":"
      << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
      << ",\"replay_key\":\""
      << ownership_qualifier_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (ownership_qualifier_lowering_contract.deterministic ? "true"
                                                              : "false")
      << "}"
      << ",\"objc_retain_release_operation_lowering_surface\":{\"ownership_qualified_sites\":"
      << retain_release_operation_lowering_contract.ownership_qualified_sites
      << ",\"retain_insertion_sites\":"
      << retain_release_operation_lowering_contract.retain_insertion_sites
      << ",\"release_insertion_sites\":"
      << retain_release_operation_lowering_contract.release_insertion_sites
      << ",\"autorelease_insertion_sites\":"
      << retain_release_operation_lowering_contract.autorelease_insertion_sites
      << ",\"contract_violation_sites\":"
      << retain_release_operation_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << retain_release_operation_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (retain_release_operation_lowering_contract.deterministic ? "true"
                                                                   : "false")
      << "}"
      << ",\"objc_autoreleasepool_scope_lowering_surface\":{\"scope_sites\":"
      << autoreleasepool_scope_lowering_contract.scope_sites
      << ",\"scope_symbolized_sites\":"
      << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
      << ",\"max_scope_depth\":"
      << autoreleasepool_scope_lowering_contract.max_scope_depth
      << ",\"scope_entry_transition_sites\":"
      << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
      << ",\"scope_exit_transition_sites\":"
      << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
      << ",\"contract_violation_sites\":"
      << autoreleasepool_scope_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << autoreleasepool_scope_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (autoreleasepool_scope_lowering_contract.deterministic ? "true"
                                                                : "false")
      << "}"
      << ",\"objc_weak_unowned_semantics_lowering_surface\":{\"ownership_candidate_sites\":"
      << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
      << ",\"weak_reference_sites\":"
      << weak_unowned_semantics_lowering_contract.weak_reference_sites
      << ",\"unowned_reference_sites\":"
      << weak_unowned_semantics_lowering_contract.unowned_reference_sites
      << ",\"unowned_safe_reference_sites\":"
      << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
      << ",\"weak_unowned_conflict_sites\":"
      << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
      << ",\"contract_violation_sites\":"
      << weak_unowned_semantics_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << weak_unowned_semantics_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (weak_unowned_semantics_lowering_contract.deterministic ? "true"
                                                                 : "false")
      << "}"
      << ",\"objc_arc_diagnostics_fixit_lowering_surface\":{\"ownership_arc_diagnostic_candidate_sites\":"
      << arc_diagnostics_fixit_lowering_contract
             .ownership_arc_diagnostic_candidate_sites
      << ",\"ownership_arc_fixit_available_sites\":"
      << arc_diagnostics_fixit_lowering_contract
             .ownership_arc_fixit_available_sites
      << ",\"ownership_arc_profiled_sites\":"
      << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
      << ",\"ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
      << arc_diagnostics_fixit_lowering_contract
             .ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ",\"ownership_arc_empty_fixit_hint_sites\":"
      << arc_diagnostics_fixit_lowering_contract
             .ownership_arc_empty_fixit_hint_sites
      << ",\"contract_violation_sites\":"
      << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << arc_diagnostics_fixit_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true"
                                                                : "false")
      << "}";
}

}  // namespace objc3::artifacts::frontend
