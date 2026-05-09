#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

inline Objc3ExecutableMetadataSemanticConsistencyBoundary
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

inline Objc3ExecutableMetadataSemanticValidationSurface
BuildExecutableMetadataSemanticValidationSurface(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary) {
  Objc3ExecutableMetadataSemanticValidationSurface surface;
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
  const auto has_edge = [&](const std::string &edge_kind,
                            const std::string &source_owner_identity,
                            const std::string &target_owner_identity) {
    return std::any_of(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind &&
                 edge.source_owner_identity == source_owner_identity &&
                 edge.target_owner_identity == target_owner_identity;
        });
  };

  surface.class_inheritance_edge_count = count_edges("class-to-superclass");
  surface.protocol_inheritance_edge_count =
      count_edges("protocol-to-inherited-protocol");
  surface.metaclass_super_edge_count =
      count_edges("metaclass-to-super-metaclass");
  surface.override_edge_count = count_edges("method-to-overridden-method");

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataMetaclassGraphNode *>
      metaclass_nodes_by_owner_identity;
  metaclass_nodes_by_owner_identity.reserve(
      graph.metaclass_nodes_lexicographic.size());
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    metaclass_nodes_by_owner_identity.emplace(metaclass_node.owner_identity,
                                              &metaclass_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataProtocolGraphNode *>
      protocol_nodes_by_owner_identity;
  protocol_nodes_by_owner_identity.reserve(
      graph.protocol_nodes_lexicographic.size());
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    protocol_nodes_by_owner_identity.emplace(protocol_node.owner_identity,
                                             &protocol_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataMethodGraphNode *>
      class_interface_methods_by_key;
  class_interface_methods_by_key.reserve(graph.method_nodes_lexicographic.size());
  const auto build_interface_method_key =
      [](const std::string &declaration_owner_identity,
         const std::string &selector, bool is_class_method) {
        return declaration_owner_identity + "::" +
               (is_class_method ? "class" : "instance") + "::" + selector;
      };
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    class_interface_methods_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        &method_node);
  }

  surface.class_inheritance_edges_complete = true;
  surface.inheritance_chain_cycle_free = true;
  surface.superclass_targets_resolved = true;
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    if (!class_node.has_super) {
      continue;
    }
    if (!has_edge("class-to-superclass", class_node.owner_identity,
                  class_node.super_class_owner_identity)) {
      surface.class_inheritance_edges_complete = false;
    }
    std::string next_super_owner_identity = class_node.super_class_owner_identity;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        surface.inheritance_chain_cycle_free = false;
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end()) {
        surface.superclass_targets_resolved = false;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
  }

  surface.protocol_inheritance_edges_complete = true;
  surface.protocol_inheritance_targets_resolved = true;
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    for (const auto &target_owner_identity :
         protocol_node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_edge("protocol-to-inherited-protocol",
                    protocol_node.owner_identity, target_owner_identity)) {
        surface.protocol_inheritance_edges_complete = false;
      }
      if (protocol_nodes_by_owner_identity.find(target_owner_identity) ==
          protocol_nodes_by_owner_identity.end()) {
        surface.protocol_inheritance_targets_resolved = false;
      }
    }
  }

  surface.metaclass_edges_complete = true;
  surface.metaclass_targets_resolved = true;
  surface.metaclass_lineage_aligned = true;
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    const auto class_it =
        class_nodes_by_owner_identity.find(metaclass_node.class_owner_identity);
    if (class_it == class_nodes_by_owner_identity.end()) {
      surface.metaclass_targets_resolved = false;
      surface.metaclass_lineage_aligned = false;
      continue;
    }
    if (!has_edge("class-to-metaclass", metaclass_node.class_owner_identity,
                  metaclass_node.owner_identity)) {
      surface.metaclass_edges_complete = false;
    }
    if (class_it->second->metaclass_owner_identity !=
        metaclass_node.owner_identity) {
      surface.metaclass_lineage_aligned = false;
    }
    if (metaclass_node.has_super) {
      if (!has_edge("metaclass-to-super-metaclass", metaclass_node.owner_identity,
                    metaclass_node.super_metaclass_owner_identity)) {
        surface.metaclass_edges_complete = false;
      }
      const auto super_metaclass_it =
          metaclass_nodes_by_owner_identity.find(
              metaclass_node.super_metaclass_owner_identity);
      if (super_metaclass_it == metaclass_nodes_by_owner_identity.end()) {
        surface.metaclass_targets_resolved = false;
        surface.metaclass_lineage_aligned = false;
      } else if (class_it->second->has_super &&
                 super_metaclass_it->second->class_owner_identity !=
                     class_it->second->super_class_owner_identity) {
        surface.metaclass_lineage_aligned = false;
      }
    } else if (!metaclass_node.super_metaclass_owner_identity.empty()) {
      surface.metaclass_lineage_aligned = false;
    }
  }

  surface.method_override_edges_complete = true;
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    const auto interface_it = interface_nodes_by_owner_identity.find(
        method_node.declaration_owner_identity);
    if (interface_it == interface_nodes_by_owner_identity.end() ||
        !interface_it->second->has_super) {
      continue;
    }

    std::string next_super_owner_identity =
        interface_it->second->super_class_owner_identity;
    const Objc3ExecutableMetadataMethodGraphNode *overridden_method = nullptr;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end() ||
          class_it->second->interface_owner_identity.empty()) {
        break;
      }
      const auto base_method_it = class_interface_methods_by_key.find(
          build_interface_method_key(class_it->second->interface_owner_identity,
                                     method_node.selector,
                                     method_node.is_class_method));
      if (base_method_it != class_interface_methods_by_key.end()) {
        overridden_method = base_method_it->second;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
    if (overridden_method == nullptr) {
      continue;
    }
    if (!has_edge("method-to-overridden-method", method_node.owner_identity,
                  overridden_method->owner_identity)) {
      surface.method_override_edges_complete = false;
    } else if (method_node.is_class_method) {
      ++surface.class_method_override_edge_count;
    } else {
      ++surface.instance_method_override_edge_count;
    }
  }

  surface.override_lookup_complete =
      method_lookup_override_conflict_summary.unresolved_base_interfaces == 0u;
  surface.override_conflicts_absent =
      method_lookup_override_conflict_summary.override_conflicts == 0u;
  surface.protocol_composition_valid =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          0u &&
      class_protocol_category_linking_summary.protocol_composition_symbols >=
          class_protocol_category_linking_summary
              .category_composition_symbols &&
      class_protocol_category_linking_summary.protocol_composition_sites >=
          class_protocol_category_linking_summary.category_composition_sites;

  surface.inheritance_validation_ready =
      surface.class_inheritance_edges_complete &&
      surface.protocol_inheritance_edges_complete &&
      surface.inheritance_chain_cycle_free &&
      surface.superclass_targets_resolved &&
      surface.protocol_inheritance_targets_resolved;
  surface.override_validation_ready =
      surface.method_lookup_override_conflict_handoff_deterministic &&
      surface.method_override_edges_complete &&
      surface.override_lookup_complete &&
      surface.override_conflicts_absent;
  surface.protocol_composition_validation_ready =
      surface.class_protocol_category_linking_deterministic &&
      surface.protocol_composition_valid;
  surface.metaclass_relationship_validation_ready =
      surface.metaclass_edges_complete &&
      surface.metaclass_targets_resolved &&
      surface.metaclass_lineage_aligned;

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic validation contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic consistency dependency contract id is empty";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "executable metadata semantic consistency boundary is not ready";
  } else if (!surface.method_lookup_override_conflict_handoff_deterministic) {
    surface.failure_reason =
        "method lookup/override conflict handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_deterministic) {
    surface.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!surface.inheritance_validation_ready) {
    surface.failure_reason =
        "inheritance validation is incomplete";
  } else if (!surface.override_validation_ready) {
    surface.failure_reason = "override validation is incomplete";
  } else if (!surface.protocol_composition_validation_ready) {
    surface.failure_reason =
        "protocol composition validation is incomplete";
  } else if (!surface.metaclass_relationship_validation_ready) {
    surface.failure_reason =
        "metaclass relationship validation is incomplete";
  }

  surface.semantic_validation_complete = surface.failure_reason.empty();
  surface.lowering_admission_ready = false;
  surface.fail_closed = surface.semantic_validation_complete &&
                        !surface.lowering_admission_ready;
  if (surface.failure_reason.empty() && !surface.fail_closed) {
    surface.failure_reason =
        "executable metadata semantic validation surface is not fail-closed";
  }
  return surface;
}

}  // namespace objc3c::pipeline::orchestration
