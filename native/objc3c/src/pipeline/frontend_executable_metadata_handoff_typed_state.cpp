#include "pipeline/frontend_executable_metadata_handoff_owners.h"

#include "runtime/metadata/class_metadata.h"

namespace objc3_frontend_executable_metadata_handoff {

void PopulateExecutableMetadataTypedLoweringHandoffState(
    Objc3ExecutableMetadataTypedLoweringHandoff &surface,
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &semantic_validation_surface,
    const Objc3ExecutableMetadataLoweringHandoffSurface
        &lowering_handoff_surface) {
  surface.executable_metadata_lowering_handoff_contract_id =
      lowering_handoff_surface.contract_id;
  surface.executable_metadata_source_graph_contract_id = graph.contract_id;
  surface.executable_metadata_semantic_consistency_contract_id =
      semantic_consistency_boundary.contract_id;
  surface.executable_metadata_semantic_validation_contract_id =
      semantic_validation_surface.contract_id;
  surface.source_graph_ready = IsReadyObjc3ExecutableMetadataSourceGraph(graph);
  surface.semantic_consistency_ready =
      IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
          semantic_consistency_boundary);
  surface.semantic_validation_ready =
      IsReadyObjc3ExecutableMetadataSemanticValidationSurface(
          semantic_validation_surface);
  surface.lowering_handoff_surface_ready =
      IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(
          lowering_handoff_surface);
  surface.source_graph = graph;
}

}  // namespace objc3_frontend_executable_metadata_handoff
