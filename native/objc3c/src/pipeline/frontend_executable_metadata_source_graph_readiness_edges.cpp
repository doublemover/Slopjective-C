#include "pipeline/frontend_executable_metadata_source_graph_readiness.h"

#include <algorithm>
#include <string>

namespace objc3c::pipeline::orchestration {

bool HasExecutableMetadataGraphEdge(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const std::string &edge_kind,
    const std::string &source_owner_identity,
    const std::string &target_owner_identity) {
  return std::any_of(
      graph.owner_edges_lexicographic.begin(),
      graph.owner_edges_lexicographic.end(),
      [&edge_kind, &source_owner_identity, &target_owner_identity](
          const Objc3ExecutableMetadataGraphEdge &edge) {
        return edge.edge_kind == edge_kind &&
               edge.source_owner_identity == source_owner_identity &&
               edge.target_owner_identity == target_owner_identity;
      });
}

}  // namespace objc3c::pipeline::orchestration
