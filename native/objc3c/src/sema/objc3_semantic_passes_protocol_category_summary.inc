static void AccumulateProtocolCompositionSite(bool has_protocol_composition,
                                              const std::vector<std::string> &composition_names_lexicographic,
                                              bool has_invalid_protocol_composition,
                                              bool is_category_context,
                                              Objc3ProtocolCategoryCompositionSummary &summary) {
  // anchor: protocol-composition site accounting stays deterministic
  // so the executable metadata semantic-validation surface can fail closed on
  // invalid composition sites without inventing a second accounting model.
  if (!has_protocol_composition) {
    if (has_invalid_protocol_composition) {
      summary.deterministic = false;
    }
    return;
  }

  ++summary.protocol_composition_sites;
  summary.protocol_composition_symbols += composition_names_lexicographic.size();
  if (is_category_context) {
    ++summary.category_composition_sites;
    summary.category_composition_symbols += composition_names_lexicographic.size();
  }
  if (has_invalid_protocol_composition) {
    ++summary.invalid_protocol_composition_sites;
  }
  if (!IsSortedUniqueStrings(composition_names_lexicographic)) {
    summary.deterministic = false;
  }
}

static void AccumulateProtocolCategoryCompositionFromFunctionInfo(const FunctionInfo &fn,
                                                                  Objc3ProtocolCategoryCompositionSummary &summary) {
  if (fn.param_types.size() != fn.arity ||
      fn.param_has_protocol_composition.size() != fn.arity ||
      fn.param_protocol_composition_lexicographic.size() != fn.arity ||
      fn.param_has_invalid_protocol_composition.size() != fn.arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < fn.arity; ++i) {
    AccumulateProtocolCompositionSite(fn.param_has_protocol_composition[i],
                                      fn.param_protocol_composition_lexicographic[i],
                                      fn.param_has_invalid_protocol_composition[i],
                                      false,
                                      summary);
  }
  AccumulateProtocolCompositionSite(fn.return_has_protocol_composition,
                                    fn.return_protocol_composition_lexicographic,
                                    fn.return_has_invalid_protocol_composition,
                                    false,
                                    summary);
}

static void AccumulateProtocolCategoryCompositionFromMethodInfo(const Objc3MethodInfo &method,
                                                                Objc3ProtocolCategoryCompositionSummary &summary) {
  if (method.param_types.size() != method.arity ||
      method.param_has_protocol_composition.size() != method.arity ||
      method.param_protocol_composition_lexicographic.size() != method.arity ||
      method.param_has_invalid_protocol_composition.size() != method.arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < method.arity; ++i) {
    AccumulateProtocolCompositionSite(method.param_has_protocol_composition[i],
                                      method.param_protocol_composition_lexicographic[i],
                                      method.param_has_invalid_protocol_composition[i],
                                      true,
                                      summary);
  }
  AccumulateProtocolCompositionSite(method.return_has_protocol_composition,
                                    method.return_protocol_composition_lexicographic,
                                    method.return_has_invalid_protocol_composition,
                                    true,
                                    summary);
}

static Objc3ProtocolCategoryCompositionSummary BuildProtocolCategoryCompositionSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ProtocolCategoryCompositionSummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateProtocolCategoryCompositionFromFunctionInfo(entry.second, summary);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateProtocolCategoryCompositionFromMethodInfo(method_entry.second, summary);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateProtocolCategoryCompositionFromMethodInfo(method_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.invalid_protocol_composition_sites <= summary.total_composition_sites() &&
                          summary.category_composition_sites <= summary.protocol_composition_sites &&
                          summary.category_composition_symbols <= summary.protocol_composition_symbols;
  return summary;
}

static Objc3ClassProtocolCategoryLinkingSummary BuildClassProtocolCategoryLinkingSummary(
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3ProtocolCategoryCompositionSummary &protocol_category_composition_summary) {
  Objc3ClassProtocolCategoryLinkingSummary summary;
  summary.declared_interfaces = interface_implementation_summary.declared_interfaces;
  summary.resolved_interfaces = interface_implementation_summary.resolved_interfaces;
  summary.declared_implementations = interface_implementation_summary.declared_implementations;
  summary.resolved_implementations = interface_implementation_summary.resolved_implementations;
  summary.interface_method_symbols = interface_implementation_summary.interface_method_symbols;
  summary.implementation_method_symbols = interface_implementation_summary.implementation_method_symbols;
  summary.linked_implementation_symbols = interface_implementation_summary.linked_implementation_symbols;
  summary.protocol_composition_sites = protocol_category_composition_summary.protocol_composition_sites;
  summary.protocol_composition_symbols = protocol_category_composition_summary.protocol_composition_symbols;
  summary.category_composition_sites = protocol_category_composition_summary.category_composition_sites;
  summary.category_composition_symbols = protocol_category_composition_summary.category_composition_symbols;
  summary.invalid_protocol_composition_sites = protocol_category_composition_summary.invalid_protocol_composition_sites;
  summary.deterministic = interface_implementation_summary.deterministic &&
                          protocol_category_composition_summary.deterministic &&
                          summary.resolved_interfaces <= summary.declared_interfaces &&
                          summary.resolved_implementations <= summary.declared_implementations &&
                          summary.linked_implementation_symbols <= summary.implementation_method_symbols &&
                          summary.linked_implementation_symbols <= summary.interface_method_symbols &&
                          summary.invalid_protocol_composition_sites <= summary.total_composition_sites() &&
                          summary.category_composition_sites <= summary.protocol_composition_sites &&
                          summary.category_composition_symbols <= summary.protocol_composition_symbols;
  return summary;
}
