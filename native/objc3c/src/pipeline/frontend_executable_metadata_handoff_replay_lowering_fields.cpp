#include "pipeline/frontend_executable_metadata_handoff_replay_owners.h"

namespace objc3_frontend_executable_metadata_handoff_replay {

void AppendExecutableMetadataLoweringHandoffReplayFields(
    std::ostringstream &out,
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  out << "executable-metadata-lowering-handoff:v1"
      << ";source_graph_ready=" << (surface.source_graph_ready ? "true" : "false")
      << ";semantic_consistency_ready="
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ";semantic_validation_ready="
      << (surface.semantic_validation_ready ? "true" : "false")
      << ";semantic_type_metadata_handoff_deterministic="
      << (surface.semantic_type_metadata_handoff_deterministic ? "true"
                                                               : "false")
      << ";protocol_category_handoff_deterministic="
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ";class_protocol_category_linking_handoff_deterministic="
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true"
                                                                        : "false")
      << ";selector_normalization_handoff_deterministic="
      << (surface.selector_normalization_handoff_deterministic ? "true"
                                                               : "false")
      << ";property_attribute_handoff_deterministic="
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ";symbol_graph_scope_resolution_handoff_deterministic="
      << (surface.symbol_graph_scope_resolution_handoff_deterministic ? "true"
                                                                      : "false")
      << ";property_synthesis_ivar_binding_handoff_deterministic="
      << (surface.property_synthesis_ivar_binding_handoff_deterministic ? "true"
                                                                        : "false")
      << ";interface_node_count=" << surface.interface_node_count
      << ";implementation_node_count=" << surface.implementation_node_count
      << ";class_node_count=" << surface.class_node_count
      << ";metaclass_node_count=" << surface.metaclass_node_count
      << ";protocol_node_count=" << surface.protocol_node_count
      << ";category_node_count=" << surface.category_node_count
      << ";property_node_count=" << surface.property_node_count
      << ";method_node_count=" << surface.method_node_count
      << ";ivar_node_count=" << surface.ivar_node_count
      << ";owner_edge_count=" << surface.owner_edge_count
      << ";ready_for_lowering=" << (surface.ready_for_lowering ? "true"
                                                               : "false");
}

}  // namespace objc3_frontend_executable_metadata_handoff_replay
