#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRErrorHandlingLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!34 = !{i64 "
      << static_cast<unsigned long long>(metadata.throws_propagation_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.throws_propagation_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_cache_invalidation_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.throws_propagation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_throws_propagation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!35 = !{i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_unwind_edge_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_cleanup_scope_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_cleanup_emit_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_landing_pad_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_cleanup_resume_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.unwind_cleanup_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unwind_cleanup_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_unwind_cleanup_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!36 = !{i64 "
      << static_cast<unsigned long long>(metadata.ns_error_bridging_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_ns_error_parameter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_ns_error_out_parameter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_ns_error_bridge_path_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.ns_error_bridging_lowering_failable_call_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.ns_error_bridging_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_bridge_boundary_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ns_error_bridging_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_ns_error_bridging_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
