  return deterministic;
}

std::string ExplainNonDeterministicSemanticTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  if (!std::is_sorted(handoff.global_names_lexicographic.begin(),
                      handoff.global_names_lexicographic.end())) {
    return "global names are not lexicographically sorted";
  }
  if (!std::is_sorted(
          handoff.functions_lexicographic.begin(),
          handoff.functions_lexicographic.end(),
          [](const Objc3SemanticFunctionTypeMetadata &lhs,
             const Objc3SemanticFunctionTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return "function metadata rows are not lexicographically sorted";
  }
  if (!std::all_of(handoff.functions_lexicographic.begin(),
                   handoff.functions_lexicographic.end(),
                   [](const Objc3SemanticFunctionTypeMetadata &fn) {
                     return !fn.name.empty();
                   })) {
    return "function metadata row has an empty name";
  }
  if (!std::is_sorted(
          handoff.interfaces_lexicographic.begin(),
          handoff.interfaces_lexicographic.end(),
          [](const Objc3SemanticInterfaceTypeMetadata &lhs,
             const Objc3SemanticInterfaceTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return "interface metadata rows are not lexicographically sorted";
  }
  if (!std::is_sorted(
          handoff.implementations_lexicographic.begin(),
          handoff.implementations_lexicographic.end(),
          [](const Objc3SemanticImplementationTypeMetadata &lhs,
             const Objc3SemanticImplementationTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return "implementation metadata rows are not lexicographically sorted";
  }
  if (!std::is_sorted(handoff.message_send_selector_lowering_sites_lexicographic.begin(),
                      handoff.message_send_selector_lowering_sites_lexicographic.end(),
                      IsMessageSendSelectorLoweringSiteMetadataLess)) {
    return "message-send selector lowering sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_literal_capture_sites_lexicographic.begin(),
                      handoff.block_literal_capture_sites_lexicographic.end(),
                      IsBlockLiteralCaptureSiteMetadataLess)) {
    return "block literal capture sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_abi_invoke_trampoline_sites_lexicographic.begin(),
                      handoff.block_abi_invoke_trampoline_sites_lexicographic.end(),
                      IsBlockAbiInvokeTrampolineSiteMetadataLess)) {
    return "block ABI invoke-trampoline sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_storage_escape_sites_lexicographic.begin(),
                      handoff.block_storage_escape_sites_lexicographic.end(),
                      IsBlockStorageEscapeSiteMetadataLess)) {
    return "block storage-escape sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_copy_dispose_sites_lexicographic.begin(),
                      handoff.block_copy_dispose_sites_lexicographic.end(),
                      IsBlockCopyDisposeSiteMetadataLess)) {
    return "block copy/dispose sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_determinism_perf_baseline_sites_lexicographic.begin(),
                      handoff.block_determinism_perf_baseline_sites_lexicographic.end(),
                      IsBlockDeterminismPerfBaselineSiteMetadataLess)) {
    return "block determinism/perf baseline sites are not sorted";
  }
  if (!std::is_sorted(handoff.autoreleasepool_scope_sites_lexicographic.begin(),
                      handoff.autoreleasepool_scope_sites_lexicographic.end(),
                      IsAutoreleasePoolScopeSiteMetadataLess)) {
    return "autoreleasepool scope sites are not sorted";
  }

  const auto is_deterministic_method_metadata =
      [](const Objc3SemanticMethodTypeMetadata &metadata) {
        if (metadata.selector.empty() ||
            metadata.selector_normalized.empty() ||
            metadata.selector != metadata.selector_normalized ||
            metadata.selector_parameter_piece_count > metadata.selector_piece_count ||
            metadata.selector_had_pieceless_form !=
                (metadata.selector_piece_count == 0u) ||
            metadata.selector_has_arity_mismatch !=
                (metadata.selector_parameter_piece_count != metadata.arity)) {
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
        return true;
      };
  const auto is_deterministic_property_metadata =
      [](const Objc3SemanticPropertyTypeMetadata &metadata) {
        const bool expected_invalid_type_suffix =
            metadata.has_invalid_generic_suffix ||
            metadata.has_invalid_pointer_declarator ||
            metadata.has_invalid_nullability_suffix ||
            metadata.has_invalid_ownership_qualifier;
        if (metadata.name.empty() ||
            (metadata.has_invalid_generic_suffix &&
             !metadata.has_generic_suffix) ||
            (metadata.has_invalid_pointer_declarator &&
             !metadata.has_pointer_declarator) ||
            (metadata.has_invalid_nullability_suffix &&
             !metadata.has_nullability_suffix) ||
            (metadata.has_invalid_ownership_qualifier &&
             !metadata.has_ownership_qualifier) ||
            metadata.has_invalid_type_suffix !=
                expected_invalid_type_suffix) {
          return false;
        }
        if ((!metadata.has_ownership_qualifier &&
             (metadata.ownership_insert_retain ||
              metadata.ownership_insert_release ||
              metadata.ownership_insert_autorelease)) ||
            (metadata.ownership_insert_autorelease &&
             (metadata.ownership_insert_retain ||
              metadata.ownership_insert_release))) {
          return false;
        }
        if ((!metadata.ownership_arc_diagnostic_candidate &&
             (metadata.ownership_arc_fixit_available ||
              !metadata.ownership_arc_diagnostic_profile.empty() ||
              !metadata.ownership_arc_fixit_hint.empty())) ||
            (metadata.ownership_arc_fixit_available &&
             metadata.ownership_arc_fixit_hint.empty())) {
          return false;
        }
        if (!IsSortedUniqueStrings(metadata.attribute_names_lexicographic)) {
          return false;
        }
        if (metadata.attribute_entries !=
            metadata.attribute_names_lexicographic.size()) {
          return false;
        }
        if (!metadata.has_getter && !metadata.getter_selector.empty()) {
          return false;
        }
        if (!metadata.has_setter && !metadata.setter_selector.empty()) {
          return false;
        }
        if (!metadata.effective_setter_available &&
            !metadata.effective_setter_selector.empty()) {
          return false;
        }
        return true;
      };

  for (const auto &metadata : handoff.functions_lexicographic) {
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
      return "function metadata row '" + metadata.name +
             "' has parameter metadata arrays that drift from arity";
    }
  }

  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    if (!std::is_sorted(interface_metadata.methods_lexicographic.begin(),
                        interface_metadata.methods_lexicographic.end(),
                        [](const Objc3SemanticMethodTypeMetadata &lhs,
                           const Objc3SemanticMethodTypeMetadata &rhs) {
                          return lhs.selector < rhs.selector;
                        })) {
      return "interface '" + interface_metadata.name +
             "' method metadata rows are not lexicographically sorted";
    }
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      if (!is_deterministic_method_metadata(method_metadata)) {
        return "interface '" + interface_metadata.name + "' method '" +
               method_metadata.selector +
               "' metadata is not deterministic";
      }
    }
    if (!std::is_sorted(interface_metadata.properties_lexicographic.begin(),
                        interface_metadata.properties_lexicographic.end(),
                        [](const Objc3SemanticPropertyTypeMetadata &lhs,
                           const Objc3SemanticPropertyTypeMetadata &rhs) {
                          return lhs.name < rhs.name;
                        })) {
      return "interface '" + interface_metadata.name +
             "' property metadata rows are not lexicographically sorted";
    }
    for (const auto &property_metadata :
         interface_metadata.properties_lexicographic) {
      if (!is_deterministic_property_metadata(property_metadata)) {
        return "interface '" + interface_metadata.name + "' property '" +
               property_metadata.name +
               "' metadata is not deterministic";
      }
    }
  }

  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    if (!std::is_sorted(implementation_metadata.methods_lexicographic.begin(),
                        implementation_metadata.methods_lexicographic.end(),
                        [](const Objc3SemanticMethodTypeMetadata &lhs,
                           const Objc3SemanticMethodTypeMetadata &rhs) {
                          return lhs.selector < rhs.selector;
                        })) {
      return "implementation '" + implementation_metadata.name +
             "' method metadata rows are not lexicographically sorted";
    }
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      if (!is_deterministic_method_metadata(method_metadata)) {
        return "implementation '" + implementation_metadata.name + "' method '" +
               method_metadata.selector +
               "' metadata is not deterministic";
      }
    }
    if (!std::is_sorted(implementation_metadata.properties_lexicographic.begin(),
                        implementation_metadata.properties_lexicographic.end(),
                        [](const Objc3SemanticPropertyTypeMetadata &lhs,
                           const Objc3SemanticPropertyTypeMetadata &rhs) {
                          return lhs.name < rhs.name;
                        })) {
      return "implementation '" + implementation_metadata.name +
             "' property metadata rows are not lexicographically sorted";
    }
    for (const auto &property_metadata :
         implementation_metadata.properties_lexicographic) {
      if (!is_deterministic_property_metadata(property_metadata)) {
        return "implementation '" + implementation_metadata.name +
               "' property '" + property_metadata.name +
               "' metadata is not deterministic";
      }
    }
  }

  Objc3TypeAnnotationSurfaceSummary type_annotation_summary;
  const auto accumulate_function_type_annotations =
      [&type_annotation_summary](
          const Objc3SemanticFunctionTypeMetadata &metadata) {
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
          type_annotation_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          if (metadata.param_has_generic_suffix[i]) {
            ++type_annotation_summary.generic_suffix_sites;
          }
          if (metadata.param_has_pointer_declarator[i]) {
            ++type_annotation_summary.pointer_declarator_sites;
          }
          if (metadata.param_has_nullability_suffix[i]) {
            ++type_annotation_summary.nullability_suffix_sites;
          }
          if (metadata.param_has_ownership_qualifier[i]) {
            ++type_annotation_summary.ownership_qualifier_sites;
          }
          if (IsObjCReferenceAnnotationSite(metadata.param_id_spelling[i],
                                            metadata.param_class_spelling[i],
                                            metadata.param_instancetype_spelling[i],
                                            metadata.param_object_pointer_type_spelling[i])) {
            ++type_annotation_summary.object_pointer_type_sites;
          }
          if (metadata.param_has_invalid_generic_suffix[i]) {
            ++type_annotation_summary.invalid_generic_suffix_sites;
          }
          if (metadata.param_has_invalid_pointer_declarator[i]) {
            ++type_annotation_summary.invalid_pointer_declarator_sites;
          }
          if (metadata.param_has_invalid_nullability_suffix[i]) {
            ++type_annotation_summary.invalid_nullability_suffix_sites;
          }
          if (metadata.param_has_invalid_ownership_qualifier[i]) {
            ++type_annotation_summary.invalid_ownership_qualifier_sites;
          }
          const bool expected_invalid =
              metadata.param_has_invalid_generic_suffix[i] ||
              metadata.param_has_invalid_pointer_declarator[i] ||
              metadata.param_has_invalid_nullability_suffix[i] ||
              metadata.param_has_invalid_ownership_qualifier[i];
          if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
            type_annotation_summary.deterministic = false;
          }
        }
        if (metadata.return_has_generic_suffix) {
          ++type_annotation_summary.generic_suffix_sites;
        }
        if (metadata.return_has_pointer_declarator) {
          ++type_annotation_summary.pointer_declarator_sites;
        }
        if (metadata.return_has_nullability_suffix) {
          ++type_annotation_summary.nullability_suffix_sites;
        }
        if (metadata.return_has_ownership_qualifier) {
          ++type_annotation_summary.ownership_qualifier_sites;
        }
        if (IsObjCReferenceAnnotationSite(metadata.return_id_spelling,
                                          metadata.return_class_spelling,
                                          metadata.return_instancetype_spelling,
                                          metadata.return_object_pointer_type_spelling)) {
          ++type_annotation_summary.object_pointer_type_sites;
        }
        if (metadata.return_has_invalid_generic_suffix) {
          ++type_annotation_summary.invalid_generic_suffix_sites;
        }
        if (metadata.return_has_invalid_pointer_declarator) {
          ++type_annotation_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.return_has_invalid_nullability_suffix) {
          ++type_annotation_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.return_has_invalid_ownership_qualifier) {
          ++type_annotation_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_return_invalid =
            metadata.return_has_invalid_generic_suffix ||
            metadata.return_has_invalid_pointer_declarator ||
            metadata.return_has_invalid_nullability_suffix ||
            metadata.return_has_invalid_ownership_qualifier;
        if (metadata.return_has_invalid_type_suffix !=
            expected_return_invalid) {
          type_annotation_summary.deterministic = false;
        }
      };
  const auto accumulate_method_type_annotations =
      [&type_annotation_summary](
          const Objc3SemanticMethodTypeMetadata &metadata) {
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
          type_annotation_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          if (metadata.param_has_generic_suffix[i]) {
            ++type_annotation_summary.generic_suffix_sites;
          }
          if (metadata.param_has_pointer_declarator[i]) {
            ++type_annotation_summary.pointer_declarator_sites;
          }
          if (metadata.param_has_nullability_suffix[i]) {
            ++type_annotation_summary.nullability_suffix_sites;
          }
          if (metadata.param_has_ownership_qualifier[i]) {
            ++type_annotation_summary.ownership_qualifier_sites;
          }
          if (IsObjCReferenceAnnotationSite(metadata.param_id_spelling[i],
                                            metadata.param_class_spelling[i],
                                            metadata.param_instancetype_spelling[i],
                                            metadata.param_object_pointer_type_spelling[i])) {
            ++type_annotation_summary.object_pointer_type_sites;
          }
          if (metadata.param_has_invalid_generic_suffix[i]) {
            ++type_annotation_summary.invalid_generic_suffix_sites;
          }
          if (metadata.param_has_invalid_pointer_declarator[i]) {
            ++type_annotation_summary.invalid_pointer_declarator_sites;
          }
          if (metadata.param_has_invalid_nullability_suffix[i]) {
            ++type_annotation_summary.invalid_nullability_suffix_sites;
          }
          if (metadata.param_has_invalid_ownership_qualifier[i]) {
            ++type_annotation_summary.invalid_ownership_qualifier_sites;
          }
          const bool expected_invalid =
              metadata.param_has_invalid_generic_suffix[i] ||
              metadata.param_has_invalid_pointer_declarator[i] ||
              metadata.param_has_invalid_nullability_suffix[i] ||
              metadata.param_has_invalid_ownership_qualifier[i];
          if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
            type_annotation_summary.deterministic = false;
          }
        }
        if (metadata.return_has_generic_suffix) {
          ++type_annotation_summary.generic_suffix_sites;
        }
        if (metadata.return_has_pointer_declarator) {
          ++type_annotation_summary.pointer_declarator_sites;
        }
        if (metadata.return_has_nullability_suffix) {
          ++type_annotation_summary.nullability_suffix_sites;
        }
        if (metadata.return_has_ownership_qualifier) {
          ++type_annotation_summary.ownership_qualifier_sites;
        }
        if (IsObjCReferenceAnnotationSite(metadata.return_id_spelling,
                                          metadata.return_class_spelling,
                                          metadata.return_instancetype_spelling,
                                          metadata.return_object_pointer_type_spelling)) {
          ++type_annotation_summary.object_pointer_type_sites;
        }
        if (metadata.return_has_invalid_generic_suffix) {
          ++type_annotation_summary.invalid_generic_suffix_sites;
        }
        if (metadata.return_has_invalid_pointer_declarator) {
          ++type_annotation_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.return_has_invalid_nullability_suffix) {
          ++type_annotation_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.return_has_invalid_ownership_qualifier) {
          ++type_annotation_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_return_invalid =
            metadata.return_has_invalid_generic_suffix ||
            metadata.return_has_invalid_pointer_declarator ||
            metadata.return_has_invalid_nullability_suffix ||
            metadata.return_has_invalid_ownership_qualifier;
        if (metadata.return_has_invalid_type_suffix !=
            expected_return_invalid) {
          type_annotation_summary.deterministic = false;
        }
      };
  const auto accumulate_property_type_annotations =
      [&type_annotation_summary](
          const Objc3SemanticPropertyTypeMetadata &metadata) {
        if (metadata.has_generic_suffix) {
          ++type_annotation_summary.generic_suffix_sites;
        }
        if (metadata.has_pointer_declarator) {
          ++type_annotation_summary.pointer_declarator_sites;
        }
        if (metadata.has_nullability_suffix) {
          ++type_annotation_summary.nullability_suffix_sites;
        }
        if (metadata.has_ownership_qualifier) {
          ++type_annotation_summary.ownership_qualifier_sites;
        }
        if (IsObjCReferenceAnnotationSite(metadata.id_spelling,
                                          metadata.class_spelling,
                                          metadata.instancetype_spelling,
                                          metadata.object_pointer_type_spelling)) {
          ++type_annotation_summary.object_pointer_type_sites;
        }
        if (metadata.has_invalid_generic_suffix) {
          ++type_annotation_summary.invalid_generic_suffix_sites;
        }
        if (metadata.has_invalid_pointer_declarator) {
          ++type_annotation_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.has_invalid_nullability_suffix) {
          ++type_annotation_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.has_invalid_ownership_qualifier) {
          ++type_annotation_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_invalid =
            metadata.has_invalid_generic_suffix ||
            metadata.has_invalid_pointer_declarator ||
            metadata.has_invalid_nullability_suffix ||
            metadata.has_invalid_ownership_qualifier;
        if (metadata.has_invalid_type_suffix != expected_invalid) {
          type_annotation_summary.deterministic = false;
        }
      };
  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function_type_annotations(metadata);
  }
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method);
    }
    for (const auto &property : metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property);
    }
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method);
    }
    for (const auto &property : metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property);
    }
  }
  type_annotation_summary.deterministic =
      type_annotation_summary.deterministic &&
      type_annotation_summary.invalid_generic_suffix_sites <=
          type_annotation_summary.generic_suffix_sites &&
      type_annotation_summary.invalid_pointer_declarator_sites <=
          type_annotation_summary.pointer_declarator_sites &&
      type_annotation_summary.invalid_nullability_suffix_sites <=
          type_annotation_summary.nullability_suffix_sites &&
      type_annotation_summary.invalid_ownership_qualifier_sites <=
          type_annotation_summary.ownership_qualifier_sites &&
      type_annotation_summary.invalid_type_annotation_sites() <=
          type_annotation_summary.total_type_annotation_sites();
  if (!type_annotation_summary.deterministic) {
    return "type-annotation summary is not deterministic";
  }
  if (handoff.type_annotation_surface_summary.generic_suffix_sites !=
          type_annotation_summary.generic_suffix_sites ||
      handoff.type_annotation_surface_summary.pointer_declarator_sites !=
          type_annotation_summary.pointer_declarator_sites ||
      handoff.type_annotation_surface_summary.nullability_suffix_sites !=
          type_annotation_summary.nullability_suffix_sites ||
      handoff.type_annotation_surface_summary.ownership_qualifier_sites !=
          type_annotation_summary.ownership_qualifier_sites ||
      handoff.type_annotation_surface_summary.object_pointer_type_sites !=
          type_annotation_summary.object_pointer_type_sites ||
      handoff.type_annotation_surface_summary.invalid_generic_suffix_sites !=
          type_annotation_summary.invalid_generic_suffix_sites ||
      handoff.type_annotation_surface_summary
              .invalid_pointer_declarator_sites !=
          type_annotation_summary.invalid_pointer_declarator_sites ||
      handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites !=
          type_annotation_summary.invalid_nullability_suffix_sites ||
      handoff.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites !=
          type_annotation_summary.invalid_ownership_qualifier_sites) {
    std::ostringstream detail;
    detail << "type-annotation summary drifted from recomputed handoff"
           << " (object-pointer="
           << handoff.type_annotation_surface_summary.object_pointer_type_sites
           << "/" << type_annotation_summary.object_pointer_type_sites
           << ", ownership="
           << handoff.type_annotation_surface_summary.ownership_qualifier_sites
           << "/" << type_annotation_summary.ownership_qualifier_sites
           << ", invalid="
           << handoff.type_annotation_surface_summary.invalid_type_annotation_sites()
           << "/" << type_annotation_summary.invalid_type_annotation_sites()
           << ")";
    return detail.str();
  }

  const Objc3SymbolGraphScopeResolutionSummary
      symbol_graph_scope_resolution_summary =
          BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(handoff);
  if (!handoff.symbol_graph_scope_resolution_summary.deterministic) {
    return "symbol graph/scope resolution summary is not deterministic";
  }
  if (handoff.symbol_graph_scope_resolution_summary.global_symbol_nodes !=
          symbol_graph_scope_resolution_summary.global_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary.function_symbol_nodes !=
          symbol_graph_scope_resolution_summary.function_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary.interface_symbol_nodes !=
          symbol_graph_scope_resolution_summary.interface_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_symbol_nodes !=
          symbol_graph_scope_resolution_summary
              .implementation_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary
              .interface_property_symbol_nodes !=
          symbol_graph_scope_resolution_summary
              .interface_property_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_property_symbol_nodes !=
          symbol_graph_scope_resolution_summary
              .implementation_property_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes !=
          symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_method_symbol_nodes !=
          symbol_graph_scope_resolution_summary
              .implementation_method_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary.top_level_scope_symbols !=
          symbol_graph_scope_resolution_summary.top_level_scope_symbols ||
      handoff.symbol_graph_scope_resolution_summary.nested_scope_symbols !=
          symbol_graph_scope_resolution_summary.nested_scope_symbols ||
      handoff.symbol_graph_scope_resolution_summary.scope_frames_total !=
          symbol_graph_scope_resolution_summary.scope_frames_total ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites !=
          symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits !=
          symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_misses !=
          symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_misses ||
      handoff.symbol_graph_scope_resolution_summary.method_resolution_sites !=
          symbol_graph_scope_resolution_summary.method_resolution_sites ||
      handoff.symbol_graph_scope_resolution_summary.method_resolution_hits !=
          symbol_graph_scope_resolution_summary.method_resolution_hits ||
      handoff.symbol_graph_scope_resolution_summary.method_resolution_misses !=
          symbol_graph_scope_resolution_summary.method_resolution_misses) {
    return "symbol graph/scope resolution summary drifted from recomputed handoff";
  }

  const Objc3MethodLookupOverrideConflictSummary
      method_lookup_override_conflict_summary =
          BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(handoff);
  if (!handoff.method_lookup_override_conflict_summary.deterministic) {
    return "method lookup/override summary is not deterministic";
  }
  if (handoff.method_lookup_override_conflict_summary.method_lookup_sites !=
          method_lookup_override_conflict_summary.method_lookup_sites ||
      handoff.method_lookup_override_conflict_summary.method_lookup_hits !=
          method_lookup_override_conflict_summary.method_lookup_hits ||
      handoff.method_lookup_override_conflict_summary.method_lookup_misses !=
          method_lookup_override_conflict_summary.method_lookup_misses ||
      handoff.method_lookup_override_conflict_summary.override_lookup_sites !=
          method_lookup_override_conflict_summary.override_lookup_sites ||
      handoff.method_lookup_override_conflict_summary.override_lookup_hits !=
          method_lookup_override_conflict_summary.override_lookup_hits ||
      handoff.method_lookup_override_conflict_summary.override_lookup_misses !=
          method_lookup_override_conflict_summary.override_lookup_misses ||
      handoff.method_lookup_override_conflict_summary.override_conflicts !=
          method_lookup_override_conflict_summary.override_conflicts ||
      handoff.method_lookup_override_conflict_summary.unresolved_base_interfaces !=
          method_lookup_override_conflict_summary.unresolved_base_interfaces) {
    return "method lookup/override summary drifted from recomputed handoff";
  }

  const Objc3PropertySynthesisIvarBindingSummary
      property_synthesis_ivar_binding_summary =
          BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(handoff);
  if (!handoff.property_synthesis_ivar_binding_summary.deterministic) {
    return "property synthesis/ivar binding summary is not deterministic";
  }
  if (handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites !=
          property_synthesis_ivar_binding_summary.property_synthesis_sites ||
      handoff.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings !=
          property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings ||
      handoff.property_synthesis_ivar_binding_summary
              .property_synthesis_default_ivar_bindings !=
          property_synthesis_ivar_binding_summary
              .property_synthesis_default_ivar_bindings ||
      handoff.property_synthesis_ivar_binding_summary
              .interface_owned_property_synthesis_sites !=
          property_synthesis_ivar_binding_summary
              .interface_owned_property_synthesis_sites ||
      handoff.property_synthesis_ivar_binding_summary
              .implementation_property_redeclaration_sites !=
          property_synthesis_ivar_binding_summary
              .implementation_property_redeclaration_sites ||
      handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites !=
          property_synthesis_ivar_binding_summary.ivar_binding_sites ||
      handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved !=
          property_synthesis_ivar_binding_summary.ivar_binding_resolved ||
      handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing !=
          property_synthesis_ivar_binding_summary.ivar_binding_missing ||
      handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts !=
          property_synthesis_ivar_binding_summary.ivar_binding_conflicts) {
    std::ostringstream detail;
    detail << "property synthesis/ivar binding summary drifted from recomputed handoff"
           << " (sites="
           << handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites
           << "/" << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ", resolved="
           << handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << "/" << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ", missing="
           << handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing
           << "/" << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ", conflicts="
           << handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << "/" << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ")";
    return detail.str();
  }

  const Objc3IdClassSelObjectPointerTypeCheckingSummary
      id_class_sel_object_pointer_type_checking_summary =
          BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(
              handoff);
  if (!handoff.id_class_sel_object_pointer_type_checking_summary.deterministic) {
    return "id/Class/SEL/object-pointer checking summary is not deterministic";
  }
  if (handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_reference_form_count !=
          id_class_sel_object_pointer_type_checking_summary
              .canonical_reference_form_count ||
      handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_message_scalar_form_count !=
          id_class_sel_object_pointer_type_checking_summary
              .canonical_message_scalar_form_count ||
      handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_bridge_top_form_count !=
          id_class_sel_object_pointer_type_checking_summary
              .canonical_bridge_top_form_count ||
      handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites !=
          id_class_sel_object_pointer_type_checking_summary.param_type_sites ||
      handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites !=
          id_class_sel_object_pointer_type_checking_summary.return_type_sites ||
      handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites !=
          id_class_sel_object_pointer_type_checking_summary.property_type_sites) {
    return "id/Class/SEL/object-pointer checking summary drifted from recomputed handoff";
  }

  return "determinism invariant failed before the current debug summary checks";
}

void ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,
                            const Objc3SemanticValidationOptions &options,
                            std::vector<std::string> &diagnostics) {
  const Objc3SemanticBodyValidationContext context =
      BuildObjc3SemanticBodyValidationContext(program, surface, options, diagnostics);
  const Objc3Program &ast = *context.ast;
  std::vector<std::string> protocol_definition_diagnostics;
  const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
      protocol_definitions = BuildProtocolSemanticDefinitions(
          ast, options.arc_mode_enabled, protocol_definition_diagnostics);
  std::unordered_set<std::string> generic_interface_names;
  for (const auto &entry : surface.interfaces) {
    if (!entry.second.generic_parameter_names_source_order.empty()) {
      generic_interface_names.insert(entry.first);
    }
  }
  const auto borrowed_callable_contracts =
      BuildOwnershipBorrowedCallableContracts(ast);
  std::unordered_map<std::string, const Objc3InterfaceDecl *> actor_interfaces;
  for (const auto &interface_decl : ast.interfaces) {
    if (interface_decl.is_actor) {
      actor_interfaces.emplace(interface_decl.name, &interface_decl);
    }
  }
  std::unordered_map<std::string, ValueType> body_globals = surface.globals;
  for (const auto &interface_decl : ast.interfaces) {
    body_globals.try_emplace(interface_decl.name, ValueType::ObjCClass);
  }
  for (const auto &implementation_decl : ast.implementations) {
    body_globals.try_emplace(implementation_decl.name, ValueType::ObjCClass);
  }
  StaticScalarBindings global_static_bindings;
  std::unordered_set<std::string> assigned_identifier_names;
  for (const auto &fn : ast.functions) {
    CollectAssignedIdentifiers(fn.body, assigned_identifier_names);
  }
  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      if (!method.has_body) {
        continue;
      }
      CollectAssignedIdentifiers(method.body, assigned_identifier_names);
    }
  }
  std::vector<int> global_initializer_values;
  if (ResolveGlobalInitializerValues(ast.globals, global_initializer_values)) {
    const std::size_t count = std::min(ast.globals.size(), global_initializer_values.size());
    for (std::size_t i = 0; i < count; ++i) {
      const std::string &name = ast.globals[i].name;
      if (assigned_identifier_names.find(name) != assigned_identifier_names.end()) {
        continue;
      }
      global_static_bindings[name] = global_initializer_values[i];
    }
  }

  for (const auto &fn : ast.functions) {
    ValidateReturnTypeSuffixes(fn, diagnostics, &generic_interface_names);
    ValidateParameterTypeSuffixes(fn, diagnostics, &generic_interface_names);
    DiagnoseOwnershipRetainableFamilyCallableLegality(
        fn, "function '" + fn.name + "'", diagnostics);
    if (fn.executor_affinity_declared && !fn.async_declared) {
      diagnostics.push_back(MakeDiag(
          fn.line, fn.column, "O3S224",
          "async semantics failed: objc_executor requires an async function or method"));
    }
    if (fn.async_declared && fn.is_prototype) {
      diagnostics.push_back(MakeDiag(
          fn.line, fn.column, "O3S225",
          "async semantics failed: async function prototypes remain unsupported until continuation lowering lands"));
    }
    if (fn.async_declared && fn.throws_declared) {
      diagnostics.push_back(MakeDiag(
          fn.line, fn.column, "O3S226",
          "async semantics failed: async throws functions remain unsupported until async error propagation lands"));
    }

    std::vector<SemanticScope> scopes;
    scopes.push_back({});
    for (const auto &param : fn.params) {
      if (scopes.back().find(param.name) != scopes.back().end()) {
        diagnostics.push_back(MakeDiag(param.line, param.column, "O3S201", "duplicate parameter '" + param.name + "'"));
      } else {
        scopes.back().emplace(param.name, MakeSemanticTypeFromParam(param));
      }
    }

    if (!fn.is_prototype) {
      const Objc3ConcurrencyTaskCallableLegalityProfile task_legality_profile =
          BuildConcurrencyTaskCallableLegalityProfile(fn.body);
      DiagnoseConcurrencyTaskCallableLegality(fn.body, fn.async_declared, diagnostics);
      DiagnoseConcurrencyExecutorAffinityCompletion(
          task_legality_profile, fn.async_declared,
          fn.executor_affinity_declared, fn.executor_affinity_kind, fn.line,
          fn.column, diagnostics);
      DiagnoseOwnershipResourceMoveSemantics(fn.body, scopes, diagnostics);
      std::unordered_set<std::string> borrowed_parameter_names;
      for (const auto &param : fn.params) {
        if (param.borrowed_pointer_qualified) {
          borrowed_parameter_names.insert(param.name);
        }
      }
      DiagnoseOwnershipBorrowedPointerEscapeSemantics(
          fn.body, scopes, borrowed_parameter_names, borrowed_callable_contracts,
          BuildOwnershipBorrowedReturnContract(fn), diagnostics);
      const SemanticTypeInfo expected_return_type = MakeSemanticTypeFromFunctionReturn(fn);
      const StaticScalarBindings static_scalar_bindings = CollectFunctionStaticScalarBindings(fn, &global_static_bindings);
      Objc3MessageSendResolutionContext message_send_context;
      message_send_context.surface = &surface;
      message_send_context.protocol_definitions = &protocol_definitions;
      message_send_context.inside_async_context = fn.async_declared;
      ValidateStatements(fn.body, scopes, body_globals, surface.functions, expected_return_type, fn.name, diagnostics,
                         0, 0, 0, 0, false, options.max_message_send_args,
                         message_send_context);
      // Legacy extraction anchor retained for contract tests:
      // ValidateStatements(fn.body, scopes, surface.globals, surface.functions, fn.return_type, fn.name, diagnostics,
      if (!(expected_return_type.type == ValueType::Void && !expected_return_type.is_vector) &&
          !BlockAlwaysReturns(fn.body, &static_scalar_bindings)) {
        diagnostics.push_back(
            MakeDiag(fn.line, fn.column, "O3S205", "missing return path in function '" + fn.name + "'"));
      }
    }
  }

  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &method : protocol_decl.methods) {
      DiagnoseOwnershipRetainableFamilyCallableLegality(
          method,
          "method '" + MethodSelectorName(method) + "' in protocol '" +
              protocol_decl.name + "'",
          diagnostics);
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method : interface_decl.methods) {
      DiagnoseOwnershipRetainableFamilyCallableLegality(
          method,
          "method '" + MethodSelectorName(method) + "' in interface '" +
              interface_decl.name + "'",
          diagnostics);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      DiagnoseOwnershipRetainableFamilyCallableLegality(
          method,
          "method '" + MethodSelectorName(method) + "' in implementation '" +
              implementation_decl.name + "'",
          diagnostics);
      DiagnoseConcurrencyActorMethodIsolationRules(
          method, IsActorInterfaceOwner(actor_interfaces, implementation_decl.name),
          method.line, method.column, diagnostics);
      DiagnoseConcurrencyActorRaceHazardRules(
          method, IsActorInterfaceOwner(actor_interfaces, implementation_decl.name),
          method.line, method.column, diagnostics);
      if (method.executor_affinity_declared && !method.async_declared) {
        diagnostics.push_back(MakeDiag(
            method.line, method.column, "O3S224",
            "async semantics failed: objc_executor requires an async function or method"));
      }
      if (method.async_declared && method.throws_declared) {
        diagnostics.push_back(MakeDiag(
            method.line, method.column, "O3S226",
            "async semantics failed: async throws functions remain unsupported until async error propagation lands"));
      }
      if (!method.has_body) {
        continue;
      }

      std::vector<SemanticScope> scopes;
      scopes.push_back({});
      scopes.back().emplace("self",
                            method.is_class_method
                                ? MakeScalarSemanticType(ValueType::ObjCClass)
                                : MakeScalarSemanticType(ValueType::ObjCId));
      scopes.back().emplace("super",
                            method.is_class_method
                                ? MakeScalarSemanticType(ValueType::ObjCClass)
                                : MakeScalarSemanticType(ValueType::ObjCId));
      for (const auto &param : method.params) {
        if (scopes.back().find(param.name) != scopes.back().end()) {
          diagnostics.push_back(MakeDiag(param.line, param.column, "O3S201", "duplicate parameter '" + param.name + "'"));
        } else {
          scopes.back().emplace(param.name, MakeSemanticTypeFromParam(param));
        }
      }

      const Objc3ConcurrencyTaskCallableLegalityProfile task_legality_profile =
          BuildConcurrencyTaskCallableLegalityProfile(method.body);
      DiagnoseConcurrencyTaskCallableLegality(method.body, method.async_declared,
                                        diagnostics);
      DiagnoseConcurrencyExecutorAffinityCompletion(
          task_legality_profile, method.async_declared,
          method.executor_affinity_declared, method.executor_affinity_kind,
          method.line, method.column, diagnostics);
      DiagnoseOwnershipResourceMoveSemantics(method.body, scopes, diagnostics);
      std::unordered_set<std::string> borrowed_parameter_names;
      for (const auto &param : method.params) {
        if (param.borrowed_pointer_qualified) {
          borrowed_parameter_names.insert(param.name);
        }
      }
      DiagnoseOwnershipBorrowedPointerEscapeSemantics(
          method.body, scopes, borrowed_parameter_names,
          borrowed_callable_contracts,
          BuildOwnershipBorrowedReturnContract(method), diagnostics);
      const SemanticTypeInfo expected_return_type = MakeSemanticTypeFromMethodReturn(method);
      const std::string method_context =
          "method '" + MethodSelectorName(method) + "' in implementation '" + implementation_decl.name + "'";
      Objc3MessageSendResolutionContext message_send_context;
      message_send_context.surface = &surface;
      message_send_context.protocol_definitions = &protocol_definitions;
      message_send_context.current_implementation_name = implementation_decl.name;
      message_send_context.inside_method = true;
      message_send_context.is_class_method = method.is_class_method;
      message_send_context.inside_async_context = method.async_declared;
      const auto interface_it = surface.interfaces.find(implementation_decl.name);
      if (interface_it != surface.interfaces.end()) {
        message_send_context.current_super_name = interface_it->second.super_name;
      }
      ValidateStatements(method.body, scopes, body_globals, surface.functions, expected_return_type, method_context,
                         diagnostics, 0, 0, 0, 0, false,
                         options.max_message_send_args,
                         message_send_context);
      if (!(expected_return_type.type == ValueType::Void && !expected_return_type.is_vector) &&
          !BlockAlwaysReturns(method.body, &global_static_bindings)) {
        diagnostics.push_back(MakeDiag(method.line, method.column, "O3S205",
                                       "missing return path in " + method_context));
      }
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method : interface_decl.methods) {
      DiagnoseConcurrencyActorMethodIsolationRules(method, interface_decl.is_actor,
                                             method.line, method.column,
                                             diagnostics);
    }
  }
}

void RefreshSemanticIntegrationSurfaceAfterBodyValidation(
    const Objc3ParsedProgram &program,
    Objc3SemanticIntegrationSurface &surface) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  surface.block_copy_dispose_sites_lexicographic =
      BuildBlockCopyDisposeSiteMetadataLexicographic(ast);
  surface.block_copy_dispose_semantics_summary =
      BuildBlockCopyDisposeSemanticsSummaryFromIntegrationSurface(surface);
}
