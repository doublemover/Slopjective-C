#include "pipeline/frontend_executable_metadata_handoff_replay_owners.h"

namespace objc3_frontend_executable_metadata_handoff_replay {

void AppendExecutableMetadataTypedLoweringHandoffReplayHeader(
    std::ostringstream &out,
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  out << "executable-metadata-typed-lowering-handoff:v1"
      << ";graph_contract=" << surface.executable_metadata_source_graph_contract_id
      << ";protocol_category_source_closure_contract="
      << surface.source_graph.protocol_category_source_closure_contract_id
      << ";protocol_inheritance_identity_model="
      << surface.source_graph.protocol_inheritance_identity_model
      << ";category_attachment_identity_model="
      << surface.source_graph.category_attachment_identity_model
      << ";protocol_category_conformance_identity_model="
      << surface.source_graph.protocol_category_conformance_identity_model
      << ";semantic_consistency_contract="
      << surface.executable_metadata_semantic_consistency_contract_id
      << ";semantic_validation_contract="
      << surface.executable_metadata_semantic_validation_contract_id
      << ";lowering_handoff_contract="
      << surface.executable_metadata_lowering_handoff_contract_id
      << ";schema_ordering_model=" << surface.manifest_schema_ordering_model
      << ";deterministic=" << (surface.deterministic ? "true" : "false")
      << ";ready_for_lowering="
      << (surface.ready_for_lowering ? "true" : "false");
}

}  // namespace objc3_frontend_executable_metadata_handoff_replay
