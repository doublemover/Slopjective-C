#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include <string>
#include <utility>

#include "pipeline/frontend_metadata_handoff_helpers.h"

namespace objc3c::pipeline::orchestration {

void BuildExecutableMetadataPropertySynthesisIndex(
    const Objc3Program &program,
    ExecutableMetadataPropertySynthesisIndex &property_synthesis_index) {
  property_synthesis_index.class_implementation_names.reserve(
      program.implementations.size());
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      property_synthesis_index.class_implementation_names.insert(
          implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      property_synthesis_index.implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }
}

void AddExecutableMetadataOwnerEdge(
    Objc3ExecutableMetadataSourceGraph &graph,
    const std::string &edge_kind,
    const std::string &source_owner_identity,
    const std::string &target_owner_identity,
    unsigned line,
    unsigned column) {
  if (source_owner_identity.empty() || target_owner_identity.empty()) {
    return;
  }
  Objc3ExecutableMetadataGraphEdge edge;
  edge.edge_kind = edge_kind;
  edge.source_owner_identity = source_owner_identity;
  edge.target_owner_identity = target_owner_identity;
  edge.line = line;
  edge.column = column;
  graph.owner_edges_lexicographic.push_back(std::move(edge));
}

}  // namespace objc3c::pipeline::orchestration
