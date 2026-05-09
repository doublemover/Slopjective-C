#include "parse/objc3_type_feature_profiles.h"

#include "parse/objc3_parser_profile_helpers.h"

#include <sstream>

namespace objc3c::parse {

std::string BuildLightweightGenericConstraintProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  const bool generic_instantiation_valid =
      !has_generic_suffix || (generic_suffix_terminated && object_pointer_type_spelling);
  std::ostringstream out;
  out << "lightweight-generics:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";suffix-bytes=" << generic_suffix_text.size()
      << ";instantiation-valid=" << (generic_instantiation_valid ? "true" : "false");
  return out.str();
}

bool IsLightweightGenericConstraintProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated) {
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}

std::string BuildNullabilityFlowProfile(
    bool object_pointer_type_spelling, std::size_t nullability_suffix_count,
    bool has_pointer_declarator, bool has_generic_suffix,
    bool generic_suffix_terminated) {
  const bool flow_precision_valid =
      nullability_suffix_count == 0 || object_pointer_type_spelling;
  std::ostringstream out;
  out << "nullability-flow:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";suffix-count=" << nullability_suffix_count
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";generic-terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";flow-precision-valid=" << (flow_precision_valid ? "true" : "false");
  return out.str();
}

bool IsNullabilityFlowProfileNormalized(bool object_pointer_type_spelling,
                                        std::size_t nullability_suffix_count) {
  if (nullability_suffix_count == 0) {
    return true;
  }
  return object_pointer_type_spelling;
}

std::string BuildVarianceBridgeCastProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t covariant_markers = CountMarkerOccurrences(generic_suffix_text, "__covariant");
  const std::size_t contravariant_markers = CountMarkerOccurrences(generic_suffix_text, "__contravariant");
  const std::size_t invariant_markers = CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_transfer_markers = CountMarkerOccurrences(generic_suffix_text, "__bridge_transfer");
  const std::size_t bridge_retained_markers = CountMarkerOccurrences(generic_suffix_text, "__bridge_retained");
  const std::size_t bridge_markers = CountMarkerOccurrences(generic_suffix_text, "__bridge") +
                                     CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge");
  const std::size_t bridge_transfer_total =
      bridge_transfer_markers + CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_transfer");
  const std::size_t bridge_retained_total =
      bridge_retained_markers + CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_retained");
  const bool variance_marked =
      covariant_markers + contravariant_markers + invariant_markers > 0;
  const bool bridge_marked = bridge_markers + bridge_transfer_total + bridge_retained_total > 0;
  const bool variance_safe = (covariant_markers == 0 || contravariant_markers == 0) &&
                             (covariant_markers + contravariant_markers <= 1);
  const bool bridge_cast_valid = bridge_transfer_total <= 1 && bridge_retained_total <= 1;
  const bool object_pointer_required_for_markers =
      !variance_marked && !bridge_marked ? true : object_pointer_type_spelling;

  std::ostringstream out;
  out << "variance-bridge-cast:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";covariant-markers=" << covariant_markers
      << ";contravariant-markers=" << contravariant_markers
      << ";invariant-markers=" << invariant_markers
      << ";bridge-markers=" << bridge_markers
      << ";bridge-transfer-markers=" << bridge_transfer_total
      << ";bridge-retained-markers=" << bridge_retained_total
      << ";variance-safe=" << (variance_safe ? "true" : "false")
      << ";bridge-cast-valid=" << (bridge_cast_valid ? "true" : "false")
      << ";marker-object-pointer-valid=" << (object_pointer_required_for_markers ? "true" : "false");
  return out.str();
}

bool IsVarianceBridgeCastProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t covariant_markers = CountMarkerOccurrences(generic_suffix_text, "__covariant");
  const std::size_t contravariant_markers = CountMarkerOccurrences(generic_suffix_text, "__contravariant");
  const std::size_t invariant_markers = CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_transfer_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_transfer") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_transfer");
  const std::size_t bridge_retained_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_retained") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_retained");
  const bool variance_marked =
      covariant_markers + contravariant_markers + invariant_markers > 0;
  const bool bridge_marked =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
          CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge") +
          bridge_transfer_markers + bridge_retained_markers >
      0;
  const bool variance_safe = (covariant_markers == 0 || contravariant_markers == 0) &&
                             (covariant_markers + contravariant_markers <= 1);
  const bool bridge_cast_valid = bridge_transfer_markers <= 1 && bridge_retained_markers <= 1;
  if (variance_marked && (!has_generic_suffix || !generic_suffix_terminated)) {
    return false;
  }
  if ((variance_marked || bridge_marked) && !object_pointer_type_spelling) {
    return false;
  }
  return variance_safe && bridge_cast_valid;
}

std::string BuildGenericMetadataAbiProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t generic_argument_slots =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t variance_markers =
      CountMarkerOccurrences(generic_suffix_text, "__covariant") +
      CountMarkerOccurrences(generic_suffix_text, "__contravariant") +
      CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge");
  const bool metadata_emission_ready =
      has_generic_suffix && generic_suffix_terminated && object_pointer_type_spelling &&
      generic_argument_slots > 0;
  const bool abi_layout_stable = metadata_emission_ready &&
                                 (!has_pointer_declarator || object_pointer_type_spelling);

  std::ostringstream out;
  out << "generic-metadata-abi:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";generic-argument-slots=" << generic_argument_slots
      << ";variance-markers=" << variance_markers
      << ";bridge-markers=" << bridge_markers
      << ";metadata-emission-ready=" << (metadata_emission_ready ? "true" : "false")
      << ";abi-layout-stable=" << (abi_layout_stable ? "true" : "false");
  return out.str();
}

bool IsGenericMetadataAbiProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  if (!has_generic_suffix) {
    return true;
  }

  const std::size_t generic_argument_slots =
      CountTopLevelGenericArgumentSlots(generic_suffix_text);
  if (!generic_suffix_terminated || !object_pointer_type_spelling ||
      generic_argument_slots == 0) {
    return false;
  }

  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  return true;
}

std::string BuildModuleImportGraphProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t module_segments = CountNamespaceSegments(object_pointer_type_name);
  const bool graph_well_formed =
      !has_generic_suffix ||
      (generic_suffix_terminated && object_pointer_type_spelling && import_edge_candidates > 0);
  const bool namespace_stable = module_segments <= 1 || object_pointer_type_spelling;

  std::ostringstream out;
  out << "module-import-graph:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";module-segments=" << module_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";graph-well-formed=" << (graph_well_formed ? "true" : "false")
      << ";namespace-stable=" << (namespace_stable ? "true" : "false");
  return out.str();
}

bool IsModuleImportGraphProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text) {
  if (!has_generic_suffix) {
    return true;
  }
  const std::size_t import_edge_candidates =
      CountTopLevelGenericArgumentSlots(generic_suffix_text);
  return generic_suffix_terminated &&
         object_pointer_type_spelling &&
         import_edge_candidates > 0;
}

std::string BuildNamespaceCollisionShadowingProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t namespace_segments = CountNamespaceSegments(object_pointer_type_name);
  const bool namespace_collision_risk = namespace_segments > 1 && import_edge_candidates > 0;
  const bool shadowing_risk = has_pointer_declarator && namespace_segments > 1;
  const bool diagnostics_ready =
      !namespace_collision_risk ||
      (generic_suffix_terminated && object_pointer_type_spelling);

  std::ostringstream out;
  out << "namespace-collision-shadowing:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << namespace_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";namespace-collision-risk=" << (namespace_collision_risk ? "true" : "false")
      << ";shadowing-risk=" << (shadowing_risk ? "true" : "false")
      << ";diagnostics-ready=" << (diagnostics_ready ? "true" : "false");
  return out.str();
}

bool IsNamespaceCollisionShadowingProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t namespace_segments = CountNamespaceSegments(object_pointer_type_name);
  const bool namespace_collision_risk = namespace_segments > 1 && import_edge_candidates > 0;
  if (!namespace_collision_risk) {
    return true;
  }
  return generic_suffix_terminated &&
         object_pointer_type_spelling;
}

std::string BuildPublicPrivateApiPartitionProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  const bool private_partition_required = namespace_segments > 1;
  const bool public_api_safe = !private_partition_required;
  const bool partition_ready = !private_partition_required ||
                               (generic_suffix_terminated &&
                                object_pointer_type_spelling);
  const bool pointer_partition_overlap =
      has_pointer_declarator && private_partition_required;

  std::ostringstream out;
  out << "public-private-api-partition:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << namespace_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";public-api-safe=" << (public_api_safe ? "true" : "false")
      << ";private-partition-required="
      << (private_partition_required ? "true" : "false")
      << ";partition-ready=" << (partition_ready ? "true" : "false")
      << ";pointer-partition-overlap="
      << (pointer_partition_overlap ? "true" : "false");
  return out.str();
}

bool IsPublicPrivateApiPartitionProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  const bool private_partition_required = namespace_segments > 1;
  if (!private_partition_required) {
    return true;
  }
  if (import_edge_candidates == 0) {
    return object_pointer_type_spelling;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}

std::string BuildIncrementalModuleCacheInvalidationProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  const bool cache_key_ready =
      object_pointer_type_spelling &&
      (!has_generic_suffix ||
       (generic_suffix_terminated && import_edge_candidates > 0));
  const bool cache_partitioned = namespace_segments > 1;
  const bool invalidation_on_shape_change =
      has_generic_suffix || has_pointer_declarator || cache_partitioned;
  const bool invalidation_ready =
      !invalidation_on_shape_change || cache_key_ready;

  std::ostringstream out;
  out << "incremental-module-cache-invalidation:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << namespace_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";cache-key-ready=" << (cache_key_ready ? "true" : "false")
      << ";cache-partitioned=" << (cache_partitioned ? "true" : "false")
      << ";invalidation-on-shape-change="
      << (invalidation_on_shape_change ? "true" : "false")
      << ";invalidation-ready=" << (invalidation_ready ? "true" : "false");
  return out.str();
}

bool IsIncrementalModuleCacheInvalidationProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  if (namespace_segments > 1 && !object_pointer_type_spelling) {
    return false;
  }
  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated &&
         object_pointer_type_spelling &&
         import_edge_candidates > 0;
}

std::string BuildCrossModuleConformanceProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  const bool cross_module_boundary_engaged =
      namespace_segments > 1 || has_generic_suffix;
  const bool conformance_surface_ready =
      object_pointer_type_spelling &&
      (!has_generic_suffix ||
       (generic_suffix_terminated && import_edge_candidates > 0));
  const bool boundary_shape_stable =
      !cross_module_boundary_engaged || conformance_surface_ready;
  const bool pointer_boundary_coupling =
      has_pointer_declarator && cross_module_boundary_engaged;
  const bool deterministic_handoff =
      boundary_shape_stable &&
      (!has_pointer_declarator || object_pointer_type_spelling);

  std::ostringstream out;
  out << "cross-module-conformance:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << namespace_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";cross-module-boundary-engaged="
      << (cross_module_boundary_engaged ? "true" : "false")
      << ";conformance-surface-ready="
      << (conformance_surface_ready ? "true" : "false")
      << ";boundary-shape-stable="
      << (boundary_shape_stable ? "true" : "false")
      << ";pointer-boundary-coupling="
      << (pointer_boundary_coupling ? "true" : "false")
      << ";deterministic-handoff="
      << (deterministic_handoff ? "true" : "false");
  return out.str();
}

bool IsCrossModuleConformanceProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  if (namespace_segments <= 1 && !has_generic_suffix) {
    return true;
  }
  if (!object_pointer_type_spelling) {
    return false;
  }
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && import_edge_candidates > 0;
}

std::string BuildThrowsDeclarationProfile(
    bool throws_declared, bool has_return_annotation, bool is_prototype,
    bool has_body, bool is_method_declaration, bool is_class_method,
    std::size_t parameter_count, std::size_t selector_piece_count) {
  const bool declaration_shape_valid =
      (is_prototype && !has_body) || (!is_prototype && has_body);
  const bool method_selector_surface_ready =
      !is_method_declaration || selector_piece_count > 0;
  const bool propagation_ready = declaration_shape_valid && method_selector_surface_ready;

  std::ostringstream out;
  out << "throws-declaration:declared=" << (throws_declared ? "true" : "false")
      << ";has-return-annotation=" << (has_return_annotation ? "true" : "false")
      << ";prototype=" << (is_prototype ? "true" : "false")
      << ";has-body=" << (has_body ? "true" : "false")
      << ";is-method-declaration=" << (is_method_declaration ? "true" : "false")
      << ";is-class-method=" << (is_class_method ? "true" : "false")
      << ";parameter-count=" << parameter_count
      << ";selector-piece-count=" << selector_piece_count
      << ";declaration-shape-valid=" << (declaration_shape_valid ? "true" : "false")
      << ";method-selector-surface-ready=" << (method_selector_surface_ready ? "true" : "false")
      << ";propagation-ready=" << (propagation_ready ? "true" : "false");
  return out.str();
}

bool IsThrowsDeclarationProfileNormalized(bool is_prototype, bool has_body,
                                          bool is_method_declaration,
                                          std::size_t selector_piece_count) {
  const bool declaration_shape_valid =
      (is_prototype && !has_body) || (!is_prototype && has_body);
  if (!declaration_shape_valid) {
    return false;
  }
  if (!is_method_declaration) {
    return true;
  }
  return selector_piece_count > 0;
}

}  // namespace objc3c::parse
