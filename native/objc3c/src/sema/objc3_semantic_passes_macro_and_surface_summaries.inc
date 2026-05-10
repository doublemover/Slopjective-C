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
