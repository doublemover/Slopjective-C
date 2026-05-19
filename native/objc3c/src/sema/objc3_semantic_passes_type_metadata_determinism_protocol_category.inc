  Objc3ProtocolCategoryCompositionSummary protocol_category_summary;
  const auto accumulate_function_metadata_composition =
      [&protocol_category_summary](const Objc3SemanticFunctionTypeMetadata &metadata) {
        if (metadata.param_has_protocol_composition.size() != metadata.arity ||
            metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
            metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
          protocol_category_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          AccumulateProtocolCompositionSite(metadata.param_has_protocol_composition[i],
                                           metadata.param_protocol_composition_lexicographic[i],
                                           metadata.param_has_invalid_protocol_composition[i],
                                           false,
                                           protocol_category_summary);
        }
        AccumulateProtocolCompositionSite(metadata.return_has_protocol_composition,
                                         metadata.return_protocol_composition_lexicographic,
                                         metadata.return_has_invalid_protocol_composition,
                                         false,
                                         protocol_category_summary);
      };
  const auto accumulate_method_metadata_composition =
      [&protocol_category_summary](const Objc3SemanticMethodTypeMetadata &metadata) {
        if (metadata.param_has_protocol_composition.size() != metadata.arity ||
            metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
            metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
          protocol_category_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          AccumulateProtocolCompositionSite(metadata.param_has_protocol_composition[i],
                                           metadata.param_protocol_composition_lexicographic[i],
                                           metadata.param_has_invalid_protocol_composition[i],
                                           true,
                                           protocol_category_summary);
        }
        AccumulateProtocolCompositionSite(metadata.return_has_protocol_composition,
                                         metadata.return_protocol_composition_lexicographic,
                                         metadata.return_has_invalid_protocol_composition,
                                         true,
                                         protocol_category_summary);
      };
  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function_metadata_composition(metadata);
  }
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_metadata_composition(method);
    }
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_metadata_composition(method);
    }
  }
  protocol_category_summary.deterministic =
      protocol_category_summary.deterministic &&
      protocol_category_summary.invalid_protocol_composition_sites <=
          protocol_category_summary.total_composition_sites() &&
      protocol_category_summary.category_composition_sites <= protocol_category_summary.protocol_composition_sites &&
      protocol_category_summary.category_composition_symbols <= protocol_category_summary.protocol_composition_symbols;
