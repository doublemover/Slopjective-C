static void ApplyTypeFormScaffoldSummaryToIdClassSelObjectPointerTypeCheckingSummary(
    Objc3IdClassSelObjectPointerTypeCheckingSummary &summary) {
  const Objc3TypeFormScaffoldSummary scaffold = BuildObjc3TypeFormScaffoldSummary();
  summary.canonical_reference_form_count = scaffold.canonical_reference_form_count;
  summary.canonical_message_scalar_form_count = scaffold.canonical_message_scalar_form_count;
  summary.canonical_bridge_top_form_count = scaffold.canonical_bridge_top_form_count;
  summary.canonical_reference_forms_unique = scaffold.canonical_reference_forms_unique;
  summary.canonical_message_scalar_forms_unique = scaffold.canonical_message_scalar_forms_unique;
  summary.canonical_bridge_top_forms_unique = scaffold.canonical_bridge_top_forms_unique;
  summary.canonical_bridge_top_subset_of_reference = scaffold.canonical_bridge_top_subset_of_reference;
  summary.canonical_type_form_scaffold_ready = IsReadyObjc3TypeFormScaffoldSummary(scaffold);
  summary.canonical_type_form_diagnostics_hardening_consistent = scaffold.diagnostics_hardening_consistent;
  summary.canonical_type_form_diagnostics_hardening_ready = scaffold.diagnostics_hardening_ready;
  summary.canonical_type_form_diagnostics_hardening_key = scaffold.diagnostics_hardening_key;
  summary.canonical_type_form_recovery_determinism_consistent = scaffold.recovery_determinism_consistent;
  summary.canonical_type_form_recovery_determinism_ready = scaffold.recovery_determinism_ready;
  summary.canonical_type_form_recovery_determinism_key = scaffold.recovery_determinism_key;
  summary.canonical_type_form_conformance_matrix_consistent = scaffold.conformance_matrix_consistent;
  summary.canonical_type_form_conformance_matrix_ready = scaffold.conformance_matrix_ready;
  summary.canonical_type_form_conformance_matrix_key = scaffold.conformance_matrix_key;
  summary.canonical_type_form_conformance_corpus_case_count = scaffold.conformance_corpus_case_count;
  summary.canonical_type_form_conformance_corpus_passed_case_count = scaffold.conformance_corpus_passed_case_count;
  summary.canonical_type_form_conformance_corpus_failed_case_count = scaffold.conformance_corpus_failed_case_count;
  summary.canonical_type_form_conformance_corpus_consistent = scaffold.conformance_corpus_consistent;
  summary.canonical_type_form_conformance_corpus_ready = scaffold.conformance_corpus_ready;
  summary.canonical_type_form_conformance_corpus_key = scaffold.conformance_corpus_key;
  summary.canonical_type_form_performance_quality_required_guardrail_count =
      scaffold.performance_quality_required_guardrail_count;
  summary.canonical_type_form_performance_quality_passed_guardrail_count =
      scaffold.performance_quality_passed_guardrail_count;
  summary.canonical_type_form_performance_quality_failed_guardrail_count =
      scaffold.performance_quality_failed_guardrail_count;
  summary.canonical_type_form_performance_quality_guardrails_consistent =
      scaffold.performance_quality_guardrails_consistent;
  summary.canonical_type_form_performance_quality_guardrails_ready =
      scaffold.performance_quality_guardrails_ready;
  summary.canonical_type_form_performance_quality_guardrails_key =
      scaffold.performance_quality_guardrails_key;
}

static Objc3IdClassSelObjectPointerTypeCheckingSummary
BuildIdClassSelObjectPointerTypeCheckingSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3IdClassSelObjectPointerTypeCheckingSummary summary;
  ApplyTypeFormScaffoldSummaryToIdClassSelObjectPointerTypeCheckingSummary(summary);

  const auto accumulate_function = [&summary](const FunctionInfo &info) {
    ++summary.return_type_sites;
    if (info.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (info.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (info.return_type == ValueType::ObjCSel) {
      ++summary.return_sel_spelling_sites;
    }
    if (info.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (info.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += info.param_types.size();
    const std::size_t count = std::min(
        info.param_types.size(),
        std::min(info.param_id_spelling.size(),
                 std::min(info.param_class_spelling.size(),
                          std::min(info.param_instancetype_spelling.size(),
                                   info.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (info.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (info.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (info.param_types[i] == ValueType::ObjCSel) {
        ++summary.param_sel_spelling_sites;
      }
      if (info.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (info.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != info.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_method = [&summary](const Objc3MethodInfo &info) {
    ++summary.return_type_sites;
    if (info.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (info.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (info.return_type == ValueType::ObjCSel) {
      ++summary.return_sel_spelling_sites;
    }
    if (info.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (info.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += info.param_types.size();
    const std::size_t count = std::min(
        info.param_types.size(),
        std::min(info.param_id_spelling.size(),
                 std::min(info.param_class_spelling.size(),
                          std::min(info.param_instancetype_spelling.size(),
                                   info.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (info.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (info.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (info.param_types[i] == ValueType::ObjCSel) {
        ++summary.param_sel_spelling_sites;
      }
      if (info.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (info.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != info.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_property = [&summary](const Objc3PropertyInfo &info) {
    ++summary.property_type_sites;
    if (info.id_spelling) {
      ++summary.property_id_spelling_sites;
    }
    if (info.class_spelling) {
      ++summary.property_class_spelling_sites;
    }
    if (info.type == ValueType::ObjCSel) {
      ++summary.property_sel_spelling_sites;
    }
    if (info.instancetype_spelling) {
      ++summary.property_instancetype_spelling_sites;
    }
    if (info.object_pointer_type_spelling) {
      ++summary.property_object_pointer_type_sites;
    }
  };

  for (const auto &entry : surface.functions) {
    accumulate_function(entry.second);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
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
