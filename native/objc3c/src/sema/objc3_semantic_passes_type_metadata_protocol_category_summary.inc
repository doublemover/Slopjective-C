  handoff.protocol_category_composition_summary = Objc3ProtocolCategoryCompositionSummary{};
  const auto accumulate_function_metadata_composition =
      [&handoff](const Objc3SemanticFunctionTypeMetadata &metadata) {
        if (metadata.param_has_protocol_composition.size() != metadata.arity ||
            metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
            metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
          handoff.protocol_category_composition_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          AccumulateProtocolCompositionSite(metadata.param_has_protocol_composition[i],
                                           metadata.param_protocol_composition_lexicographic[i],
                                           metadata.param_has_invalid_protocol_composition[i],
                                           false,
                                           handoff.protocol_category_composition_summary);
        }
        AccumulateProtocolCompositionSite(metadata.return_has_protocol_composition,
                                         metadata.return_protocol_composition_lexicographic,
                                         metadata.return_has_invalid_protocol_composition,
                                         false,
                                         handoff.protocol_category_composition_summary);
      };
  const auto accumulate_method_metadata_composition =
      [&handoff](const Objc3SemanticMethodTypeMetadata &metadata) {
        if (metadata.param_has_protocol_composition.size() != metadata.arity ||
            metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
            metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
          handoff.protocol_category_composition_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          AccumulateProtocolCompositionSite(metadata.param_has_protocol_composition[i],
                                           metadata.param_protocol_composition_lexicographic[i],
                                           metadata.param_has_invalid_protocol_composition[i],
                                           true,
                                           handoff.protocol_category_composition_summary);
        }
        AccumulateProtocolCompositionSite(metadata.return_has_protocol_composition,
                                         metadata.return_protocol_composition_lexicographic,
                                         metadata.return_has_invalid_protocol_composition,
                                         true,
                                         handoff.protocol_category_composition_summary);
      };

  for (const auto &function_metadata : handoff.functions_lexicographic) {
    accumulate_function_metadata_composition(function_metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method_metadata_composition(method_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method_metadata_composition(method_metadata);
    }
  }
  handoff.protocol_category_composition_summary.deterministic =
      handoff.protocol_category_composition_summary.deterministic &&
      handoff.protocol_category_composition_summary.invalid_protocol_composition_sites <=
          handoff.protocol_category_composition_summary.total_composition_sites() &&
      handoff.protocol_category_composition_summary.category_composition_sites <=
          handoff.protocol_category_composition_summary.protocol_composition_sites &&
      handoff.protocol_category_composition_summary.category_composition_symbols <=
          handoff.protocol_category_composition_summary.protocol_composition_symbols;
  handoff.class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(handoff.interface_implementation_summary,
                                               handoff.protocol_category_composition_summary);
