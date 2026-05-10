  summary.semantic_dependency_required =
      IsReadyObjc3MetaprogrammingExpansionBehaviorSemanticModelSummary(
          dependency_summary);
  summary.supported_derive_inventory_landed =
      summary.generated_method_entry_count + summary.selector_conflict_sites ==
          summary.supported_derive_request_sites &&
      summary.equality_derive_sites + summary.hash_derive_sites +
              summary.debug_description_derive_sites ==
          summary.supported_derive_request_sites;
  summary.unsupported_derive_fail_closed =
      summary.unsupported_derive_request_sites ==
      count_diagnostic("O3S317", "unsupported derive");
  summary.unsupported_topology_fail_closed =
      summary.unsupported_topology_sites ==
      count_diagnostic("O3S318", "primary interfaces");
  summary.selector_conflicts_fail_closed =
      summary.selector_conflict_sites ==
      count_diagnostic("O3S319", "derive expansion selector conflict");
  summary.runtime_materialization_deferred = true;
  summary.deterministic =
      summary.semantic_dependency_required &&
      summary.supported_derive_inventory_landed &&
      summary.unsupported_derive_fail_closed &&
      summary.unsupported_topology_fail_closed &&
      summary.selector_conflicts_fail_closed &&
      IsSortedUniqueStrings(summary.expansion_inventory_rows_lexicographic);
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "metaprogramming derive expansion inventory must remain deterministic and fail closed";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << dependency_summary.contract_id
      << ";derive-sites=" << summary.derive_request_sites << ":"
      << summary.supported_derive_request_sites << ":"
      << summary.unsupported_derive_request_sites << ":"
      << summary.unsupported_topology_sites
      << ";derived-method-entries=" << summary.generated_method_entry_count
      << ";selector-conflicts=" << summary.selector_conflict_sites
      << ";equatable-alias-sites=" << summary.equatable_alias_sites
      << ";canonical-derive-sites=" << summary.equality_derive_sites << ":"
      << summary.hash_derive_sites << ":"
      << summary.debug_description_derive_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

#include "sema/objc3_semantic_passes_metaprogramming_macro_property_summaries.inc"

#include "sema/objc3_semantic_passes_error_bridge_legality_summary.inc"

#include "sema/objc3_semantic_passes_static_scalar_binding_helpers.inc"

Objc3AtomicMemoryOrderMappingSummary BuildAtomicMemoryOrderMappingSummary(const Objc3ParsedProgram &program) {
  Objc3AtomicMemoryOrderMappingSummary summary;
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  for (const auto &fn : ast.functions) {
    CollectAtomicMemoryOrderMappingsInStatements(fn.body, summary);
  }
  return summary;
}

Objc3VectorTypeLoweringSummary BuildVectorTypeLoweringSummary(const Objc3SemanticIntegrationSurface &surface) {
  Objc3VectorTypeLoweringSummary summary;
  for (const auto &entry : surface.functions) {
    const FunctionInfo &fn = entry.second;
    if (fn.param_types.size() != fn.arity ||
        fn.param_is_vector.size() != fn.arity ||
        fn.param_vector_base_spelling.size() != fn.arity ||
        fn.param_vector_lane_count.size() != fn.arity ||
        fn.param_has_invalid_type_suffix.size() != fn.arity) {
      summary.deterministic = false;
      continue;
    }

    if (fn.return_is_vector) {
      RecordVectorTypeLoweringAnnotation(fn.return_type, fn.return_vector_lane_count, true, summary);
    }

    for (std::size_t i = 0; i < fn.arity; ++i) {
      if (!fn.param_is_vector[i]) {
        continue;
      }
      RecordVectorTypeLoweringAnnotation(fn.param_types[i], fn.param_vector_lane_count[i], false, summary);
    }
  }
  return summary;
}

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

static void AccumulateSelectorNormalizationFromMethodInfo(const Objc3MethodInfo &method,
                                                          Objc3SelectorNormalizationSummary &summary) {
  ++summary.methods_total;
  if (method.selector_contract_normalized) {
    ++summary.normalized_methods;
  }
  summary.selector_piece_entries += method.selector_piece_count;
  summary.selector_parameter_piece_entries += method.selector_parameter_piece_count;
  if (method.selector_had_pieceless_form) {
    ++summary.selector_pieceless_methods;
  }
  if (method.selector_has_spelling_mismatch) {
    ++summary.selector_spelling_mismatches;
  }
  if (method.selector_has_arity_mismatch) {
    ++summary.selector_arity_mismatches;
  }
  if (method.selector_has_parameter_linkage_mismatch) {
    ++summary.selector_parameter_linkage_mismatches;
  }
  if (method.selector_has_normalization_flag_mismatch) {
    ++summary.selector_normalization_flag_mismatches;
  }
  if (method.selector_has_missing_piece_keyword) {
    ++summary.selector_missing_keyword_pieces;
  }
  if (method.selector_parameter_piece_count > method.selector_piece_count ||
      method.selector_normalized.empty()) {
    summary.deterministic = false;
  }
}

static Objc3SelectorNormalizationSummary BuildSelectorNormalizationSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3SelectorNormalizationSummary summary;
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateSelectorNormalizationFromMethodInfo(method_entry.second, summary);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateSelectorNormalizationFromMethodInfo(method_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.normalized_methods <= summary.methods_total &&
                          summary.selector_parameter_piece_entries <= summary.selector_piece_entries &&
                          summary.contract_violations() <= summary.methods_total;
  return summary;
}

#include "sema/objc3_semantic_passes_property_attribute_summary.inc"

#include "sema/objc3_semantic_passes_type_annotation_surface_summary.inc"

static Objc3LightweightGenericConstraintSummary BuildLightweightGenericConstraintSummaryFromTypeAnnotationSurfaceSummary(
    const Objc3TypeAnnotationSurfaceSummary &type_annotation_summary) {
  Objc3LightweightGenericConstraintSummary summary;
  summary.generic_constraint_sites = type_annotation_summary.generic_suffix_sites;
  summary.generic_suffix_sites = type_annotation_summary.generic_suffix_sites;
  summary.object_pointer_type_sites = type_annotation_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites = type_annotation_summary.pointer_declarator_sites;
  summary.terminated_generic_suffix_sites =
      type_annotation_summary.generic_suffix_sites - type_annotation_summary.invalid_generic_suffix_sites;
  summary.normalized_constraint_sites =
      std::min(summary.terminated_generic_suffix_sites, summary.object_pointer_type_sites);
  summary.contract_violation_sites =
      type_annotation_summary.invalid_generic_suffix_sites +
      (summary.generic_suffix_sites > summary.object_pointer_type_sites
           ? summary.generic_suffix_sites - summary.object_pointer_type_sites
           : 0u);
  summary.deterministic =
      type_annotation_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.terminated_generic_suffix_sites <= summary.generic_suffix_sites &&
      summary.normalized_constraint_sites <= summary.generic_constraint_sites &&
      summary.contract_violation_sites <= summary.generic_constraint_sites;
  return summary;
}

static Objc3NullabilityFlowWarningPrecisionSummary
BuildNullabilityFlowWarningPrecisionSummaryFromTypeAnnotationSurfaceSummary(
    const Objc3TypeAnnotationSurfaceSummary &type_annotation_summary) {
  Objc3NullabilityFlowWarningPrecisionSummary summary;
  summary.nullability_flow_sites = type_annotation_summary.nullability_suffix_sites;
  summary.object_pointer_type_sites = type_annotation_summary.object_pointer_type_sites;
  summary.nullability_suffix_sites = type_annotation_summary.nullability_suffix_sites;
  summary.nullable_suffix_sites = type_annotation_summary.nullability_suffix_sites;
  summary.nonnull_suffix_sites = 0;
  summary.normalized_sites =
      summary.nullability_suffix_sites - type_annotation_summary.invalid_nullability_suffix_sites;
  summary.contract_violation_sites =
      type_annotation_summary.invalid_nullability_suffix_sites +
      (summary.nullability_suffix_sites > summary.object_pointer_type_sites
           ? summary.nullability_suffix_sites - summary.object_pointer_type_sites
           : 0u);
  summary.deterministic =
      type_annotation_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.normalized_sites <= summary.nullability_flow_sites &&
      summary.contract_violation_sites <= summary.nullability_flow_sites &&
      summary.nullability_suffix_sites == summary.nullable_suffix_sites + summary.nonnull_suffix_sites;
  return summary;
}

static Objc3ProtocolQualifiedObjectTypeSummary
BuildProtocolQualifiedObjectTypeSummaryFromTypeAnnotationSurfaceSummary(
    const Objc3TypeAnnotationSurfaceSummary &type_annotation_summary) {
  Objc3ProtocolQualifiedObjectTypeSummary summary;
  summary.protocol_qualified_object_type_sites = type_annotation_summary.generic_suffix_sites;
  summary.protocol_composition_sites = type_annotation_summary.generic_suffix_sites;
  summary.object_pointer_type_sites = type_annotation_summary.object_pointer_type_sites;
  summary.terminated_protocol_composition_sites =
      type_annotation_summary.generic_suffix_sites - type_annotation_summary.invalid_generic_suffix_sites;
