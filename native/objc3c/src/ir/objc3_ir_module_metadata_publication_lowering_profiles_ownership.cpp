#include "ir/objc3_ir_module_metadata_publication_lowering_profiles_ownership.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataOwnershipLoweringProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_ownership_qualifier_lowering_profile = ownership_qualifier_sites="
      << frontend_metadata_.ownership_qualifier_lowering_ownership_qualifier_sites
      << ", invalid_ownership_qualifier_sites="
      << frontend_metadata_.ownership_qualifier_lowering_invalid_ownership_qualifier_sites
      << ", object_pointer_type_annotation_sites="
      << frontend_metadata_.ownership_qualifier_lowering_object_pointer_type_annotation_sites
      << ", deterministic_ownership_qualifier_lowering_handoff="
      << (frontend_metadata_.deterministic_ownership_qualifier_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_retain_release_operation_lowering_profile = ownership_qualified_sites="
      << frontend_metadata_.retain_release_operation_lowering_ownership_qualified_sites
      << ", retain_insertion_sites="
      << frontend_metadata_.retain_release_operation_lowering_retain_insertion_sites
      << ", release_insertion_sites="
      << frontend_metadata_.retain_release_operation_lowering_release_insertion_sites
      << ", autorelease_insertion_sites="
      << frontend_metadata_.retain_release_operation_lowering_autorelease_insertion_sites
      << ", contract_violation_sites="
      << frontend_metadata_.retain_release_operation_lowering_contract_violation_sites
      << ", deterministic_retain_release_operation_lowering_handoff="
      << (frontend_metadata_.deterministic_retain_release_operation_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_autoreleasepool_scope_lowering_profile = scope_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
      << ", scope_symbolized_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_scope_symbolized_sites
      << ", max_scope_depth="
      << frontend_metadata_.autoreleasepool_scope_lowering_max_scope_depth
      << ", scope_entry_transition_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_scope_entry_transition_sites
      << ", scope_exit_transition_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_scope_exit_transition_sites
      << ", contract_violation_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_contract_violation_sites
      << ", deterministic_autoreleasepool_scope_lowering_handoff="
      << (frontend_metadata_.deterministic_autoreleasepool_scope_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_weak_unowned_semantics_lowering_profile = ownership_candidate_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_ownership_candidate_sites
      << ", weak_reference_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_weak_reference_sites
      << ", unowned_reference_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_unowned_reference_sites
      << ", unowned_safe_reference_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_unowned_safe_reference_sites
      << ", weak_unowned_conflict_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_conflict_sites
      << ", contract_violation_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_contract_violation_sites
      << ", deterministic_weak_unowned_semantics_lowering_handoff="
      << (frontend_metadata_.deterministic_weak_unowned_semantics_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_arc_diagnostics_fixit_lowering_profile = ownership_arc_diagnostic_candidate_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites
      << ", ownership_arc_fixit_available_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites
      << ", ownership_arc_profiled_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites
      << ", ownership_arc_weak_unowned_conflict_diagnostic_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ", ownership_arc_empty_fixit_hint_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites
      << ", contract_violation_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_contract_violation_sites
      << ", deterministic_arc_diagnostics_fixit_lowering_handoff="
      << (frontend_metadata_.deterministic_arc_diagnostics_fixit_lowering_handoff ? "true" : "false")
      << "\n";
}
