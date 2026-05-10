#include "pipeline/frontend_executable_metadata_semantic_surface_helpers_owners.h"

#include <algorithm>
#include <cstddef>
#include <string>

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataSemanticValidationBaseState(
    Objc3ExecutableMetadataSemanticValidationSurface &surface,
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary) {
  surface.executable_metadata_semantic_consistency_contract_id =
      semantic_consistency_boundary.contract_id;
  surface.semantic_consistency_ready =
      IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
          semantic_consistency_boundary);

  const Objc3MethodLookupOverrideConflictSummary
      &method_lookup_override_conflict_summary =
          sema_type_metadata_handoff.method_lookup_override_conflict_summary;
  surface.method_lookup_override_conflict_handoff_deterministic =
      method_lookup_override_conflict_summary.deterministic;
  surface.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;

  surface.override_lookup_sites =
      method_lookup_override_conflict_summary.override_lookup_sites;
  surface.override_lookup_hits =
      method_lookup_override_conflict_summary.override_lookup_hits;
  surface.override_lookup_misses =
      method_lookup_override_conflict_summary.override_lookup_misses;
  surface.override_conflicts =
      method_lookup_override_conflict_summary.override_conflicts;
  surface.unresolved_base_interfaces =
      method_lookup_override_conflict_summary.unresolved_base_interfaces;
  surface.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  surface.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  surface.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  surface.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  surface.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;

  const auto count_edges = [&](const std::string &edge_kind) {
    return static_cast<std::size_t>(std::count_if(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind;
        }));
  };

  surface.class_inheritance_edge_count = count_edges("class-to-superclass");
  surface.protocol_inheritance_edge_count =
      count_edges("protocol-to-inherited-protocol");
  surface.metaclass_super_edge_count =
      count_edges("metaclass-to-super-metaclass");
  surface.override_edge_count = count_edges("method-to-overridden-method");
}

}  // namespace objc3c::pipeline::orchestration
