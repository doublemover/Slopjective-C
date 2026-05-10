#include "sema/objc3_semantic_passes_type_metadata_protocol_category_summary.inc"

  handoff.type_annotation_surface_summary = Objc3TypeAnnotationSurfaceSummary{};
  const auto accumulate_function_type_annotations =
      [&handoff](const Objc3SemanticFunctionTypeMetadata &metadata) {
        if (metadata.param_has_generic_suffix.size() != metadata.arity ||
            metadata.param_has_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_ownership_qualifier.size() != metadata.arity ||
            metadata.param_id_spelling.size() != metadata.arity ||
            metadata.param_class_spelling.size() != metadata.arity ||
            metadata.param_instancetype_spelling.size() != metadata.arity ||
            metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
            metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
            metadata.param_has_invalid_type_suffix.size() != metadata.arity) {
          handoff.type_annotation_surface_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          if (metadata.param_has_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.generic_suffix_sites;
          }
          if (metadata.param_has_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
          }
          if (metadata.param_has_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
          }
          if (metadata.param_has_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
          }
          if (IsObjCReferenceAnnotationSite(metadata.param_id_spelling[i],
                                            metadata.param_class_spelling[i],
                                            metadata.param_instancetype_spelling[i],
                                            metadata.param_object_pointer_type_spelling[i])) {
            ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
          }
          if (metadata.param_has_invalid_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
          }
          if (metadata.param_has_invalid_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
          }
          if (metadata.param_has_invalid_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
          }
          if (metadata.param_has_invalid_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
          }
          const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                        metadata.param_has_invalid_pointer_declarator[i] ||
                                        metadata.param_has_invalid_nullability_suffix[i] ||
                                        metadata.param_has_invalid_ownership_qualifier[i];
          if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
            handoff.type_annotation_surface_summary.deterministic = false;
          }
        }
        if (metadata.return_has_generic_suffix) {
          ++handoff.type_annotation_surface_summary.generic_suffix_sites;
        }
        if (metadata.return_has_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
        }
        if (metadata.return_has_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
        }
        if (metadata.return_has_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
        }
        if (IsObjCReferenceAnnotationSite(metadata.return_id_spelling,
                                          metadata.return_class_spelling,
                                          metadata.return_instancetype_spelling,
                                          metadata.return_object_pointer_type_spelling)) {
          ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
        }
        if (metadata.return_has_invalid_generic_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
        }
        if (metadata.return_has_invalid_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.return_has_invalid_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.return_has_invalid_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_return_invalid = metadata.return_has_invalid_generic_suffix ||
                                             metadata.return_has_invalid_pointer_declarator ||
                                             metadata.return_has_invalid_nullability_suffix ||
                                             metadata.return_has_invalid_ownership_qualifier;
        if (metadata.return_has_invalid_type_suffix != expected_return_invalid) {
          handoff.type_annotation_surface_summary.deterministic = false;
        }
      };
  const auto accumulate_method_type_annotations =
      [&handoff](const Objc3SemanticMethodTypeMetadata &metadata) {
        if (metadata.param_has_generic_suffix.size() != metadata.arity ||
            metadata.param_has_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_ownership_qualifier.size() != metadata.arity ||
            metadata.param_id_spelling.size() != metadata.arity ||
            metadata.param_class_spelling.size() != metadata.arity ||
            metadata.param_instancetype_spelling.size() != metadata.arity ||
            metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
            metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
            metadata.param_has_invalid_type_suffix.size() != metadata.arity) {
          handoff.type_annotation_surface_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          if (metadata.param_has_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.generic_suffix_sites;
          }
          if (metadata.param_has_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
          }
          if (metadata.param_has_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
          }
          if (metadata.param_has_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
          }
          if (IsObjCReferenceAnnotationSite(metadata.param_id_spelling[i],
                                            metadata.param_class_spelling[i],
                                            metadata.param_instancetype_spelling[i],
                                            metadata.param_object_pointer_type_spelling[i])) {
            ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
          }
          if (metadata.param_has_invalid_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
          }
          if (metadata.param_has_invalid_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
          }
          if (metadata.param_has_invalid_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
          }
          if (metadata.param_has_invalid_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
          }
          const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                        metadata.param_has_invalid_pointer_declarator[i] ||
                                        metadata.param_has_invalid_nullability_suffix[i] ||
                                        metadata.param_has_invalid_ownership_qualifier[i];
          if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
            handoff.type_annotation_surface_summary.deterministic = false;
          }
        }
        if (metadata.return_has_generic_suffix) {
          ++handoff.type_annotation_surface_summary.generic_suffix_sites;
        }
        if (metadata.return_has_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
        }
        if (metadata.return_has_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
        }
        if (metadata.return_has_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
        }
        if (IsObjCReferenceAnnotationSite(metadata.return_id_spelling,
                                          metadata.return_class_spelling,
                                          metadata.return_instancetype_spelling,
                                          metadata.return_object_pointer_type_spelling)) {
          ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
        }
        if (metadata.return_has_invalid_generic_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
        }
        if (metadata.return_has_invalid_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.return_has_invalid_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.return_has_invalid_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_return_invalid = metadata.return_has_invalid_generic_suffix ||
                                             metadata.return_has_invalid_pointer_declarator ||
                                             metadata.return_has_invalid_nullability_suffix ||
                                             metadata.return_has_invalid_ownership_qualifier;
        if (metadata.return_has_invalid_type_suffix != expected_return_invalid) {
          handoff.type_annotation_surface_summary.deterministic = false;
        }
      };
  const auto accumulate_property_type_annotations =
      [&handoff](const Objc3SemanticPropertyTypeMetadata &metadata) {
        if (metadata.has_generic_suffix) {
          ++handoff.type_annotation_surface_summary.generic_suffix_sites;
        }
        if (metadata.has_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
        }
        if (metadata.has_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
        }
        if (metadata.has_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
        }
        if (IsObjCReferenceAnnotationSite(metadata.id_spelling,
                                          metadata.class_spelling,
                                          metadata.instancetype_spelling,
                                          metadata.object_pointer_type_spelling)) {
          ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
        }
        if (metadata.has_invalid_generic_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
        }
        if (metadata.has_invalid_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.has_invalid_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.has_invalid_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_invalid = metadata.has_invalid_generic_suffix ||
                                      metadata.has_invalid_pointer_declarator ||
                                      metadata.has_invalid_nullability_suffix ||
                                      metadata.has_invalid_ownership_qualifier;
        if (metadata.has_invalid_type_suffix != expected_invalid) {
          handoff.type_annotation_surface_summary.deterministic = false;
        }
      };
  for (const auto &function_metadata : handoff.functions_lexicographic) {
    accumulate_function_type_annotations(function_metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property_metadata);
    }
  }
  handoff.type_annotation_surface_summary.deterministic =
      handoff.type_annotation_surface_summary.deterministic &&
      handoff.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          handoff.type_annotation_surface_summary.generic_suffix_sites &&
      handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          handoff.type_annotation_surface_summary.pointer_declarator_sites &&
      handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          handoff.type_annotation_surface_summary.nullability_suffix_sites &&
      handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
          handoff.type_annotation_surface_summary.ownership_qualifier_sites &&
      handoff.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          handoff.type_annotation_surface_summary.total_type_annotation_sites();
#include "sema/objc3_semantic_passes_type_metadata_summary_publication.inc"
  return handoff;
}

bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  if (!std::is_sorted(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end())) {
    return false;
  }
  if (!std::is_sorted(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),
                       [](const Objc3SemanticFunctionTypeMetadata &lhs, const Objc3SemanticFunctionTypeMetadata &rhs) {
                         return lhs.name < rhs.name;
                       })) {
    return false;
  }
  // Legacy extraction anchor retained for contract tests:
  // return std::all_of(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),
  const bool functions_metadata_rows_named =
      std::all_of(handoff.functions_lexicographic.begin(),
                  handoff.functions_lexicographic.end(),
                  [](const Objc3SemanticFunctionTypeMetadata &fn) { return !fn.name.empty(); });
  if (!functions_metadata_rows_named) {
    return false;
  }
  if (!std::is_sorted(handoff.interfaces_lexicographic.begin(), handoff.interfaces_lexicographic.end(),
                      [](const Objc3SemanticInterfaceTypeMetadata &lhs, const Objc3SemanticInterfaceTypeMetadata &rhs) {
                        return lhs.name < rhs.name;
                      })) {
    return false;
  }
  if (!std::is_sorted(
          handoff.implementations_lexicographic.begin(),
          handoff.implementations_lexicographic.end(),
          [](const Objc3SemanticImplementationTypeMetadata &lhs, const Objc3SemanticImplementationTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return false;
  }
  if (!std::is_sorted(handoff.message_send_selector_lowering_sites_lexicographic.begin(),
                      handoff.message_send_selector_lowering_sites_lexicographic.end(),
                      IsMessageSendSelectorLoweringSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_literal_capture_sites_lexicographic.begin(),
                      handoff.block_literal_capture_sites_lexicographic.end(),
                      IsBlockLiteralCaptureSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_abi_invoke_trampoline_sites_lexicographic.begin(),
                      handoff.block_abi_invoke_trampoline_sites_lexicographic.end(),
                      IsBlockAbiInvokeTrampolineSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_storage_escape_sites_lexicographic.begin(),
                      handoff.block_storage_escape_sites_lexicographic.end(),
                      IsBlockStorageEscapeSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_copy_dispose_sites_lexicographic.begin(),
                      handoff.block_copy_dispose_sites_lexicographic.end(),
                      IsBlockCopyDisposeSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_determinism_perf_baseline_sites_lexicographic.begin(),
                      handoff.block_determinism_perf_baseline_sites_lexicographic.end(),
                      IsBlockDeterminismPerfBaselineSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.autoreleasepool_scope_sites_lexicographic.begin(),
                      handoff.autoreleasepool_scope_sites_lexicographic.end(),
                      IsAutoreleasePoolScopeSiteMetadataLess)) {
    return false;
  }

  const auto is_deterministic_method_metadata = [](const Objc3SemanticMethodTypeMetadata &metadata) {
    if (metadata.selector.empty() ||
        metadata.selector_normalized.empty() ||
        metadata.selector != metadata.selector_normalized ||
        metadata.selector_parameter_piece_count > metadata.selector_piece_count ||
        metadata.selector_had_pieceless_form != (metadata.selector_piece_count == 0u) ||
        metadata.selector_has_arity_mismatch != (metadata.selector_parameter_piece_count != metadata.arity)) {
      return false;
    }
    if (metadata.selector_contract_normalized &&
        (metadata.selector_had_pieceless_form ||
         metadata.selector_has_spelling_mismatch ||
         metadata.selector_has_arity_mismatch ||
         metadata.selector_has_parameter_linkage_mismatch ||
         metadata.selector_has_normalization_flag_mismatch ||
         metadata.selector_has_missing_piece_keyword)) {
      return false;
    }
    if (metadata.selector_has_missing_piece_keyword && metadata.selector_contract_normalized) {
      return false;
    }
    const bool param_types_match_arity = metadata.param_types.size() == metadata.arity;
    const bool param_invalid_type_suffix_match_arity =
        metadata.param_has_invalid_type_suffix.size() == metadata.arity;
    const bool param_is_vector_match_arity = metadata.param_is_vector.size() == metadata.arity;
    const bool param_vector_base_spelling_match_arity =
        metadata.param_vector_base_spelling.size() == metadata.arity;
    const bool param_vector_lane_count_match_arity =
        metadata.param_vector_lane_count.size() == metadata.arity;
    if (!param_types_match_arity ||
        metadata.param_types.size() != metadata.arity ||
        !param_is_vector_match_arity ||
        !param_vector_base_spelling_match_arity ||
        !param_vector_lane_count_match_arity ||
        metadata.param_has_generic_suffix.size() != metadata.arity ||
        metadata.param_has_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_ownership_qualifier.size() != metadata.arity ||
        metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
        metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
        !param_invalid_type_suffix_match_arity ||
        metadata.param_has_invalid_type_suffix.size() != metadata.arity ||
        metadata.param_ownership_insert_retain.size() != metadata.arity ||
        metadata.param_ownership_insert_release.size() != metadata.arity ||
        metadata.param_ownership_insert_autorelease.size() != metadata.arity ||
        metadata.param_ownership_arc_diagnostic_candidate.size() != metadata.arity ||
        metadata.param_ownership_arc_fixit_available.size() != metadata.arity ||
        metadata.param_ownership_arc_diagnostic_profile.size() != metadata.arity ||
        metadata.param_ownership_arc_fixit_hint.size() != metadata.arity ||
        metadata.param_has_protocol_composition.size() != metadata.arity ||
        metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
        metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
      return false;
    }
    if ((!metadata.return_ownership_arc_diagnostic_candidate &&
         (metadata.return_ownership_arc_fixit_available ||
          !metadata.return_ownership_arc_diagnostic_profile.empty() ||
          !metadata.return_ownership_arc_fixit_hint.empty())) ||
        (metadata.return_ownership_arc_fixit_available &&
         metadata.return_ownership_arc_fixit_hint.empty())) {
      return false;
    }
    if ((metadata.return_has_invalid_generic_suffix && !metadata.return_has_generic_suffix) ||
        (metadata.return_has_invalid_pointer_declarator && !metadata.return_has_pointer_declarator) ||
        (metadata.return_has_invalid_nullability_suffix && !metadata.return_has_nullability_suffix) ||
        (metadata.return_has_invalid_ownership_qualifier && !metadata.return_has_ownership_qualifier) ||
        (metadata.return_has_invalid_type_suffix !=
         (metadata.return_has_invalid_generic_suffix || metadata.return_has_invalid_pointer_declarator ||
          metadata.return_has_invalid_nullability_suffix || metadata.return_has_invalid_ownership_qualifier))) {
      return false;
    }
    if (metadata.return_has_invalid_protocol_composition && !metadata.return_has_protocol_composition) {
      return false;
    }
    if (metadata.return_has_protocol_composition &&
        !IsSortedUniqueStrings(metadata.return_protocol_composition_lexicographic)) {
      return false;
    }
    for (std::size_t i = 0; i < metadata.arity; ++i) {
      const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                    metadata.param_has_invalid_pointer_declarator[i] ||
                                    metadata.param_has_invalid_nullability_suffix[i] ||
                                    metadata.param_has_invalid_ownership_qualifier[i];
      if ((metadata.param_has_invalid_generic_suffix[i] && !metadata.param_has_generic_suffix[i]) ||
          (metadata.param_has_invalid_pointer_declarator[i] && !metadata.param_has_pointer_declarator[i]) ||
          (metadata.param_has_invalid_nullability_suffix[i] && !metadata.param_has_nullability_suffix[i]) ||
          (metadata.param_has_invalid_ownership_qualifier[i] && !metadata.param_has_ownership_qualifier[i]) ||
          metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
        return false;
      }
      if ((!metadata.param_has_ownership_qualifier[i] &&
           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i] ||
            metadata.param_ownership_insert_autorelease[i])) ||
          (metadata.param_ownership_insert_autorelease[i] &&
           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i]))) {
        return false;
      }
      if ((!metadata.param_ownership_arc_diagnostic_candidate[i] &&
           (metadata.param_ownership_arc_fixit_available[i] ||
            !metadata.param_ownership_arc_diagnostic_profile[i].empty() ||
            !metadata.param_ownership_arc_fixit_hint[i].empty())) ||
          (metadata.param_ownership_arc_fixit_available[i] &&
           metadata.param_ownership_arc_fixit_hint[i].empty())) {
        return false;
      }
      if (!IsSortedUniqueStrings(metadata.param_protocol_composition_lexicographic[i])) {
        return false;
      }
      if (metadata.param_has_invalid_protocol_composition[i] && !metadata.param_has_protocol_composition[i]) {
        return false;
      }
    }
    if ((!metadata.return_has_ownership_qualifier &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
          metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      return false;
    }
    return true;
  };

  const bool deterministic_functions =
      std::all_of(handoff.functions_lexicographic.begin(),
                  handoff.functions_lexicographic.end(),
                  [](const Objc3SemanticFunctionTypeMetadata &metadata) {
                    if (metadata.param_types.size() != metadata.arity ||
                        metadata.param_is_vector.size() != metadata.arity ||
                        metadata.param_vector_base_spelling.size() != metadata.arity ||
                        metadata.param_vector_lane_count.size() != metadata.arity ||
                        metadata.param_has_generic_suffix.size() != metadata.arity ||
                        metadata.param_has_pointer_declarator.size() != metadata.arity ||
                        metadata.param_has_nullability_suffix.size() != metadata.arity ||
                        metadata.param_has_ownership_qualifier.size() != metadata.arity ||
                        metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
                        metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
                        metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
                        metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
                        metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
                        metadata.param_has_invalid_type_suffix.size() != metadata.arity ||
                        metadata.param_ownership_insert_retain.size() != metadata.arity ||
                        metadata.param_ownership_insert_release.size() != metadata.arity ||
                        metadata.param_ownership_insert_autorelease.size() != metadata.arity ||
                        metadata.param_ownership_arc_diagnostic_candidate.size() != metadata.arity ||
                        metadata.param_ownership_arc_fixit_available.size() != metadata.arity ||
                        metadata.param_ownership_arc_diagnostic_profile.size() != metadata.arity ||
                        metadata.param_ownership_arc_fixit_hint.size() != metadata.arity ||
                        metadata.param_has_protocol_composition.size() != metadata.arity ||
                        metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
                        metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
                      return false;
                    }
                    if ((!metadata.return_ownership_arc_diagnostic_candidate &&
                         (metadata.return_ownership_arc_fixit_available ||
                          !metadata.return_ownership_arc_diagnostic_profile.empty() ||
                          !metadata.return_ownership_arc_fixit_hint.empty())) ||
                        (metadata.return_ownership_arc_fixit_available &&
                         metadata.return_ownership_arc_fixit_hint.empty())) {
                      return false;
                    }
                    if ((metadata.return_has_invalid_generic_suffix && !metadata.return_has_generic_suffix) ||
                        (metadata.return_has_invalid_pointer_declarator && !metadata.return_has_pointer_declarator) ||
                        (metadata.return_has_invalid_nullability_suffix && !metadata.return_has_nullability_suffix) ||
                        (metadata.return_has_invalid_ownership_qualifier && !metadata.return_has_ownership_qualifier) ||
                        (metadata.return_has_invalid_type_suffix !=
                         (metadata.return_has_invalid_generic_suffix || metadata.return_has_invalid_pointer_declarator ||
                          metadata.return_has_invalid_nullability_suffix ||
                          metadata.return_has_invalid_ownership_qualifier))) {
                      return false;
                    }
                    if (metadata.return_has_invalid_protocol_composition && !metadata.return_has_protocol_composition) {
                      return false;
                    }
                    if (metadata.return_has_protocol_composition &&
                        !IsSortedUniqueStrings(metadata.return_protocol_composition_lexicographic)) {
                      return false;
                    }
                    for (std::size_t i = 0; i < metadata.arity; ++i) {
                      const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                                    metadata.param_has_invalid_pointer_declarator[i] ||
                                                    metadata.param_has_invalid_nullability_suffix[i] ||
                                                    metadata.param_has_invalid_ownership_qualifier[i];
                      if ((metadata.param_has_invalid_generic_suffix[i] && !metadata.param_has_generic_suffix[i]) ||
                          (metadata.param_has_invalid_pointer_declarator[i] && !metadata.param_has_pointer_declarator[i]) ||
                          (metadata.param_has_invalid_nullability_suffix[i] && !metadata.param_has_nullability_suffix[i]) ||
                          (metadata.param_has_invalid_ownership_qualifier[i] &&
                           !metadata.param_has_ownership_qualifier[i]) ||
                          metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
                        return false;
                      }
                      if ((!metadata.param_has_ownership_qualifier[i] &&
                           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i] ||
                            metadata.param_ownership_insert_autorelease[i])) ||
                          (metadata.param_ownership_insert_autorelease[i] &&
                           (metadata.param_ownership_insert_retain[i] ||
                            metadata.param_ownership_insert_release[i]))) {
                        return false;
                      }
                      if ((!metadata.param_ownership_arc_diagnostic_candidate[i] &&
                           (metadata.param_ownership_arc_fixit_available[i] ||
                            !metadata.param_ownership_arc_diagnostic_profile[i].empty() ||
                            !metadata.param_ownership_arc_fixit_hint[i].empty())) ||
                          (metadata.param_ownership_arc_fixit_available[i] &&
                           metadata.param_ownership_arc_fixit_hint[i].empty())) {
                        return false;
                      }
                      if (!IsSortedUniqueStrings(metadata.param_protocol_composition_lexicographic[i])) {
                        return false;
                      }
                      if (metadata.param_has_invalid_protocol_composition[i] &&
                          !metadata.param_has_protocol_composition[i]) {
                        return false;
                      }
                    }
                    if ((!metadata.return_has_ownership_qualifier &&
                         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
                          metadata.return_ownership_insert_autorelease)) ||
                        (metadata.return_ownership_insert_autorelease &&
                         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
                      return false;
                    }
                    return true;
                  });

  const bool deterministic_interfaces =
      std::all_of(handoff.interfaces_lexicographic.begin(),
                  handoff.interfaces_lexicographic.end(),
                  [&is_deterministic_method_metadata](const Objc3SemanticInterfaceTypeMetadata &metadata) {
                    return std::is_sorted(metadata.methods_lexicographic.begin(),
                                          metadata.methods_lexicographic.end(),
                                          [](const Objc3SemanticMethodTypeMetadata &lhs,
                                             const Objc3SemanticMethodTypeMetadata &rhs) {
                                            return lhs.selector < rhs.selector;
                                          }) &&
                           std::all_of(metadata.methods_lexicographic.begin(),
                                       metadata.methods_lexicographic.end(),
                                       is_deterministic_method_metadata);
                  });

  const bool deterministic_implementations =
      std::all_of(
          handoff.implementations_lexicographic.begin(),
          handoff.implementations_lexicographic.end(),
          [&is_deterministic_method_metadata](const Objc3SemanticImplementationTypeMetadata &metadata) {
            return std::is_sorted(metadata.methods_lexicographic.begin(),
                                  metadata.methods_lexicographic.end(),
                                  [](const Objc3SemanticMethodTypeMetadata &lhs,
                                     const Objc3SemanticMethodTypeMetadata &rhs) {
                                    return lhs.selector < rhs.selector;
                                  }) &&
                   std::all_of(metadata.methods_lexicographic.begin(),
                               metadata.methods_lexicographic.end(),
                               is_deterministic_method_metadata);
          });

  if (!deterministic_functions || !deterministic_interfaces || !deterministic_implementations) {
    return false;
  }

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

  Objc3SelectorNormalizationSummary selector_summary;
  const auto accumulate_selector_summary = [&selector_summary](const Objc3SemanticMethodTypeMetadata &metadata) {
    ++selector_summary.methods_total;
    if (metadata.selector_contract_normalized) {
      ++selector_summary.normalized_methods;
    }
    selector_summary.selector_piece_entries += metadata.selector_piece_count;
    selector_summary.selector_parameter_piece_entries += metadata.selector_parameter_piece_count;
    if (metadata.selector_had_pieceless_form) {
      ++selector_summary.selector_pieceless_methods;
    }
    if (metadata.selector_has_spelling_mismatch) {
      ++selector_summary.selector_spelling_mismatches;
    }
    if (metadata.selector_has_arity_mismatch) {
      ++selector_summary.selector_arity_mismatches;
    }
    if (metadata.selector_has_parameter_linkage_mismatch) {
      ++selector_summary.selector_parameter_linkage_mismatches;
    }
    if (metadata.selector_has_normalization_flag_mismatch) {
      ++selector_summary.selector_normalization_flag_mismatches;
    }
    if (metadata.selector_has_missing_piece_keyword) {
      ++selector_summary.selector_missing_keyword_pieces;
    }
    if (metadata.selector.empty() ||
        metadata.selector_normalized.empty() ||
        metadata.selector != metadata.selector_normalized ||
        metadata.selector_parameter_piece_count > metadata.selector_piece_count) {
      selector_summary.deterministic = false;
    }
  };
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_selector_summary(method);
    }
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_selector_summary(method);
    }
  }
  selector_summary.deterministic =
      selector_summary.deterministic &&
      selector_summary.normalized_methods <= selector_summary.methods_total &&
      selector_summary.selector_parameter_piece_entries <= selector_summary.selector_piece_entries &&
      selector_summary.contract_violations() <= selector_summary.methods_total;

#include "sema/objc3_semantic_passes_type_metadata_determinism_type_annotations.inc"

  std::size_t interface_method_symbols = 0;
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interface_method_symbols += metadata.methods_lexicographic.size();
  }
  std::size_t implementation_method_symbols = 0;
  for (const auto &metadata : handoff.implementations_lexicographic) {
    implementation_method_symbols += metadata.methods_lexicographic.size();
  }
  const Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_summary =
      BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(handoff);
  const Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary =
      BuildLightweightGenericConstraintSummaryFromTypeAnnotationSurfaceSummary(
          handoff.type_annotation_surface_summary);
  const Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary =
      BuildNullabilityFlowWarningPrecisionSummaryFromTypeAnnotationSurfaceSummary(
          handoff.type_annotation_surface_summary);
  const Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary =
      BuildProtocolQualifiedObjectTypeSummaryFromTypeAnnotationSurfaceSummary(
          handoff.type_annotation_surface_summary);
  const Objc3VarianceBridgeCastSummary variance_bridge_cast_summary =
      BuildVarianceBridgeCastSummaryFromTypeAnnotationAndProtocolSummary(
          handoff.type_annotation_surface_summary,
          handoff.protocol_qualified_object_type_summary);
  const Objc3GenericMetadataAbiSummary generic_metadata_abi_summary =
      BuildGenericMetadataAbiSummaryFromTypeAnnotationAndConstraintSummaries(
          handoff.type_annotation_surface_summary,
          handoff.lightweight_generic_constraint_summary,
          handoff.variance_bridge_cast_summary);
  const Objc3ModuleImportGraphSummary module_import_graph_summary =
      BuildModuleImportGraphSummaryFromTypeAnnotationAndGenericMetadataSummary(
          handoff.type_annotation_surface_summary,
          handoff.generic_metadata_abi_summary);
  const Objc3NamespaceCollisionShadowingSummary
      namespace_collision_shadowing_summary =
          BuildNamespaceCollisionShadowingSummaryFromModuleImportGraphSummary(
              handoff.module_import_graph_summary);
  const Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary =
      BuildPublicPrivateApiPartitionSummaryFromNamespaceCollisionShadowingSummary(
          handoff.namespace_collision_shadowing_summary);
  const Objc3IncrementalModuleCacheInvalidationSummary
      incremental_module_cache_invalidation_summary =
          BuildIncrementalModuleCacheInvalidationSummaryFromPublicPrivateApiPartitionSummary(
              handoff.public_private_api_partition_summary);
  const Objc3CrossModuleConformanceSummary cross_module_conformance_summary =
      BuildCrossModuleConformanceSummaryFromIncrementalModuleCacheInvalidationSummary(
          handoff.incremental_module_cache_invalidation_summary);
  const Objc3ThrowsPropagationSummary throws_propagation_summary =
      BuildThrowsPropagationSummaryFromCrossModuleConformanceSummary(
          handoff.cross_module_conformance_summary);
  const Objc3AsyncContinuationSummary async_continuation_summary =
      BuildAsyncContinuationSummaryFromTypeMetadataHandoff(handoff);
  const Objc3AwaitLoweringSuspensionStateSummary
      await_lowering_suspension_state_lowering_summary =
          handoff.await_lowering_suspension_state_lowering_summary;
  const Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary =
      BuildActorIsolationSendabilitySummaryFromTypeMetadataHandoff(handoff);
  const Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary =
      BuildTaskRuntimeCancellationSummaryFromTypeMetadataHandoff(handoff);
  const Objc3ConcurrencyReplayRaceGuardSummary
      concurrency_replay_race_guard_summary =
          BuildConcurrencyReplayRaceGuardSummaryFromTypeMetadataHandoff(handoff);
  const Objc3NSErrorBridgingSummary ns_error_bridging_summary =
      BuildNSErrorBridgingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3ResultLikeLoweringSummary result_like_lowering_summary =
      handoff.result_like_lowering_summary;
  const Objc3ErrorDiagnosticsRecoverySummary
      error_diagnostics_recovery_summary =
          handoff.error_diagnostics_recovery_summary;
  const Objc3UnwindCleanupSummary unwind_cleanup_summary =
      BuildUnwindCleanupSummaryFromThrowsAndResultSummaries(
          handoff.throws_propagation_summary,
          handoff.result_like_lowering_summary);
  const Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary =
      BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(handoff);
  // property-ivar executable-source-closure anchor:
  // freeze the source-surface replay key above layout/accessor realization.
  const Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary =
      BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary =
      BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary =
      BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary =
      BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary =
      BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary =
      BuildBlockCopyDisposeSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary =
      BuildBlockDeterminismPerfBaselineSummaryFromTypeMetadataHandoff(handoff);
  const Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary =
      BuildMessageSendSelectorLoweringSummaryFromTypeMetadataHandoff(handoff);
  const Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary =
      BuildDispatchAbiMarshallingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary =
      BuildNilReceiverSemanticsFoldabilitySummaryFromTypeMetadataHandoff(handoff);
  const Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary =
      BuildSuperDispatchMethodFamilySummaryFromTypeMetadataHandoff(handoff);
  const Objc3RuntimeLinkHostLinkSummary runtime_link_host_link_summary =
      BuildRuntimeLinkHostLinkSummaryFromTypeMetadataHandoff(handoff);
  const Objc3RetainReleaseOperationSummary retain_release_operation_summary =
      BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(handoff);
  const Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary =
      BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary =
      BuildUnsafePointerExtensionSummaryFromTypeAnnotationAndWeakUnownedSummaries(
          handoff.type_annotation_surface_summary,
          handoff.weak_unowned_semantics_summary);
  const Objc3InlineAsmIntrinsicGovernanceSummary
      inline_asm_intrinsic_governance_summary =
          BuildInlineAsmIntrinsicGovernanceSummaryFromUnsafePointerAndThrowsSummaries(
              handoff.unsafe_pointer_extension_summary,
              handoff.throws_propagation_summary);
  // semantic anchor: Part 12 advanced diagnostic taxonomy,
  // portability publication, and feature-specific fix-it synthesis all consume
  // this existing deterministic ARC/fix-it semantic slice as the live
  // baseline.
  const Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary =
      BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff(handoff);
  const Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary =
      BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff(handoff);
  const Objc3InterfaceImplementationSummary &summary = handoff.interface_implementation_summary;
  const Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(summary, protocol_category_summary);
  const bool deterministic = summary.deterministic &&
         summary.resolved_interfaces == handoff.interfaces_lexicographic.size() &&
         summary.resolved_implementations == handoff.implementations_lexicographic.size() &&
         summary.interface_method_symbols == interface_method_symbols &&
         summary.implementation_method_symbols == implementation_method_symbols &&
         summary.linked_implementation_symbols <= summary.implementation_method_symbols &&
         summary.linked_implementation_symbols <= summary.interface_method_symbols &&
         handoff.class_protocol_category_linking_summary.deterministic &&
         handoff.class_protocol_category_linking_summary.declared_interfaces ==
             class_protocol_category_linking_summary.declared_interfaces &&
         handoff.class_protocol_category_linking_summary.resolved_interfaces ==
             class_protocol_category_linking_summary.resolved_interfaces &&
         handoff.class_protocol_category_linking_summary.declared_implementations ==
             class_protocol_category_linking_summary.declared_implementations &&
         handoff.class_protocol_category_linking_summary.resolved_implementations ==
             class_protocol_category_linking_summary.resolved_implementations &&
         handoff.class_protocol_category_linking_summary.interface_method_symbols ==
             class_protocol_category_linking_summary.interface_method_symbols &&
         handoff.class_protocol_category_linking_summary.implementation_method_symbols ==
             class_protocol_category_linking_summary.implementation_method_symbols &&
         handoff.class_protocol_category_linking_summary.linked_implementation_symbols ==
             class_protocol_category_linking_summary.linked_implementation_symbols &&
         handoff.class_protocol_category_linking_summary.protocol_composition_sites ==
             class_protocol_category_linking_summary.protocol_composition_sites &&
         handoff.class_protocol_category_linking_summary.protocol_composition_symbols ==
             class_protocol_category_linking_summary.protocol_composition_symbols &&
         handoff.class_protocol_category_linking_summary.category_composition_sites ==
             class_protocol_category_linking_summary.category_composition_sites &&
         handoff.class_protocol_category_linking_summary.category_composition_symbols ==
             class_protocol_category_linking_summary.category_composition_symbols &&
         handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
             class_protocol_category_linking_summary.invalid_protocol_composition_sites &&
         handoff.protocol_category_composition_summary.deterministic &&
         handoff.protocol_category_composition_summary.protocol_composition_sites ==
             protocol_category_summary.protocol_composition_sites &&
         handoff.protocol_category_composition_summary.protocol_composition_symbols ==
             protocol_category_summary.protocol_composition_symbols &&
         handoff.protocol_category_composition_summary.category_composition_sites ==
             protocol_category_summary.category_composition_sites &&
         handoff.protocol_category_composition_summary.category_composition_symbols ==
              protocol_category_summary.category_composition_symbols &&
         handoff.protocol_category_composition_summary.invalid_protocol_composition_sites ==
              protocol_category_summary.invalid_protocol_composition_sites &&
         handoff.selector_normalization_summary.deterministic &&
         handoff.selector_normalization_summary.methods_total == selector_summary.methods_total &&
         handoff.selector_normalization_summary.normalized_methods == selector_summary.normalized_methods &&
         handoff.selector_normalization_summary.selector_piece_entries == selector_summary.selector_piece_entries &&
         handoff.selector_normalization_summary.selector_parameter_piece_entries ==
             selector_summary.selector_parameter_piece_entries &&
         handoff.selector_normalization_summary.selector_pieceless_methods ==
             selector_summary.selector_pieceless_methods &&
         handoff.selector_normalization_summary.selector_spelling_mismatches ==
             selector_summary.selector_spelling_mismatches &&
         handoff.selector_normalization_summary.selector_arity_mismatches ==
             selector_summary.selector_arity_mismatches &&
         handoff.selector_normalization_summary.selector_parameter_linkage_mismatches ==
             selector_summary.selector_parameter_linkage_mismatches &&
         handoff.selector_normalization_summary.selector_normalization_flag_mismatches ==
             selector_summary.selector_normalization_flag_mismatches &&
         handoff.selector_normalization_summary.selector_missing_keyword_pieces ==
             selector_summary.selector_missing_keyword_pieces &&
         handoff.type_annotation_surface_summary.deterministic &&
         handoff.type_annotation_surface_summary.generic_suffix_sites == type_annotation_summary.generic_suffix_sites &&
         handoff.type_annotation_surface_summary.pointer_declarator_sites ==
             type_annotation_summary.pointer_declarator_sites &&
         handoff.type_annotation_surface_summary.nullability_suffix_sites ==
             type_annotation_summary.nullability_suffix_sites &&
         handoff.type_annotation_surface_summary.ownership_qualifier_sites ==
             type_annotation_summary.ownership_qualifier_sites &&
         handoff.type_annotation_surface_summary.object_pointer_type_sites ==
             type_annotation_summary.object_pointer_type_sites &&
         handoff.type_annotation_surface_summary.invalid_generic_suffix_sites ==
             type_annotation_summary.invalid_generic_suffix_sites &&
         handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
             type_annotation_summary.invalid_pointer_declarator_sites &&
         handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
             type_annotation_summary.invalid_nullability_suffix_sites &&
         handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==
             type_annotation_summary.invalid_ownership_qualifier_sites &&
         handoff.lightweight_generic_constraint_summary.deterministic &&
         handoff.lightweight_generic_constraint_summary.generic_constraint_sites ==
             lightweight_generic_constraint_summary.generic_constraint_sites &&
         handoff.lightweight_generic_constraint_summary.generic_suffix_sites ==
             lightweight_generic_constraint_summary.generic_suffix_sites &&
         handoff.lightweight_generic_constraint_summary.object_pointer_type_sites ==
             lightweight_generic_constraint_summary.object_pointer_type_sites &&
         handoff.lightweight_generic_constraint_summary.terminated_generic_suffix_sites ==
             lightweight_generic_constraint_summary.terminated_generic_suffix_sites &&
         handoff.lightweight_generic_constraint_summary.pointer_declarator_sites ==
             lightweight_generic_constraint_summary.pointer_declarator_sites &&
         handoff.lightweight_generic_constraint_summary.normalized_constraint_sites ==
             lightweight_generic_constraint_summary.normalized_constraint_sites &&
         handoff.lightweight_generic_constraint_summary.contract_violation_sites ==
             lightweight_generic_constraint_summary.contract_violation_sites &&
         handoff.lightweight_generic_constraint_summary.terminated_generic_suffix_sites <=
             handoff.lightweight_generic_constraint_summary.generic_suffix_sites &&
         handoff.lightweight_generic_constraint_summary.normalized_constraint_sites <=
             handoff.lightweight_generic_constraint_summary.generic_constraint_sites &&
         handoff.lightweight_generic_constraint_summary.contract_violation_sites <=
             handoff.lightweight_generic_constraint_summary.generic_constraint_sites &&
         handoff.nullability_flow_warning_precision_summary.deterministic &&
         handoff.nullability_flow_warning_precision_summary.nullability_flow_sites ==
             nullability_flow_warning_precision_summary.nullability_flow_sites &&
         handoff.nullability_flow_warning_precision_summary.object_pointer_type_sites ==
             nullability_flow_warning_precision_summary.object_pointer_type_sites &&
         handoff.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
             nullability_flow_warning_precision_summary.nullability_suffix_sites &&
         handoff.nullability_flow_warning_precision_summary.nullable_suffix_sites ==
             nullability_flow_warning_precision_summary.nullable_suffix_sites &&
         handoff.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
             nullability_flow_warning_precision_summary.nonnull_suffix_sites &&
         handoff.nullability_flow_warning_precision_summary.normalized_sites ==
             nullability_flow_warning_precision_summary.normalized_sites &&
         handoff.nullability_flow_warning_precision_summary.contract_violation_sites ==
             nullability_flow_warning_precision_summary.contract_violation_sites &&
