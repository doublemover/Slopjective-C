#include "pipeline/frontend_executable_metadata_handoff_owners.h"

#include "pipeline/frontend_executable_metadata_handoff_replay.h"

namespace objc3_frontend_executable_metadata_handoff {

void PublishExecutableMetadataTypedLoweringHandoffReadiness(
    Objc3ExecutableMetadataTypedLoweringHandoff &surface,
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataLoweringHandoffSurface
        &lowering_handoff_surface) {
  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff contract id is empty";
  } else if (surface.executable_metadata_lowering_handoff_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff contract id is empty";
  } else if (surface.executable_metadata_source_graph_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff source graph contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff semantic consistency contract id is empty";
  } else if (surface.executable_metadata_semantic_validation_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff semantic validation contract id is empty";
  } else if (surface.manifest_schema_ordering_model.empty()) {
    surface.failure_reason =
        "typed lowering handoff manifest schema ordering model is empty";
  } else if (!surface.source_graph_ready) {
    surface.failure_reason =
        "typed lowering handoff source graph is not ready";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "typed lowering handoff semantic consistency boundary is not ready";
  } else if (!surface.semantic_validation_ready) {
    surface.failure_reason =
        "typed lowering handoff semantic validation surface is not ready";
  } else if (!surface.lowering_handoff_surface_ready) {
    surface.failure_reason =
        "typed lowering handoff freeze surface is not ready";
  } else if (graph.interface_nodes_lexicographic.size() !=
             lowering_handoff_surface.interface_node_count) {
    surface.failure_reason =
        "typed lowering handoff interface node count drifted from the freeze surface";
  } else if (graph.implementation_nodes_lexicographic.size() !=
             lowering_handoff_surface.implementation_node_count) {
    surface.failure_reason =
        "typed lowering handoff implementation node count drifted from the freeze surface";
  } else if (graph.class_nodes_lexicographic.size() !=
             lowering_handoff_surface.class_node_count) {
    surface.failure_reason =
        "typed lowering handoff class node count drifted from the freeze surface";
  } else if (graph.metaclass_nodes_lexicographic.size() !=
             lowering_handoff_surface.metaclass_node_count) {
    surface.failure_reason =
        "typed lowering handoff metaclass node count drifted from the freeze surface";
  } else if (graph.protocol_nodes_lexicographic.size() !=
             lowering_handoff_surface.protocol_node_count) {
    surface.failure_reason =
        "typed lowering handoff protocol node count drifted from the freeze surface";
  } else if (graph.category_nodes_lexicographic.size() !=
             lowering_handoff_surface.category_node_count) {
    surface.failure_reason =
        "typed lowering handoff category node count drifted from the freeze surface";
  } else if (graph.property_nodes_lexicographic.size() !=
             lowering_handoff_surface.property_node_count) {
    surface.failure_reason =
        "typed lowering handoff property node count drifted from the freeze surface";
  } else if (graph.method_nodes_lexicographic.size() !=
             lowering_handoff_surface.method_node_count) {
    surface.failure_reason =
        "typed lowering handoff method node count drifted from the freeze surface";
  } else if (graph.ivar_nodes_lexicographic.size() !=
             lowering_handoff_surface.ivar_node_count) {
    surface.failure_reason =
        "typed lowering handoff ivar node count drifted from the freeze surface";
  } else if (graph.owner_edges_lexicographic.size() !=
             lowering_handoff_surface.owner_edge_count) {
    surface.failure_reason =
        "typed lowering handoff owner edge count drifted from the freeze surface";
  }

  surface.deterministic =
      surface.failure_reason.empty() && graph.deterministic &&
      !lowering_handoff_surface.replay_key.empty();
  surface.manifest_schema_frozen = surface.failure_reason.empty();
  surface.fail_closed = surface.manifest_schema_frozen;
  surface.ready_for_lowering =
      surface.manifest_schema_frozen && surface.deterministic &&
      surface.fail_closed;
  if (surface.failure_reason.empty()) {
    surface.replay_key =
        BuildExecutableMetadataTypedLoweringHandoffReplayKey(surface);
  }
  if (!surface.failure_reason.empty() || !surface.ready_for_lowering) {
    if (surface.failure_reason.empty()) {
      surface.failure_reason =
          "typed lowering handoff did not become lowering-ready";
    }
    surface.replay_key.clear();
  }
}

}  // namespace objc3_frontend_executable_metadata_handoff
