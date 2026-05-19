#pragma once

#include "sema/model/semantic_type_executable_metadata_contracts.h"
#include "runtime/metadata/executable_metadata_source_graph.h"

struct Objc3ExecutableMetadataSemanticConsistencyBoundary {
  std::string contract_id =
      kObjc3ExecutableMetadataSemanticConsistencyContractId;
  std::string executable_metadata_source_graph_contract_id;
  bool semantic_boundary_frozen = false;
  bool lowering_admission_ready = false;
  bool fail_closed = false;
  bool source_graph_ready = false;
  bool protocol_category_handoff_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool selector_normalization_deterministic = false;
  bool property_attribute_deterministic = false;
  bool symbol_graph_scope_resolution_deterministic = false;
  bool protocol_inheritance_edges_complete = false;
  bool category_attachment_edges_complete = false;
  bool declaration_export_owner_split_complete = false;
  bool property_method_ivar_owner_edges_complete = false;
  bool semantic_conflict_diagnostics_enforcement_pending = true;
  bool duplicate_export_owner_enforcement_pending = true;
  bool lowering_admission_pending = true;
  std::size_t protocol_node_count = 0;
  std::size_t category_node_count = 0;
  std::size_t property_node_count = 0;
  std::size_t method_node_count = 0;
  std::size_t ivar_node_count = 0;
  std::size_t owner_edge_count = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary) {
  return !boundary.contract_id.empty() &&
         !boundary.executable_metadata_source_graph_contract_id.empty() &&
         boundary.semantic_boundary_frozen &&
         !boundary.lowering_admission_ready &&
         boundary.fail_closed &&
         boundary.source_graph_ready &&
         boundary.protocol_category_handoff_deterministic &&
         boundary.class_protocol_category_linking_deterministic &&
         boundary.selector_normalization_deterministic &&
         boundary.property_attribute_deterministic &&
         boundary.symbol_graph_scope_resolution_deterministic &&
         boundary.protocol_inheritance_edges_complete &&
         boundary.category_attachment_edges_complete &&
         boundary.declaration_export_owner_split_complete &&
         boundary.property_method_ivar_owner_edges_complete &&
         boundary.semantic_conflict_diagnostics_enforcement_pending &&
         boundary.duplicate_export_owner_enforcement_pending &&
         boundary.lowering_admission_pending &&
         boundary.failure_reason.empty();
}

struct Objc3ExecutableMetadataSemanticValidationSurface {
  std::string contract_id =
      kObjc3ExecutableMetadataSemanticValidationContractId;
  std::string executable_metadata_semantic_consistency_contract_id;
  bool semantic_consistency_ready = false;
  bool method_lookup_override_conflict_handoff_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool class_inheritance_edges_complete = false;
  bool protocol_inheritance_edges_complete = false;
  bool metaclass_edges_complete = false;
  bool inheritance_chain_cycle_free = false;
  bool superclass_targets_resolved = false;
  bool protocol_inheritance_targets_resolved = false;
  bool metaclass_targets_resolved = false;
  bool metaclass_lineage_aligned = false;
  bool method_override_edges_complete = false;
  bool override_lookup_complete = false;
  bool override_conflicts_absent = false;
  bool protocol_composition_valid = false;
  bool inheritance_validation_ready = false;
  bool override_validation_ready = false;
  bool protocol_composition_validation_ready = false;
  bool metaclass_relationship_validation_ready = false;
  bool semantic_validation_complete = false;
  bool lowering_admission_ready = false;
  bool fail_closed = false;
  std::size_t class_inheritance_edge_count = 0;
  std::size_t protocol_inheritance_edge_count = 0;
  std::size_t metaclass_super_edge_count = 0;
  std::size_t override_edge_count = 0;
  std::size_t class_method_override_edge_count = 0;
  std::size_t instance_method_override_edge_count = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataSemanticValidationSurface(
    const Objc3ExecutableMetadataSemanticValidationSurface &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         surface.semantic_consistency_ready &&
         surface.method_lookup_override_conflict_handoff_deterministic &&
         surface.class_protocol_category_linking_deterministic &&
         surface.class_inheritance_edges_complete &&
         surface.protocol_inheritance_edges_complete &&
         surface.metaclass_edges_complete &&
         surface.inheritance_chain_cycle_free &&
         surface.superclass_targets_resolved &&
         surface.protocol_inheritance_targets_resolved &&
         surface.metaclass_targets_resolved &&
         surface.metaclass_lineage_aligned &&
         surface.method_override_edges_complete &&
         surface.override_lookup_complete &&
         surface.override_conflicts_absent &&
         surface.protocol_composition_valid &&
         surface.inheritance_validation_ready &&
         surface.override_validation_ready &&
         surface.protocol_composition_validation_ready &&
         surface.metaclass_relationship_validation_ready &&
         surface.semantic_validation_complete &&
         !surface.lowering_admission_ready &&
         surface.fail_closed &&
         surface.failure_reason.empty();
}

struct Objc3ExecutableMetadataLoweringHandoffSurface {
  std::string contract_id = kObjc3ExecutableMetadataLoweringHandoffContractId;
  std::string executable_metadata_source_graph_contract_id;
  std::string executable_metadata_semantic_consistency_contract_id;
  std::string executable_metadata_semantic_validation_contract_id;
  bool source_graph_ready = false;
  bool semantic_consistency_ready = false;
  bool semantic_validation_ready = false;
  bool semantic_type_metadata_handoff_deterministic = false;
  bool protocol_category_handoff_deterministic = false;
  bool class_protocol_category_linking_handoff_deterministic = false;
  bool selector_normalization_handoff_deterministic = false;
  bool property_attribute_handoff_deterministic = false;
  bool symbol_graph_scope_resolution_handoff_deterministic = false;
  bool property_synthesis_ivar_binding_handoff_deterministic = false;
  bool lowering_schema_frozen = false;
  bool fail_closed = false;
  bool ready_for_lowering = false;
  std::size_t interface_node_count = 0;
  std::size_t implementation_node_count = 0;
  std::size_t class_node_count = 0;
  std::size_t metaclass_node_count = 0;
  std::size_t protocol_node_count = 0;
  std::size_t category_node_count = 0;
  std::size_t property_node_count = 0;
  std::size_t method_node_count = 0;
  std::size_t ivar_node_count = 0;
  std::size_t owner_edge_count = 0;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_source_graph_contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         !surface.executable_metadata_semantic_validation_contract_id.empty() &&
         surface.source_graph_ready &&
         surface.semantic_consistency_ready &&
         surface.semantic_validation_ready &&
         surface.semantic_type_metadata_handoff_deterministic &&
         surface.protocol_category_handoff_deterministic &&
         surface.class_protocol_category_linking_handoff_deterministic &&
         surface.selector_normalization_handoff_deterministic &&
         surface.property_attribute_handoff_deterministic &&
         surface.symbol_graph_scope_resolution_handoff_deterministic &&
         surface.property_synthesis_ivar_binding_handoff_deterministic &&
         surface.lowering_schema_frozen &&
         surface.fail_closed &&
         !surface.ready_for_lowering &&
         !surface.replay_key.empty() &&
         surface.failure_reason.empty();
}

struct Objc3ExecutableMetadataTypedLoweringHandoff {
  std::string contract_id = kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string executable_metadata_lowering_handoff_contract_id;
  std::string executable_metadata_source_graph_contract_id;
  std::string executable_metadata_semantic_consistency_contract_id;
  std::string executable_metadata_semantic_validation_contract_id;
  std::string manifest_schema_ordering_model =
      kObjc3ExecutableMetadataTypedLoweringManifestSchemaOrderingModel;
  bool source_graph_ready = false;
  bool semantic_consistency_ready = false;
  bool semantic_validation_ready = false;
  bool lowering_handoff_surface_ready = false;
  bool deterministic = false;
  bool manifest_schema_frozen = false;
  bool fail_closed = false;
  bool ready_for_lowering = false;
  Objc3ExecutableMetadataSourceGraph source_graph;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_lowering_handoff_contract_id.empty() &&
         !surface.executable_metadata_source_graph_contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         !surface.executable_metadata_semantic_validation_contract_id.empty() &&
         !surface.manifest_schema_ordering_model.empty() &&
         surface.source_graph_ready &&
         surface.semantic_consistency_ready &&
         surface.semantic_validation_ready &&
         surface.lowering_handoff_surface_ready &&
         surface.deterministic &&
         surface.manifest_schema_frozen &&
         surface.fail_closed &&
         surface.ready_for_lowering &&
         !surface.replay_key.empty() &&
         surface.failure_reason.empty();
}
