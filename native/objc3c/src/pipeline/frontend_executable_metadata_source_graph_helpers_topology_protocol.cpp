#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include <algorithm>
#include <string>

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataProtocolTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph,
    ExecutableMetadataSourceGraphTopologyContext &context) {
  for (const auto &protocol_decl : program.protocols) {
    Objc3ExecutableMetadataProtocolGraphNode node;
    node.protocol_name = protocol_decl.name;
    node.owner_identity = protocol_decl.semantic_link_symbol;
    node.inherited_protocol_owner_identities_lexicographic =
        protocol_decl.inherited_protocols_lexicographic;
    node.property_count = protocol_decl.properties.size();
    node.method_count = protocol_decl.methods.size();
    node.is_forward_declaration = protocol_decl.is_forward_declaration;
    node.declaration_complete =
        !node.protocol_name.empty() && !node.owner_identity.empty();
    node.line = protocol_decl.line;
    node.column = protocol_decl.column;
    std::sort(node.inherited_protocol_owner_identities_lexicographic.begin(),
              node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_owner_identities_lexicographic.erase(
        std::unique(
            node.inherited_protocol_owner_identities_lexicographic.begin(),
            node.inherited_protocol_owner_identities_lexicographic.end()),
        node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_identity_complete =
        std::all_of(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.protocol_nodes_lexicographic.push_back(node);

    for (const auto &target :
         node.inherited_protocol_owner_identities_lexicographic) {
      AddExecutableMetadataOwnerEdge(
          graph, "protocol-to-inherited-protocol", node.owner_identity, target,
          node.line, node.column);
    }

    AddExecutableMetadataPropertyNodes(
        protocol_decl.properties, "protocol", protocol_decl.name,
        protocol_decl.semantic_link_symbol, protocol_decl.semantic_link_symbol,
        context.property_synthesis_index, graph);
    AddExecutableMetadataMethodNodes(
        protocol_decl.methods, "protocol", protocol_decl.name,
        protocol_decl.semantic_link_symbol, protocol_decl.semantic_link_symbol,
        protocol_decl.semantic_link_symbol, false, graph,
        context.method_edge_records);
  }
}

}  // namespace objc3c::pipeline::orchestration
