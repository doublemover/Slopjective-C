#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IROwnershipArcLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!14 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_qualifier_lowering_ownership_qualifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_qualifier_lowering_invalid_ownership_qualifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_qualifier_lowering_object_pointer_type_annotation_sites)
      << ", i1 " << (metadata.deterministic_ownership_qualifier_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!15 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_ownership_qualified_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_retain_insertion_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_release_insertion_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_autorelease_insertion_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_retain_release_operation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!16 = !{i64 "
      << static_cast<unsigned long long>(metadata.autoreleasepool_scope_lowering_scope_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.autoreleasepool_scope_lowering_scope_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.autoreleasepool_scope_lowering_max_scope_depth)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.autoreleasepool_scope_lowering_scope_entry_transition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.autoreleasepool_scope_lowering_scope_exit_transition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.autoreleasepool_scope_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_autoreleasepool_scope_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!17 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.weak_unowned_semantics_lowering_ownership_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.weak_unowned_semantics_lowering_weak_reference_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.weak_unowned_semantics_lowering_unowned_reference_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.weak_unowned_semantics_lowering_unowned_safe_reference_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.weak_unowned_semantics_lowering_conflict_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.weak_unowned_semantics_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_weak_unowned_semantics_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!18 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.arc_diagnostics_fixit_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_arc_diagnostics_fixit_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
