#include "pipeline/frontend_executable_metadata_handoff_replay_owners.h"

namespace objc3_frontend_executable_metadata_handoff_replay {

void AppendExecutableMetadataTypedEdgeReplayRecords(
    std::ostringstream &out,
    const Objc3ExecutableMetadataSourceGraph &graph) {
  for (const auto &edge : graph.owner_edges_lexicographic) {
    out << ";edge=" << edge.edge_kind << "|" << edge.source_owner_identity << "|"
        << edge.target_owner_identity << "|" << edge.line << "|" << edge.column;
  }
}

}  // namespace objc3_frontend_executable_metadata_handoff_replay
