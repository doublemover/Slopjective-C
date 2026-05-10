#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_ownership_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_row_helpers.h"

void EmitObjc3IROwnershipLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRDispatchOwnershipArcCounterRow(
      "!15",
      {metadata.retain_release_operation_lowering_ownership_qualified_sites,
       metadata.retain_release_operation_lowering_retain_insertion_sites,
       metadata.retain_release_operation_lowering_release_insertion_sites,
       metadata.retain_release_operation_lowering_autorelease_insertion_sites,
       metadata.retain_release_operation_lowering_contract_violation_sites},
      metadata.deterministic_retain_release_operation_lowering_handoff, out);
  EmitObjc3IRDispatchOwnershipArcCounterRow(
      "!16",
      {metadata.autoreleasepool_scope_lowering_scope_sites,
       metadata.autoreleasepool_scope_lowering_scope_symbolized_sites,
       metadata.autoreleasepool_scope_lowering_max_scope_depth,
       metadata.autoreleasepool_scope_lowering_scope_entry_transition_sites,
       metadata.autoreleasepool_scope_lowering_scope_exit_transition_sites,
       metadata.autoreleasepool_scope_lowering_contract_violation_sites},
      metadata.deterministic_autoreleasepool_scope_lowering_handoff, out);
  EmitObjc3IRDispatchOwnershipArcCounterRow(
      "!17",
      {metadata.weak_unowned_semantics_lowering_ownership_candidate_sites,
       metadata.weak_unowned_semantics_lowering_weak_reference_sites,
       metadata.weak_unowned_semantics_lowering_unowned_reference_sites,
       metadata.weak_unowned_semantics_lowering_unowned_safe_reference_sites,
       metadata.weak_unowned_semantics_lowering_conflict_sites,
       metadata.weak_unowned_semantics_lowering_contract_violation_sites},
      metadata.deterministic_weak_unowned_semantics_lowering_handoff, out);
}
