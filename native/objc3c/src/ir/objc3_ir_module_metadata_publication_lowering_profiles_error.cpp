#include "ir/objc3_ir_module_metadata_publication_lowering_profiles_error.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataErrorLoweringProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_throws_propagation_lowering_profile = throws_propagation_sites="
      << frontend_metadata_.throws_propagation_lowering_sites
      << ", namespace_segment_sites="
      << frontend_metadata_.throws_propagation_lowering_namespace_segment_sites
      << ", import_edge_candidate_sites="
      << frontend_metadata_
             .throws_propagation_lowering_import_edge_candidate_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_.throws_propagation_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_.throws_propagation_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_.throws_propagation_lowering_normalized_sites
      << ", cache_invalidation_candidate_sites="
      << frontend_metadata_
             .throws_propagation_lowering_cache_invalidation_candidate_sites
      << ", contract_violation_sites="
      << frontend_metadata_.throws_propagation_lowering_contract_violation_sites
      << ", deterministic_throws_propagation_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_throws_propagation_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_ns_error_bridging_lowering_profile = ns_error_bridging_sites="
      << frontend_metadata_.ns_error_bridging_lowering_sites
      << ", ns_error_parameter_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_ns_error_parameter_sites
      << ", ns_error_out_parameter_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_ns_error_out_parameter_sites
      << ", ns_error_bridge_path_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_ns_error_bridge_path_sites
      << ", failable_call_sites="
      << frontend_metadata_.ns_error_bridging_lowering_failable_call_sites
      << ", normalized_sites="
      << frontend_metadata_.ns_error_bridging_lowering_normalized_sites
      << ", bridge_boundary_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_bridge_boundary_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_contract_violation_sites
      << ", deterministic_ns_error_bridging_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_ns_error_bridging_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_unwind_cleanup_lowering_profile = unwind_cleanup_sites="
      << frontend_metadata_.unwind_cleanup_lowering_sites
      << ", unwind_edge_sites="
      << frontend_metadata_.unwind_cleanup_lowering_unwind_edge_sites
      << ", cleanup_scope_sites="
      << frontend_metadata_.unwind_cleanup_lowering_cleanup_scope_sites
      << ", cleanup_emit_sites="
      << frontend_metadata_.unwind_cleanup_lowering_cleanup_emit_sites
      << ", landing_pad_sites="
      << frontend_metadata_.unwind_cleanup_lowering_landing_pad_sites
      << ", cleanup_resume_sites="
      << frontend_metadata_.unwind_cleanup_lowering_cleanup_resume_sites
      << ", normalized_sites="
      << frontend_metadata_.unwind_cleanup_lowering_normalized_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.unwind_cleanup_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_.unwind_cleanup_lowering_contract_violation_sites
      << ", deterministic_unwind_cleanup_lowering_handoff="
      << (frontend_metadata_.deterministic_unwind_cleanup_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_error_diagnostics_recovery_lowering_profile = error_diagnostic_sites="
      << frontend_metadata_.error_diagnostics_recovery_lowering_sites
      << ", parser_diagnostic_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_parser_diagnostic_sites
      << ", semantic_diagnostic_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_semantic_diagnostic_sites
      << ", fixit_hint_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_fixit_hint_sites
      << ", recovery_candidate_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_recovery_candidate_sites
      << ", recovery_applied_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_recovery_applied_sites
      << ", normalized_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_normalized_sites
      << ", guard_blocked_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_contract_violation_sites
      << ", deterministic_error_diagnostics_recovery_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_error_diagnostics_recovery_lowering_handoff
              ? "true"
              : "false")
      << "\n";
}
