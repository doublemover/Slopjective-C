#include "pipeline/frontend_executable_metadata_semantic_surface_helpers.h"

#include <algorithm>
#include <cstddef>
#include <string>

namespace objc3c::pipeline::orchestration {

Objc3ExecutableMetadataSemanticConsistencyBoundary
BuildExecutableMetadataSemanticConsistencyBoundary(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary) {
  Objc3ExecutableMetadataSemanticConsistencyBoundary boundary;
  boundary.executable_metadata_source_graph_contract_id = graph.contract_id;
  boundary.source_graph_ready = IsReadyObjc3ExecutableMetadataSourceGraph(graph);
  boundary.protocol_category_handoff_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  boundary.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  boundary.selector_normalization_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  boundary.property_attribute_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  boundary.symbol_graph_scope_resolution_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary
          .deterministic_scope_resolution_handoff;
  boundary.protocol_node_count = graph.protocol_nodes_lexicographic.size();
  boundary.category_node_count = graph.category_nodes_lexicographic.size();
  boundary.property_node_count = graph.property_nodes_lexicographic.size();
  boundary.method_node_count = graph.method_nodes_lexicographic.size();
  boundary.ivar_node_count = graph.ivar_nodes_lexicographic.size();
  boundary.owner_edge_count = graph.owner_edges_lexicographic.size();

  const auto has_graph_edge =
      [&graph](const std::string &edge_kind, const std::string &source,
               const std::string &target) {
        return std::any_of(
            graph.owner_edges_lexicographic.begin(),
            graph.owner_edges_lexicographic.end(),
            [&](const Objc3ExecutableMetadataGraphEdge &edge) {
              return edge.edge_kind == edge_kind &&
                     edge.source_owner_identity == source &&
                     edge.target_owner_identity == target;
            });
      };

  boundary.protocol_inheritance_edges_complete = true;
  for (const auto &node : graph.protocol_nodes_lexicographic) {
    for (const auto &target :
         node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_graph_edge("protocol-to-inherited-protocol", node.owner_identity,
                          target)) {
        boundary.protocol_inheritance_edges_complete = false;
      }
    }
  }

  boundary.category_attachment_edges_complete = true;
  for (const auto &node : graph.category_nodes_lexicographic) {
    if (!has_graph_edge("category-to-class", node.owner_identity,
                        node.class_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    if (node.has_interface &&
        !has_graph_edge("category-to-interface", node.owner_identity,
                        node.interface_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    if (node.has_implementation &&
        !has_graph_edge("category-to-implementation", node.owner_identity,
                        node.implementation_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    for (const auto &target :
         node.adopted_protocol_owner_identities_lexicographic) {
      if (!has_graph_edge("category-to-protocol", node.owner_identity,
                          target)) {
        boundary.category_attachment_edges_complete = false;
      }
    }
  }

  boundary.declaration_export_owner_split_complete = true;
  for (const auto &node : graph.property_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.property_name.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }
  for (const auto &node : graph.method_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.selector.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }
  for (const auto &node : graph.ivar_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() ||
        node.property_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.ivar_binding_symbol.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }

  boundary.property_method_ivar_owner_edges_complete = true;
  for (const auto &node : graph.property_nodes_lexicographic) {
    if (!has_graph_edge("property-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("property-to-export-owner", node.owner_identity,
                        node.export_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
    if (!node.ivar_binding_symbol.empty()) {
      const auto ivar_it = std::find_if(
          graph.ivar_nodes_lexicographic.begin(),
          graph.ivar_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataIvarGraphNode &ivar_node) {
            return ivar_node.property_owner_identity == node.owner_identity &&
                   ivar_node.ivar_binding_symbol == node.ivar_binding_symbol;
          });
      if (ivar_it == graph.ivar_nodes_lexicographic.end() ||
          !has_graph_edge("property-to-ivar", node.owner_identity,
                          ivar_it->owner_identity)) {
        boundary.property_method_ivar_owner_edges_complete = false;
      }
    }
    if (node.has_getter) {
      const bool getter_exists = std::any_of(
          graph.method_nodes_lexicographic.begin(),
          graph.method_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
            return method_node.export_owner_identity ==
                       node.export_owner_identity &&
                   !method_node.is_class_method &&
                   method_node.selector == node.getter_selector;
          });
      if (getter_exists) {
        const auto getter_it = std::find_if(
            graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
              return method_node.export_owner_identity ==
                         node.export_owner_identity &&
                     !method_node.is_class_method &&
                     method_node.selector == node.getter_selector &&
                     has_graph_edge("property-to-getter-method",
                                    node.owner_identity,
                                    method_node.owner_identity);
            });
        if (getter_it == graph.method_nodes_lexicographic.end()) {
          boundary.property_method_ivar_owner_edges_complete = false;
        }
      }
    }
    if (node.has_setter) {
      const bool setter_exists = std::any_of(
          graph.method_nodes_lexicographic.begin(),
          graph.method_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
            return method_node.export_owner_identity ==
                       node.export_owner_identity &&
                   !method_node.is_class_method &&
                   method_node.selector == node.setter_selector;
          });
      if (setter_exists) {
        const auto setter_it = std::find_if(
            graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
              return method_node.export_owner_identity ==
                         node.export_owner_identity &&
                     !method_node.is_class_method &&
                     method_node.selector == node.setter_selector &&
                     has_graph_edge("property-to-setter-method",
                                    node.owner_identity,
                                    method_node.owner_identity);
            });
        if (setter_it == graph.method_nodes_lexicographic.end()) {
          boundary.property_method_ivar_owner_edges_complete = false;
        }
      }
    }
  }

  for (const auto &node : graph.method_nodes_lexicographic) {
    if (!has_graph_edge("method-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("method-to-export-owner", node.owner_identity,
                        node.export_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
  }

  for (const auto &node : graph.ivar_nodes_lexicographic) {
    if (!has_graph_edge("ivar-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("ivar-to-export-owner", node.owner_identity,
                        node.export_owner_identity) ||
        !has_graph_edge("ivar-to-property", node.owner_identity,
                        node.property_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
  }

  if (boundary.contract_id.empty()) {
    boundary.failure_reason =
        "metadata semantic consistency contract id is empty";
  } else if (boundary.executable_metadata_source_graph_contract_id.empty()) {
    boundary.failure_reason =
        "metadata semantic consistency graph contract id is empty";
  } else if (!boundary.source_graph_ready) {
    boundary.failure_reason =
        "executable metadata source graph is not ready";
  } else if (!boundary.protocol_category_handoff_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason =
        "property attribute handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.protocol_inheritance_edges_complete) {
    boundary.failure_reason =
        "protocol inheritance edges are incomplete";
  } else if (!boundary.category_attachment_edges_complete) {
    boundary.failure_reason =
        "category attachment edges are incomplete";
  } else if (!boundary.declaration_export_owner_split_complete) {
    boundary.failure_reason =
        "declaration/export owner identities are incomplete";
  } else if (!boundary.property_method_ivar_owner_edges_complete) {
    boundary.failure_reason =
        "property/method/ivar owner edges are incomplete";
  }

  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.lowering_admission_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.lowering_admission_ready &&
      boundary.semantic_conflict_diagnostics_enforcement_pending &&
      boundary.duplicate_export_owner_enforcement_pending &&
      boundary.lowering_admission_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "metadata semantic consistency freeze is not fail-closed";
  }
  return boundary;
}

}  // namespace objc3c::pipeline::orchestration
