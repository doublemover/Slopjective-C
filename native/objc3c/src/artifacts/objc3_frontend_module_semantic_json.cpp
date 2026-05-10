#include "artifacts/objc3_frontend_module_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
    const Objc3CrossModuleSemanticContractsDiagnosticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"module_import_graph_sites\":"
      << summary.module_import_graph_sites
      << ",\"import_edge_candidate_sites\":"
      << summary.import_edge_candidate_sites
      << ",\"namespace_segment_sites\":" << summary.namespace_segment_sites
      << ",\"object_pointer_type_sites\":"
      << summary.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << summary.pointer_declarator_sites
      << ",\"namespace_collision_shadowing_sites\":"
      << summary.namespace_collision_shadowing_sites
      << ",\"public_private_api_partition_sites\":"
      << summary.public_private_api_partition_sites
      << ",\"incremental_module_cache_invalidation_sites\":"
      << summary.incremental_module_cache_invalidation_sites
      << ",\"cross_module_conformance_sites\":"
      << summary.cross_module_conformance_sites
      << ",\"normalized_cross_module_sites\":"
      << summary.normalized_cross_module_sites
      << ",\"cache_invalidation_candidate_sites\":"
      << summary.cache_invalidation_candidate_sites
      << ",\"diagnostic_recovery_sites\":"
      << summary.diagnostic_recovery_sites
      << ",\"diagnostic_emit_sites\":" << summary.diagnostic_emit_sites
      << ",\"recovery_anchor_sites\":" << summary.recovery_anchor_sites
      << ",\"recovery_boundary_sites\":" << summary.recovery_boundary_sites
      << ",\"fail_closed_diagnostic_sites\":"
      << summary.fail_closed_diagnostic_sites
      << ",\"diagnostic_normalized_sites\":"
      << summary.diagnostic_normalized_sites
      << ",\"diagnostic_gate_blocked_sites\":"
      << summary.diagnostic_gate_blocked_sites
      << ",\"interop_import_module_annotation_sites\":"
      << summary.interop_import_module_annotation_sites
      << ",\"interop_imported_module_name_sites\":"
      << summary.interop_imported_module_name_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"module_import_graph_semantics_landed\":"
      << (summary.module_import_graph_semantics_landed ? "true" : "false")
      << ",\"namespace_collision_semantics_landed\":"
      << (summary.namespace_collision_semantics_landed ? "true" : "false")
      << ",\"public_private_partition_semantics_landed\":"
      << (summary.public_private_partition_semantics_landed ? "true" : "false")
      << ",\"incremental_cache_semantics_landed\":"
      << (summary.incremental_cache_semantics_landed ? "true" : "false")
      << ",\"cross_module_conformance_semantics_landed\":"
      << (summary.cross_module_conformance_semantics_landed ? "true" : "false")
      << ",\"diagnostic_recovery_semantics_landed\":"
      << (summary.diagnostic_recovery_semantics_landed ? "true" : "false")
      << ",\"interop_import_semantics_landed\":"
      << (summary.interop_import_semantics_landed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
