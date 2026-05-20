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

#include "sema/objc3_semantic_passes_protocol_category_summary.inc"

#include "sema/objc3_semantic_passes_selector_normalization_summary.inc"

#include "sema/objc3_semantic_passes_property_attribute_summary.inc"

#include "sema/objc3_semantic_passes_type_annotation_surface_summary.inc"

static void AccumulateCanonicalTypeForLightweightGenericConstraintSummary(
    const Objc3SemanticCanonicalType &type,
    Objc3LightweightGenericConstraintSummary &summary) {
  if (!type.object_type_facts_authoritative ||
      !type.generic_arguments_facts_authoritative) {
    ++summary.contract_violation_sites;
    summary.deterministic = false;
    return;
  }
  if (type.is_objc_object_reference) {
    ++summary.object_pointer_type_sites;
  }
  if (!type.has_generic_suffix) {
    return;
  }
  ++summary.generic_constraint_sites;
  ++summary.generic_suffix_sites;
  if (!type.has_invalid_generic_suffix && !type.has_invalid_type_suffix) {
    ++summary.terminated_generic_suffix_sites;
  }
  if (type.is_objc_object_reference && !type.has_invalid_generic_suffix &&
      !type.has_invalid_type_suffix) {
    ++summary.normalized_constraint_sites;
  } else {
    ++summary.contract_violation_sites;
  }
}

static void AccumulateCanonicalTypeForNullabilityFlowWarningPrecisionSummary(
    const Objc3SemanticCanonicalType &type,
    Objc3NullabilityFlowWarningPrecisionSummary &summary) {
  if (!type.object_type_facts_authoritative ||
      !type.nullability_facts_authoritative) {
    ++summary.contract_violation_sites;
    summary.deterministic = false;
    return;
  }
  if (type.is_objc_object_reference) {
    ++summary.object_pointer_type_sites;
  }
  if (!type.has_explicit_nullability) {
    return;
  }
  ++summary.nullability_flow_sites;
  ++summary.nullability_suffix_sites;
  if (type.nullability == Objc3SemanticCanonicalNullability::Nonnull) {
    ++summary.nonnull_suffix_sites;
  } else {
    ++summary.nullable_suffix_sites;
  }
  if (type.is_objc_object_reference && !type.has_invalid_nullability_suffix &&
      !type.has_invalid_type_suffix) {
    ++summary.normalized_sites;
  } else {
    ++summary.contract_violation_sites;
  }
}

template <typename Callback>
static void ForEachCanonicalCallableTypeInSurface(
    const Objc3SemanticIntegrationSurface &surface, Callback callback) {
  for (const auto &function_entry : surface.functions) {
    const FunctionInfo &function = function_entry.second;
    for (const auto &param_type : function.param_canonical_types) {
      callback(param_type);
    }
    callback(function.return_canonical_type);
  }
  const auto visit_container =
      [&callback](const auto &container_map) {
        for (const auto &container_entry : container_map) {
          for (const auto &property_entry : container_entry.second.properties) {
            callback(property_entry.second.canonical_type);
          }
          for (const auto &method_entry : container_entry.second.methods) {
            const Objc3MethodInfo &method = method_entry.second;
            for (const auto &param_type : method.param_canonical_types) {
              callback(param_type);
            }
            callback(method.return_canonical_type);
          }
        }
      };
  visit_container(surface.interfaces);
  visit_container(surface.implementations);
}

template <typename Callback>
static void ForEachCanonicalCallableTypeInHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff, Callback callback) {
  for (const auto &function : handoff.functions_lexicographic) {
    for (const auto &param_type : function.param_canonical_types) {
      callback(param_type);
    }
    callback(function.return_canonical_type);
  }
  const auto visit_container =
      [&callback](const auto &containers) {
        for (const auto &container : containers) {
          for (const auto &property : container.properties_lexicographic) {
            callback(property.canonical_type);
          }
          for (const auto &method : container.methods_lexicographic) {
            for (const auto &param_type : method.param_canonical_types) {
              callback(param_type);
            }
            callback(method.return_canonical_type);
          }
        }
      };
  visit_container(handoff.interfaces_lexicographic);
  visit_container(handoff.implementations_lexicographic);
}

static Objc3LightweightGenericConstraintSummary
BuildLightweightGenericConstraintSummaryFromCanonicalTypes(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3LightweightGenericConstraintSummary summary;
  summary.pointer_declarator_sites =
      surface.type_annotation_surface_summary.pointer_declarator_sites;
  ForEachCanonicalCallableTypeInSurface(
      surface,
      [&summary](const Objc3SemanticCanonicalType &type) {
        AccumulateCanonicalTypeForLightweightGenericConstraintSummary(type,
                                                                     summary);
      });
  summary.deterministic =
      summary.deterministic &&
      surface.type_annotation_surface_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.terminated_generic_suffix_sites <= summary.generic_suffix_sites &&
      summary.normalized_constraint_sites <= summary.generic_constraint_sites &&
      summary.contract_violation_sites <= summary.generic_constraint_sites;
  return summary;
}

static Objc3LightweightGenericConstraintSummary
BuildLightweightGenericConstraintSummaryFromCanonicalTypes(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3LightweightGenericConstraintSummary summary;
  summary.pointer_declarator_sites =
      handoff.type_annotation_surface_summary.pointer_declarator_sites;
  ForEachCanonicalCallableTypeInHandoff(
      handoff,
      [&summary](const Objc3SemanticCanonicalType &type) {
        AccumulateCanonicalTypeForLightweightGenericConstraintSummary(type,
                                                                     summary);
      });
  summary.deterministic =
      summary.deterministic &&
      handoff.type_annotation_surface_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.terminated_generic_suffix_sites <= summary.generic_suffix_sites &&
      summary.normalized_constraint_sites <= summary.generic_constraint_sites &&
      summary.contract_violation_sites <= summary.generic_constraint_sites;
  return summary;
}

static Objc3NullabilityFlowWarningPrecisionSummary
BuildNullabilityFlowWarningPrecisionSummaryFromCanonicalTypes(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3NullabilityFlowWarningPrecisionSummary summary;
  ForEachCanonicalCallableTypeInSurface(
      surface,
      [&summary](const Objc3SemanticCanonicalType &type) {
        AccumulateCanonicalTypeForNullabilityFlowWarningPrecisionSummary(
            type, summary);
      });
  summary.deterministic =
      summary.deterministic &&
      surface.type_annotation_surface_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.normalized_sites <= summary.nullability_flow_sites &&
      summary.contract_violation_sites <= summary.nullability_flow_sites &&
      summary.nullability_suffix_sites == summary.nullable_suffix_sites + summary.nonnull_suffix_sites;
  return summary;
}

static Objc3NullabilityFlowWarningPrecisionSummary
BuildNullabilityFlowWarningPrecisionSummaryFromCanonicalTypes(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3NullabilityFlowWarningPrecisionSummary summary;
  ForEachCanonicalCallableTypeInHandoff(
      handoff,
      [&summary](const Objc3SemanticCanonicalType &type) {
        AccumulateCanonicalTypeForNullabilityFlowWarningPrecisionSummary(
            type, summary);
      });
  summary.deterministic =
      summary.deterministic &&
      handoff.type_annotation_surface_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.normalized_sites <= summary.nullability_flow_sites &&
      summary.contract_violation_sites <= summary.nullability_flow_sites &&
      summary.nullability_suffix_sites == summary.nullable_suffix_sites + summary.nonnull_suffix_sites;
  return summary;
}

#include "sema/objc3_semantic_passes_protocol_qualified_object_summary_builders.inc"
