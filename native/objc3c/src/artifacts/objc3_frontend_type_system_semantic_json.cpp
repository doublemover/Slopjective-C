#include "artifacts/objc3_frontend_type_system_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildTypeSystemTypeSemanticModelSummaryJson(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"optional_binding_sites\":" << summary.optional_binding_sites
      << ",\"optional_binding_clause_sites\":"
      << summary.optional_binding_clause_sites
      << ",\"guard_binding_sites\":" << summary.guard_binding_sites
      << ",\"optional_send_sites\":" << summary.optional_send_sites
      << ",\"nil_coalescing_sites\":" << summary.nil_coalescing_sites
      << ",\"optional_propagation_sites\":"
      << summary.optional_propagation_sites
      << ",\"optional_flow_refinement_sites\":"
      << summary.optional_flow_refinement_sites
      << ",\"guard_binding_exit_enforcement_sites\":"
      << summary.guard_binding_exit_enforcement_sites
      << ",\"typed_keypath_literal_sites\":"
      << summary.typed_keypath_literal_sites
      << ",\"typed_keypath_self_root_sites\":"
      << summary.typed_keypath_self_root_sites
      << ",\"typed_keypath_class_root_sites\":"
      << summary.typed_keypath_class_root_sites
      << ",\"object_pointer_semantic_sites\":"
      << summary.object_pointer_semantic_sites
      << ",\"protocol_composition_semantic_sites\":"
      << summary.protocol_composition_semantic_sites
      << ",\"generic_suffix_semantic_sites\":"
      << summary.generic_suffix_semantic_sites
      << ",\"generic_erasure_semantic_sites\":"
      << summary.generic_erasure_semantic_sites
      << ",\"nullability_suffix_semantic_sites\":"
      << summary.nullability_suffix_semantic_sites
      << ",\"nullability_semantic_sites\":"
      << summary.nullability_semantic_sites
      << ",\"canonical_type_entries\":"
      << summary.canonical_type_entries
      << ",\"canonical_object_type_entries\":"
      << summary.canonical_object_type_entries
      << ",\"canonical_protocol_qualified_entries\":"
      << summary.canonical_protocol_qualified_entries
      << ",\"canonical_generic_argument_entries\":"
      << summary.canonical_generic_argument_entries
      << ",\"canonical_nullable_entries\":"
      << summary.canonical_nullable_entries
      << ",\"canonical_nonnull_entries\":"
      << summary.canonical_nonnull_entries
      << ",\"canonical_implicitly_unwrapped_entries\":"
      << summary.canonical_implicitly_unwrapped_entries
      << ",\"canonical_null_resettable_entries\":"
      << summary.canonical_null_resettable_entries
      << ",\"canonical_unspecified_nullability_entries\":"
      << summary.canonical_unspecified_nullability_entries
      << ",\"canonical_invalid_type_entries\":"
      << summary.canonical_invalid_type_entries
      << ",\"invalid_generic_suffix_semantic_sites\":"
      << summary.invalid_generic_suffix_semantic_sites
      << ",\"invalid_nullability_suffix_semantic_sites\":"
      << summary.invalid_nullability_suffix_semantic_sites
      << ",\"invalid_protocol_composition_semantic_sites\":"
      << summary.invalid_protocol_composition_semantic_sites
      << ",\"optional_binding_contract_violation_sites\":"
      << summary.optional_binding_contract_violation_sites
      << ",\"optional_send_contract_violation_sites\":"
      << summary.optional_send_contract_violation_sites
      << ",\"optional_flow_contract_violation_sites\":"
      << summary.optional_flow_contract_violation_sites
      << ",\"typed_keypath_root_legality_violation_sites\":"
      << summary.typed_keypath_root_legality_violation_sites
      << ",\"typed_keypath_member_path_contract_violation_sites\":"
      << summary.typed_keypath_member_path_contract_violation_sites
      << ",\"typed_keypath_contract_violation_sites\":"
      << summary.typed_keypath_contract_violation_sites
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
