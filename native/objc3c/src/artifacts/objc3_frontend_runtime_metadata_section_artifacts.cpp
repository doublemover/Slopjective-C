#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <cstddef>
#include <sstream>
#include <string>

#include "artifacts/objc3_runtime_metadata_section_publication_artifact_builders.h"
#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildExecutableMetadataSemanticConsistencyBoundaryJson(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(boundary.contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(boundary.executable_metadata_source_graph_contract_id)
      << "\",\"semantic_boundary_frozen\":"
      << (boundary.semantic_boundary_frozen ? "true" : "false")
      << ",\"lowering_admission_ready\":"
      << (boundary.lowering_admission_ready ? "true" : "false")
      << ",\"fail_closed\":" << (boundary.fail_closed ? "true" : "false")
      << ",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(boundary)
              ? "true"
              : "false")
      << ",\"source_graph_ready\":"
      << (boundary.source_graph_ready ? "true" : "false")
      << ",\"protocol_category_handoff_deterministic\":"
      << (boundary.protocol_category_handoff_deterministic ? "true" : "false")
      << ",\"class_protocol_category_linking_deterministic\":"
      << (boundary.class_protocol_category_linking_deterministic ? "true"
                                                                 : "false")
      << ",\"selector_normalization_deterministic\":"
      << (boundary.selector_normalization_deterministic ? "true" : "false")
      << ",\"property_attribute_deterministic\":"
      << (boundary.property_attribute_deterministic ? "true" : "false")
      << ",\"symbol_graph_scope_resolution_deterministic\":"
      << (boundary.symbol_graph_scope_resolution_deterministic ? "true"
                                                               : "false")
      << ",\"protocol_inheritance_edges_complete\":"
      << (boundary.protocol_inheritance_edges_complete ? "true" : "false")
      << ",\"category_attachment_edges_complete\":"
      << (boundary.category_attachment_edges_complete ? "true" : "false")
      << ",\"declaration_export_owner_split_complete\":"
      << (boundary.declaration_export_owner_split_complete ? "true" : "false")
      << ",\"property_method_ivar_owner_edges_complete\":"
      << (boundary.property_method_ivar_owner_edges_complete ? "true" : "false")
      << ",\"semantic_conflict_diagnostics_enforcement_pending\":"
      << (boundary.semantic_conflict_diagnostics_enforcement_pending ? "true"
                                                                     : "false")
      << ",\"duplicate_export_owner_enforcement_pending\":"
      << (boundary.duplicate_export_owner_enforcement_pending ? "true"
                                                              : "false")
      << ",\"lowering_admission_pending\":"
      << (boundary.lowering_admission_pending ? "true" : "false")
      << ",\"protocol_node_count\":" << boundary.protocol_node_count
      << ",\"category_node_count\":" << boundary.category_node_count
      << ",\"property_node_count\":" << boundary.property_node_count
      << ",\"method_node_count\":" << boundary.method_node_count
      << ",\"ivar_node_count\":" << boundary.ivar_node_count
      << ",\"owner_edge_count\":" << boundary.owner_edge_count
      << ",\"failure_reason\":\"" << EscapeJsonString(boundary.failure_reason)
      << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataSemanticValidationSurfaceJson(
    const Objc3ExecutableMetadataSemanticValidationSurface &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataSemanticValidationSurface(surface)
              ? "true"
              : "false")
      << ",\"method_lookup_override_conflict_handoff_deterministic\":"
      << (surface.method_lookup_override_conflict_handoff_deterministic
              ? "true"
              : "false")
      << ",\"class_protocol_category_linking_deterministic\":"
      << (surface.class_protocol_category_linking_deterministic ? "true"
                                                                : "false")
      << ",\"class_inheritance_edges_complete\":"
      << (surface.class_inheritance_edges_complete ? "true" : "false")
      << ",\"protocol_inheritance_edges_complete\":"
      << (surface.protocol_inheritance_edges_complete ? "true" : "false")
      << ",\"metaclass_edges_complete\":"
      << (surface.metaclass_edges_complete ? "true" : "false")
      << ",\"inheritance_chain_cycle_free\":"
      << (surface.inheritance_chain_cycle_free ? "true" : "false")
      << ",\"superclass_targets_resolved\":"
      << (surface.superclass_targets_resolved ? "true" : "false")
      << ",\"protocol_inheritance_targets_resolved\":"
      << (surface.protocol_inheritance_targets_resolved ? "true" : "false")
      << ",\"metaclass_targets_resolved\":"
      << (surface.metaclass_targets_resolved ? "true" : "false")
      << ",\"metaclass_lineage_aligned\":"
      << (surface.metaclass_lineage_aligned ? "true" : "false")
      << ",\"method_override_edges_complete\":"
      << (surface.method_override_edges_complete ? "true" : "false")
      << ",\"override_lookup_complete\":"
      << (surface.override_lookup_complete ? "true" : "false")
      << ",\"override_conflicts_absent\":"
      << (surface.override_conflicts_absent ? "true" : "false")
      << ",\"protocol_composition_valid\":"
      << (surface.protocol_composition_valid ? "true" : "false")
      << ",\"inheritance_validation_ready\":"
      << (surface.inheritance_validation_ready ? "true" : "false")
      << ",\"override_validation_ready\":"
      << (surface.override_validation_ready ? "true" : "false")
      << ",\"protocol_composition_validation_ready\":"
      << (surface.protocol_composition_validation_ready ? "true" : "false")
      << ",\"metaclass_relationship_validation_ready\":"
      << (surface.metaclass_relationship_validation_ready ? "true" : "false")
      << ",\"semantic_validation_complete\":"
      << (surface.semantic_validation_complete ? "true" : "false")
      << ",\"lowering_admission_ready\":"
      << (surface.lowering_admission_ready ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"class_inheritance_edge_count\":"
      << surface.class_inheritance_edge_count
      << ",\"protocol_inheritance_edge_count\":"
      << surface.protocol_inheritance_edge_count
      << ",\"metaclass_super_edge_count\":"
      << surface.metaclass_super_edge_count
      << ",\"override_edge_count\":" << surface.override_edge_count
      << ",\"class_method_override_edge_count\":"
      << surface.class_method_override_edge_count
      << ",\"instance_method_override_edge_count\":"
      << surface.instance_method_override_edge_count
      << ",\"override_lookup_sites\":" << surface.override_lookup_sites
      << ",\"override_lookup_hits\":" << surface.override_lookup_hits
      << ",\"override_lookup_misses\":" << surface.override_lookup_misses
      << ",\"override_conflicts\":" << surface.override_conflicts
      << ",\"unresolved_base_interfaces\":"
      << surface.unresolved_base_interfaces
      << ",\"protocol_composition_sites\":"
      << surface.protocol_composition_sites
      << ",\"protocol_composition_symbols\":"
      << surface.protocol_composition_symbols
      << ",\"category_composition_sites\":"
      << surface.category_composition_sites
      << ",\"category_composition_symbols\":"
      << surface.category_composition_symbols
      << ",\"invalid_protocol_composition_sites\":"
      << surface.invalid_protocol_composition_sites
      << ",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataLoweringHandoffSurfaceJson(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(surface.executable_metadata_source_graph_contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"executable_metadata_semantic_validation_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_validation_contract_id)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(surface)
              ? "true"
              : "false")
      << ",\"source_graph_ready\":"
      << (surface.source_graph_ready ? "true" : "false")
      << ",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"semantic_validation_ready\":"
      << (surface.semantic_validation_ready ? "true" : "false")
      << ",\"semantic_type_metadata_handoff_deterministic\":"
      << (surface.semantic_type_metadata_handoff_deterministic ? "true"
                                                               : "false")
      << ",\"protocol_category_handoff_deterministic\":"
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ",\"class_protocol_category_linking_handoff_deterministic\":"
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true"
                                                                        : "false")
      << ",\"selector_normalization_handoff_deterministic\":"
      << (surface.selector_normalization_handoff_deterministic ? "true"
                                                               : "false")
      << ",\"property_attribute_handoff_deterministic\":"
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ",\"symbol_graph_scope_resolution_handoff_deterministic\":"
      << (surface.symbol_graph_scope_resolution_handoff_deterministic ? "true"
                                                                      : "false")
      << ",\"property_synthesis_ivar_binding_handoff_deterministic\":"
      << (surface.property_synthesis_ivar_binding_handoff_deterministic ? "true"
                                                                        : "false")
      << ",\"lowering_schema_frozen\":"
      << (surface.lowering_schema_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (surface.ready_for_lowering ? "true" : "false")
      << ",\"interface_node_count\":" << surface.interface_node_count
      << ",\"implementation_node_count\":"
      << surface.implementation_node_count
      << ",\"class_node_count\":" << surface.class_node_count
      << ",\"metaclass_node_count\":" << surface.metaclass_node_count
      << ",\"protocol_node_count\":" << surface.protocol_node_count
      << ",\"category_node_count\":" << surface.category_node_count
      << ",\"property_node_count\":" << surface.property_node_count
      << ",\"method_node_count\":" << surface.method_node_count
      << ",\"ivar_node_count\":" << surface.ivar_node_count
      << ",\"owner_edge_count\":" << surface.owner_edge_count
      << ",\"replay_key\":\"" << EscapeJsonString(surface.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataTypedLoweringHandoffJson(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_lowering_handoff_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_lowering_handoff_contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(surface.executable_metadata_source_graph_contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"executable_metadata_semantic_validation_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_validation_contract_id)
      << "\",\"manifest_schema_ordering_model\":\""
      << EscapeJsonString(surface.manifest_schema_ordering_model)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(surface) ? "true"
                                                                     : "false")
      << ",\"source_graph_ready\":"
      << (surface.source_graph_ready ? "true" : "false")
      << ",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"semantic_validation_ready\":"
      << (surface.semantic_validation_ready ? "true" : "false")
      << ",\"lowering_handoff_surface_ready\":"
      << (surface.lowering_handoff_surface_ready ? "true" : "false")
      << ",\"deterministic\":" << (surface.deterministic ? "true" : "false")
      << ",\"manifest_schema_frozen\":"
      << (surface.manifest_schema_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (surface.ready_for_lowering ? "true" : "false")
      << ",\"source_graph\":"
      << BuildExecutableMetadataSourceGraphJson(surface.source_graph)
      << ",\"replay_key\":\"" << EscapeJsonString(surface.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

Objc3RuntimeMetadataSectionAbiFreezeSummary
BuildRuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::BuildAbiFreeze(
          runtime_metadata_source_ownership,
          runtime_export_legality,
          runtime_export_enforcement);
}

Objc3RuntimeMetadataSectionPublicationSummary
BuildRuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::BuildPublication(
          runtime_metadata_section_abi,
          runtime_export_legality,
          runtime_export_enforcement);
}

Objc3RuntimeMetadataObjectInspectionHarnessSummary
BuildRuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::
          BuildObjectInspectionHarness(runtime_metadata_section_abi,
                                       runtime_metadata_section_publication);
}

}  // namespace objc3::artifacts::frontend
