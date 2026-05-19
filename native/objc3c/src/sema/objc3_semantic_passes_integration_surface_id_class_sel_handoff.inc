static Objc3IdClassSelObjectPointerTypeCheckingSummary
BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3IdClassSelObjectPointerTypeCheckingSummary summary;
  ApplyTypeFormScaffoldSummaryToIdClassSelObjectPointerTypeCheckingSummary(summary);

  const auto accumulate_function = [&summary](const Objc3SemanticFunctionTypeMetadata &metadata) {
    ++summary.return_type_sites;
    if (metadata.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (metadata.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (metadata.return_type == ValueType::ObjCSel) {
      ++summary.return_sel_spelling_sites;
    }
    if (metadata.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (metadata.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += metadata.param_types.size();
    const std::size_t count = std::min(
        metadata.param_types.size(),
        std::min(metadata.param_id_spelling.size(),
                 std::min(metadata.param_class_spelling.size(),
                          std::min(metadata.param_instancetype_spelling.size(),
                                   metadata.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (metadata.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (metadata.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (metadata.param_types[i] == ValueType::ObjCSel) {
        ++summary.param_sel_spelling_sites;
      }
      if (metadata.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (metadata.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != metadata.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_method = [&summary](const Objc3SemanticMethodTypeMetadata &metadata) {
    ++summary.return_type_sites;
    if (metadata.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (metadata.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (metadata.return_type == ValueType::ObjCSel) {
      ++summary.return_sel_spelling_sites;
    }
    if (metadata.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (metadata.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += metadata.param_types.size();
    const std::size_t count = std::min(
        metadata.param_types.size(),
        std::min(metadata.param_id_spelling.size(),
                 std::min(metadata.param_class_spelling.size(),
                          std::min(metadata.param_instancetype_spelling.size(),
                                   metadata.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (metadata.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (metadata.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (metadata.param_types[i] == ValueType::ObjCSel) {
        ++summary.param_sel_spelling_sites;
      }
      if (metadata.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (metadata.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != metadata.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_property = [&summary](const Objc3SemanticPropertyTypeMetadata &metadata) {
    ++summary.property_type_sites;
    if (metadata.id_spelling) {
      ++summary.property_id_spelling_sites;
    }
    if (metadata.class_spelling) {
      ++summary.property_class_spelling_sites;
    }
    if (metadata.type == ValueType::ObjCSel) {
      ++summary.property_sel_spelling_sites;
    }
    if (metadata.instancetype_spelling) {
      ++summary.property_instancetype_spelling_sites;
    }
    if (metadata.object_pointer_type_spelling) {
      ++summary.property_object_pointer_type_sites;
    }
  };

  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function(metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.canonical_reference_form_count == kObjc3CanonicalReferenceTypeForms.size() &&
      summary.canonical_message_scalar_form_count == kObjc3CanonicalScalarMessageSendTypeForms.size() &&
      summary.canonical_bridge_top_form_count == kObjc3CanonicalBridgeTopReferenceTypeForms.size() &&
      summary.canonical_reference_forms_unique &&
      summary.canonical_message_scalar_forms_unique &&
      summary.canonical_bridge_top_forms_unique &&
      summary.canonical_bridge_top_subset_of_reference &&
      summary.canonical_type_form_scaffold_ready &&
      summary.canonical_type_form_diagnostics_hardening_consistent &&
      summary.canonical_type_form_diagnostics_hardening_ready &&
      !summary.canonical_type_form_diagnostics_hardening_key.empty() &&
      summary.canonical_type_form_recovery_determinism_consistent &&
      summary.canonical_type_form_recovery_determinism_ready &&
      !summary.canonical_type_form_recovery_determinism_key.empty() &&
      summary.canonical_type_form_conformance_matrix_consistent &&
      summary.canonical_type_form_conformance_matrix_ready &&
      !summary.canonical_type_form_conformance_matrix_key.empty() &&
      summary.canonical_type_form_conformance_corpus_case_count > 0u &&
      summary.canonical_type_form_conformance_corpus_passed_case_count ==
          summary.canonical_type_form_conformance_corpus_case_count &&
      summary.canonical_type_form_conformance_corpus_failed_case_count == 0u &&
      summary.canonical_type_form_conformance_corpus_consistent &&
      summary.canonical_type_form_conformance_corpus_ready &&
      !summary.canonical_type_form_conformance_corpus_key.empty() &&
      summary.canonical_type_form_performance_quality_required_guardrail_count >
          0u &&
      summary.canonical_type_form_performance_quality_passed_guardrail_count ==
          summary
              .canonical_type_form_performance_quality_required_guardrail_count &&
      summary.canonical_type_form_performance_quality_failed_guardrail_count ==
          0u &&
      summary.canonical_type_form_performance_quality_guardrails_consistent &&
      summary.canonical_type_form_performance_quality_guardrails_ready &&
      !summary.canonical_type_form_performance_quality_guardrails_key.empty() &&
      summary.param_id_spelling_sites <= summary.param_type_sites &&
      summary.param_class_spelling_sites <= summary.param_type_sites &&
      summary.param_sel_spelling_sites <= summary.param_type_sites &&
      summary.param_instancetype_spelling_sites <= summary.param_type_sites &&
      summary.param_object_pointer_type_sites <= summary.param_type_sites &&
      summary.return_id_spelling_sites <= summary.return_type_sites &&
      summary.return_class_spelling_sites <= summary.return_type_sites &&
      summary.return_sel_spelling_sites <= summary.return_type_sites &&
      summary.return_instancetype_spelling_sites <= summary.return_type_sites &&
      summary.return_object_pointer_type_sites <= summary.return_type_sites &&
      summary.property_id_spelling_sites <= summary.property_type_sites &&
      summary.property_class_spelling_sites <= summary.property_type_sites &&
      summary.property_sel_spelling_sites <= summary.property_type_sites &&
      summary.property_instancetype_spelling_sites <= summary.property_type_sites &&
      summary.property_object_pointer_type_sites <= summary.property_type_sites &&
      summary.param_id_spelling_sites + summary.param_class_spelling_sites + summary.param_sel_spelling_sites +
              summary.param_instancetype_spelling_sites + summary.param_object_pointer_type_sites <=
          summary.param_type_sites &&
      summary.return_id_spelling_sites + summary.return_class_spelling_sites + summary.return_sel_spelling_sites +
              summary.return_instancetype_spelling_sites + summary.return_object_pointer_type_sites <=
          summary.return_type_sites &&
      summary.property_id_spelling_sites + summary.property_class_spelling_sites + summary.property_sel_spelling_sites +
              summary.property_instancetype_spelling_sites + summary.property_object_pointer_type_sites <=
          summary.property_type_sites;
  return summary;
}
