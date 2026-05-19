static Objc3SymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3SymbolGraphScopeResolutionSummary summary;
  summary.global_symbol_nodes = surface.globals.size();
  summary.function_symbol_nodes = surface.functions.size();
  summary.interface_symbol_nodes = surface.interfaces.size();
  summary.implementation_symbol_nodes = surface.implementations.size();
  summary.top_level_scope_symbols = summary.global_symbol_nodes + summary.function_symbol_nodes +
                                    summary.interface_symbol_nodes + summary.implementation_symbol_nodes;
  summary.scope_frames_total =
      static_cast<std::size_t>(1u) + summary.function_symbol_nodes + summary.interface_symbol_nodes +
      summary.implementation_symbol_nodes;

  for (const auto &entry : surface.interfaces) {
    summary.interface_property_symbol_nodes += entry.second.properties.size();
    summary.interface_method_symbol_nodes += entry.second.methods.size();
  }
  for (const auto &entry : surface.implementations) {
    summary.implementation_property_symbol_nodes += entry.second.properties.size();
    summary.implementation_method_symbol_nodes += entry.second.methods.size();
    if (entry.second.has_matching_interface) {
      ++summary.implementation_interface_resolution_hits;
    }
  }

  summary.nested_scope_symbols = summary.interface_property_symbol_nodes + summary.implementation_property_symbol_nodes +
                                 summary.interface_method_symbol_nodes + summary.implementation_method_symbol_nodes;
  summary.implementation_interface_resolution_sites = summary.implementation_symbol_nodes;
  if (summary.implementation_interface_resolution_hits > summary.implementation_interface_resolution_sites) {
    summary.deterministic = false;
    summary.implementation_interface_resolution_misses = 0;
  } else {
    summary.implementation_interface_resolution_misses =
        summary.implementation_interface_resolution_sites - summary.implementation_interface_resolution_hits;
  }

  summary.method_resolution_sites = summary.implementation_method_symbol_nodes;
  summary.method_resolution_hits = surface.interface_implementation_summary.linked_implementation_symbols;
  if (summary.method_resolution_hits > summary.method_resolution_sites) {
    summary.deterministic = false;
    summary.method_resolution_misses = 0;
  } else {
    summary.method_resolution_misses = summary.method_resolution_sites - summary.method_resolution_hits;
  }

  summary.deterministic = summary.deterministic &&
                          summary.interface_method_symbol_nodes ==
                              surface.interface_implementation_summary.interface_method_symbols &&
                          summary.implementation_method_symbol_nodes ==
                              surface.interface_implementation_summary.implementation_method_symbols &&
                          summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols &&
                          summary.implementation_interface_resolution_hits <=
                              summary.implementation_interface_resolution_sites &&
                          summary.implementation_interface_resolution_hits +
                                  summary.implementation_interface_resolution_misses ==
                              summary.implementation_interface_resolution_sites &&
                          summary.method_resolution_hits <= summary.method_resolution_sites &&
                          summary.method_resolution_hits + summary.method_resolution_misses ==
                              summary.method_resolution_sites &&
                          summary.resolution_hits_total() <= summary.resolution_sites_total() &&
                          summary.resolution_hits_total() + summary.resolution_misses_total() ==
                              summary.resolution_sites_total();
  return summary;
}

static Objc3SymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3SymbolGraphScopeResolutionSummary summary;
  summary.global_symbol_nodes = handoff.global_names_lexicographic.size();
  summary.function_symbol_nodes = handoff.functions_lexicographic.size();
  summary.interface_symbol_nodes = handoff.interfaces_lexicographic.size();
  summary.implementation_symbol_nodes = handoff.implementations_lexicographic.size();
  summary.top_level_scope_symbols = summary.global_symbol_nodes + summary.function_symbol_nodes +
                                    summary.interface_symbol_nodes + summary.implementation_symbol_nodes;
  summary.scope_frames_total =
      static_cast<std::size_t>(1u) + summary.function_symbol_nodes + summary.interface_symbol_nodes +
      summary.implementation_symbol_nodes;

  for (const auto &metadata : handoff.interfaces_lexicographic) {
    summary.interface_property_symbol_nodes += metadata.properties_lexicographic.size();
    summary.interface_method_symbol_nodes += metadata.methods_lexicographic.size();
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    summary.implementation_property_symbol_nodes += metadata.properties_lexicographic.size();
    summary.implementation_method_symbol_nodes += metadata.methods_lexicographic.size();
    if (metadata.has_matching_interface) {
      ++summary.implementation_interface_resolution_hits;
    }
  }

  summary.nested_scope_symbols = summary.interface_property_symbol_nodes + summary.implementation_property_symbol_nodes +
                                 summary.interface_method_symbol_nodes + summary.implementation_method_symbol_nodes;
  summary.implementation_interface_resolution_sites = summary.implementation_symbol_nodes;
  if (summary.implementation_interface_resolution_hits > summary.implementation_interface_resolution_sites) {
    summary.deterministic = false;
    summary.implementation_interface_resolution_misses = 0;
  } else {
    summary.implementation_interface_resolution_misses =
        summary.implementation_interface_resolution_sites - summary.implementation_interface_resolution_hits;
  }

  summary.method_resolution_sites = summary.implementation_method_symbol_nodes;
  summary.method_resolution_hits = handoff.interface_implementation_summary.linked_implementation_symbols;
  if (summary.method_resolution_hits > summary.method_resolution_sites) {
    summary.deterministic = false;
    summary.method_resolution_misses = 0;
  } else {
    summary.method_resolution_misses = summary.method_resolution_sites - summary.method_resolution_hits;
  }

  summary.deterministic = summary.deterministic &&
                          summary.interface_method_symbol_nodes ==
                              handoff.interface_implementation_summary.interface_method_symbols &&
                          summary.implementation_method_symbol_nodes ==
                              handoff.interface_implementation_summary.implementation_method_symbols &&
                          summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols &&
                          summary.implementation_interface_resolution_hits <=
                              summary.implementation_interface_resolution_sites &&
                          summary.implementation_interface_resolution_hits +
                                  summary.implementation_interface_resolution_misses ==
                              summary.implementation_interface_resolution_sites &&
                          summary.method_resolution_hits <= summary.method_resolution_sites &&
                          summary.method_resolution_hits + summary.method_resolution_misses ==
                              summary.method_resolution_sites &&
                          summary.resolution_hits_total() <= summary.resolution_sites_total() &&
                          summary.resolution_hits_total() + summary.resolution_misses_total() ==
                              summary.resolution_sites_total();
  return summary;
}
