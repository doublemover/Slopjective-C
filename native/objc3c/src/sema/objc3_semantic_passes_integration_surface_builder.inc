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

// lowering-handoff anchor: semantic integration plus type metadata
// handoff remain the canonical executable metadata graph source for typed
// lowering handoff packets and must stay deterministic before lowering claims
// readiness.
// typed-lowering anchor: the concrete lowering-ready metadata graph
// handoff continues to consume this semantic integration surface without
// reparsing syntax, so type/owner closure must remain deterministic here.
// debug-projection anchor: the lane-C manifest/IR inspection matrix
// replays this semantic integration surface verbatim before runtime section
// emission and runtime ingest packaging land.
// runtime-ingest packaging anchor: lane-D freezes one manifest
// transport boundary over this semantic integration surface before section
// emission/startup registration land, so later runtime ingest consumes the
// published graph rather than a reconstructed sema view.
Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(
    const Objc3ParsedProgram &program,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  Objc3SemanticIntegrationSurface surface;
  std::unordered_map<std::string, int> resolved_global_values;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
      protocol_definitions =
          BuildProtocolSemanticDefinitions(ast, arc_mode_enabled, diagnostics);
  const std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
      generic_definitions =
          BuildInterfaceGenericDefinitions(ast, protocol_definitions,
                                           diagnostics);
  std::unordered_set<std::string> generic_interface_names;
  for (const auto &entry : generic_definitions) {
    if (!entry.second.parameters.empty()) {
      generic_interface_names.insert(entry.first);
    }
  }
  ValidateInterfaceGenericSpecializations(ast, generic_definitions,
                                          protocol_definitions, diagnostics);
  ValidateProtocolCompositionIdentifierBindings(ast, protocol_definitions,
                                                diagnostics);
  // diagnostic precision anchor: class interface/implementation
  // summaries exclude category containers so valid class-plus-category
  // programs do not degrade into duplicate class-owner diagnostics before the
  // runtime metadata blocker computes attachment collisions and ambiguity.
  interface_implementation_summary.declared_interfaces = static_cast<std::size_t>(
      std::count_if(ast.interfaces.begin(), ast.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  interface_implementation_summary.declared_implementations =
      static_cast<std::size_t>(
          std::count_if(ast.implementations.begin(), ast.implementations.end(),
                        [](const Objc3ImplementationDecl &decl) {
                          return !decl.has_category;
                        }));

  const auto interface_container_label = [](bool has_category) {
    return has_category ? "category interface" : "interface";
  };
  const auto implementation_container_label = [](bool has_category) {
    return has_category ? "category implementation" : "implementation";
  };

  for (const auto &global : ast.globals) {
    const bool duplicate_global = surface.globals.find(global.name) != surface.globals.end();
    if (duplicate_global) {
      diagnostics.push_back(MakeDiag(global.line, global.column, "O3S200", "duplicate global '" + global.name + "'"));
    } else {
      surface.globals.emplace(global.name, ValueType::I32);
    }
    int value = 0;
    if (!EvaluateSemanticConstExpr(global.value.get(), value,
                                   &resolved_global_values)) {
      diagnostics.push_back(
          MakeDiag(global.line, global.column, "O3S210", "global initializer must be constant expression"));
    } else if (!duplicate_global) {
      resolved_global_values.emplace(global.name, value);
    }
  }

  for (const auto &fn : ast.functions) {
    if (surface.globals.find(fn.name) != surface.globals.end()) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S200", "duplicate function '" + fn.name + "'"));
      continue;
    }

    auto it = surface.functions.find(fn.name);
    if (it == surface.functions.end()) {
      FunctionInfo info;
      info.arity = fn.params.size();
      info.param_types.reserve(fn.params.size());
      info.param_canonical_types.reserve(fn.params.size());
      info.param_is_vector.reserve(fn.params.size());
      info.param_vector_base_spelling.reserve(fn.params.size());
      info.param_vector_lane_count.reserve(fn.params.size());
      info.param_has_generic_suffix.reserve(fn.params.size());
      info.param_has_pointer_declarator.reserve(fn.params.size());
      info.param_has_nullability_suffix.reserve(fn.params.size());
      info.param_has_ownership_qualifier.reserve(fn.params.size());
      info.param_id_spelling.reserve(fn.params.size());
      info.param_class_spelling.reserve(fn.params.size());
      info.param_instancetype_spelling.reserve(fn.params.size());
      info.param_object_pointer_type_spelling.reserve(fn.params.size());
      info.param_has_invalid_generic_suffix.reserve(fn.params.size());
      info.param_has_invalid_pointer_declarator.reserve(fn.params.size());
      info.param_has_invalid_nullability_suffix.reserve(fn.params.size());
      info.param_has_invalid_ownership_qualifier.reserve(fn.params.size());
      info.param_has_invalid_type_suffix.reserve(fn.params.size());
      info.param_ownership_insert_retain.reserve(fn.params.size());
      info.param_ownership_insert_release.reserve(fn.params.size());
      info.param_ownership_insert_autorelease.reserve(fn.params.size());
      info.param_ownership_is_weak_reference.reserve(fn.params.size());
      info.param_ownership_is_unowned_reference.reserve(fn.params.size());
      info.param_ownership_is_unowned_safe_reference.reserve(fn.params.size());
      info.param_ownership_arc_diagnostic_candidate.reserve(fn.params.size());
      info.param_ownership_arc_fixit_available.reserve(fn.params.size());
      info.param_ownership_arc_diagnostic_profile.reserve(fn.params.size());
      info.param_ownership_arc_fixit_hint.reserve(fn.params.size());
      info.param_has_protocol_composition.reserve(fn.params.size());
      info.param_protocol_composition_lexicographic.reserve(fn.params.size());
      info.param_has_invalid_protocol_composition.reserve(fn.params.size());
      for (const auto &param : fn.params) {
        const ProtocolCompositionInfo protocol_composition =
            BuildProtocolCompositionInfoFromParam(param,
                                                 &generic_interface_names);
        info.param_types.push_back(param.type);
        info.param_canonical_types.push_back(BuildCanonicalSemanticTypeFromParam(param));
        info.param_is_vector.push_back(param.vector_spelling);
        info.param_vector_base_spelling.push_back(param.vector_base_spelling);
        info.param_vector_lane_count.push_back(param.vector_lane_count);
        info.param_has_generic_suffix.push_back(param.has_generic_suffix);
        info.param_has_pointer_declarator.push_back(param.has_pointer_declarator);
        info.param_has_nullability_suffix.push_back(!param.nullability_suffix_tokens.empty());
        info.param_has_ownership_qualifier.push_back(param.has_ownership_qualifier);
        info.param_id_spelling.push_back(param.id_spelling);
        info.param_class_spelling.push_back(param.class_spelling);
        info.param_instancetype_spelling.push_back(param.instancetype_spelling);
        info.param_object_pointer_type_spelling.push_back(param.object_pointer_type_spelling);
        info.param_has_invalid_generic_suffix.push_back(HasInvalidGenericParamTypeSuffix(param));
        info.param_has_invalid_pointer_declarator.push_back(HasInvalidPointerParamTypeDeclarator(param));
        info.param_has_invalid_nullability_suffix.push_back(HasInvalidNullabilityParamTypeSuffix(param));
        info.param_has_invalid_ownership_qualifier.push_back(HasInvalidOwnershipQualifierParamTypeSuffix(param));
        info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
        info.param_ownership_insert_retain.push_back(param.ownership_insert_retain);
        info.param_ownership_insert_release.push_back(param.ownership_insert_release);
        info.param_ownership_insert_autorelease.push_back(param.ownership_insert_autorelease);
        info.param_ownership_is_weak_reference.push_back(param.ownership_is_weak_reference);
        info.param_ownership_is_unowned_reference.push_back(param.ownership_is_unowned_reference);
        info.param_ownership_is_unowned_safe_reference.push_back(param.ownership_is_unowned_safe_reference);
        info.param_ownership_arc_diagnostic_candidate.push_back(param.ownership_arc_diagnostic_candidate);
        info.param_ownership_arc_fixit_available.push_back(param.ownership_arc_fixit_available);
        info.param_ownership_arc_diagnostic_profile.push_back(param.ownership_arc_diagnostic_profile);
        info.param_ownership_arc_fixit_hint.push_back(param.ownership_arc_fixit_hint);
        info.param_has_protocol_composition.push_back(protocol_composition.has_protocol_composition);
        info.param_protocol_composition_lexicographic.push_back(protocol_composition.names_lexicographic);
        info.param_has_invalid_protocol_composition.push_back(protocol_composition.has_invalid_protocol_composition);
        const std::size_t param_index =
            info.param_has_ownership_qualifier.size() - 1u;
        const bool arc_inferred_object_param =
            arc_mode_enabled &&
            !info.param_has_ownership_qualifier[param_index] &&
            (info.param_id_spelling[param_index] ||
             info.param_instancetype_spelling[param_index] ||
             info.param_object_pointer_type_spelling[param_index]);
        if (arc_inferred_object_param) {
          // ARC inference/lifetime implementation anchor: function
          // parameters in the supported ARC slice now receive the same
          // canonical strong-owned retain/release profile as method
          // parameters, keeping one semantic contract for executable
          // unqualified object signatures.
          info.param_has_ownership_qualifier[param_index] = true;
          info.param_ownership_insert_retain[param_index] = true;
          info.param_ownership_insert_release[param_index] = true;
          info.param_ownership_insert_autorelease[param_index] = false;
          info.param_ownership_is_weak_reference[param_index] = false;
          info.param_ownership_is_unowned_reference[param_index] = false;
          info.param_ownership_is_unowned_safe_reference[param_index] = false;
          info.param_ownership_arc_diagnostic_candidate[param_index] = false;
          info.param_ownership_arc_fixit_available[param_index] = false;
          info.param_ownership_arc_diagnostic_profile[param_index].clear();
          info.param_ownership_arc_fixit_hint[param_index].clear();
        }
      }
      const ProtocolCompositionInfo return_protocol_composition =
          BuildProtocolCompositionInfoFromFunctionReturn(fn,
                                                        &generic_interface_names);
      info.return_has_generic_suffix = fn.has_return_generic_suffix;
      info.return_has_pointer_declarator = fn.has_return_pointer_declarator;
      info.return_has_nullability_suffix = !fn.return_nullability_suffix_tokens.empty();
      info.return_has_ownership_qualifier = fn.has_return_ownership_qualifier;
      info.return_id_spelling = fn.return_id_spelling;
      info.return_class_spelling = fn.return_class_spelling;
      info.return_instancetype_spelling = fn.return_instancetype_spelling;
      info.return_object_pointer_type_spelling = fn.return_object_pointer_type_spelling;
      info.return_has_invalid_generic_suffix = HasInvalidGenericReturnTypeSuffix(fn);
      info.return_has_invalid_pointer_declarator = HasInvalidPointerReturnTypeDeclarator(fn);
      info.return_has_invalid_nullability_suffix = HasInvalidNullabilityReturnTypeSuffix(fn);
      info.return_has_invalid_ownership_qualifier = HasInvalidOwnershipQualifierReturnTypeSuffix(fn);
      info.return_has_invalid_type_suffix = info.return_has_invalid_generic_suffix ||
                                            info.return_has_invalid_pointer_declarator ||
                                            info.return_has_invalid_nullability_suffix ||
                                            info.return_has_invalid_ownership_qualifier;
      info.return_ownership_insert_retain = fn.return_ownership_insert_retain;
      info.return_ownership_insert_release = fn.return_ownership_insert_release;
      info.return_ownership_insert_autorelease = fn.return_ownership_insert_autorelease;
      info.return_ownership_is_weak_reference = fn.return_ownership_is_weak_reference;
      info.return_ownership_is_unowned_reference = fn.return_ownership_is_unowned_reference;
      info.return_ownership_is_unowned_safe_reference = fn.return_ownership_is_unowned_safe_reference;
      info.return_ownership_arc_diagnostic_candidate = fn.return_ownership_arc_diagnostic_candidate;
      info.return_ownership_arc_fixit_available = fn.return_ownership_arc_fixit_available;
      info.return_ownership_arc_diagnostic_profile = fn.return_ownership_arc_diagnostic_profile;
      info.return_ownership_arc_fixit_hint = fn.return_ownership_arc_fixit_hint;
      info.return_type = fn.return_type;
      info.return_canonical_type = BuildCanonicalSemanticTypeFromFunctionReturn(fn);
      info.return_is_vector = fn.return_vector_spelling;
      info.return_vector_base_spelling = fn.return_vector_base_spelling;
      info.return_vector_lane_count = fn.return_vector_lane_count;
      info.return_has_protocol_composition = return_protocol_composition.has_protocol_composition;
      info.return_protocol_composition_lexicographic = return_protocol_composition.names_lexicographic;
      info.return_has_invalid_protocol_composition = return_protocol_composition.has_invalid_protocol_composition;
      const bool arc_inferred_object_return =
          arc_mode_enabled && !info.return_has_ownership_qualifier &&
          (info.return_id_spelling || info.return_instancetype_spelling ||
           info.return_object_pointer_type_spelling);
      if (arc_inferred_object_return) {
        // ARC inference/lifetime implementation anchor: function
        // returns in the supported ARC slice now receive the same canonical
        // strong-owned retain/release profile as method returns.
        info.return_has_ownership_qualifier = true;
        info.return_ownership_insert_retain = true;
        info.return_ownership_insert_release = true;
        info.return_ownership_insert_autorelease = false;
        info.return_ownership_is_weak_reference = false;
        info.return_ownership_is_unowned_reference = false;
        info.return_ownership_is_unowned_safe_reference = false;
        info.return_ownership_arc_diagnostic_candidate = false;
        info.return_ownership_arc_fixit_available = false;
        info.return_ownership_arc_diagnostic_profile.clear();
        info.return_ownership_arc_fixit_hint.clear();
      }
      info.async_continuation_profile_is_normalized =
          fn.async_continuation_profile_is_normalized;
      info.deterministic_async_continuation_handoff =
          fn.deterministic_async_continuation_handoff;
      info.async_continuation_sites = fn.async_continuation_sites;
      info.async_keyword_sites = fn.async_keyword_sites;
      info.async_function_sites = fn.async_function_sites;
      info.continuation_allocation_sites = fn.continuation_allocation_sites;
      info.continuation_resume_sites = fn.continuation_resume_sites;
      info.continuation_suspend_sites = fn.continuation_suspend_sites;
      info.async_state_machine_sites = fn.async_state_machine_sites;
      info.async_continuation_normalized_sites =
          fn.async_continuation_normalized_sites;
      info.async_continuation_gate_blocked_sites =
          fn.async_continuation_gate_blocked_sites;
      info.async_continuation_contract_violation_sites =
          fn.async_continuation_contract_violation_sites;
      info.actor_isolation_sendability_profile_is_normalized =
          fn.actor_isolation_sendability_profile_is_normalized;
      info.deterministic_actor_isolation_sendability_handoff =
          fn.deterministic_actor_isolation_sendability_handoff;
      info.actor_isolation_sendability_sites =
          fn.actor_isolation_sendability_sites;
      info.actor_isolation_decl_sites = fn.actor_isolation_decl_sites;
      info.actor_hop_sites = fn.actor_hop_sites;
      info.sendable_annotation_sites = fn.sendable_annotation_sites;
      info.non_sendable_crossing_sites = fn.non_sendable_crossing_sites;
      info.isolation_boundary_sites = fn.isolation_boundary_sites;
      info.actor_isolation_sendability_normalized_sites =
          fn.actor_isolation_sendability_normalized_sites;
      info.actor_isolation_sendability_gate_blocked_sites =
          fn.actor_isolation_sendability_gate_blocked_sites;
      info.actor_isolation_sendability_contract_violation_sites =
          fn.actor_isolation_sendability_contract_violation_sites;
      info.task_runtime_cancellation_profile_is_normalized =
          fn.task_runtime_cancellation_profile_is_normalized;
      info.deterministic_task_runtime_cancellation_handoff =
          fn.deterministic_task_runtime_cancellation_handoff;
      info.task_runtime_interop_sites = fn.task_runtime_interop_sites;
      info.runtime_hook_sites = fn.runtime_hook_sites;
      info.cancellation_check_sites = fn.cancellation_check_sites;
      info.cancellation_handler_sites = fn.cancellation_handler_sites;
      info.suspension_point_sites = fn.suspension_point_sites;
      info.cancellation_propagation_sites = fn.cancellation_propagation_sites;
      info.task_runtime_cancellation_normalized_sites =
          fn.task_runtime_cancellation_normalized_sites;
      info.task_runtime_cancellation_gate_blocked_sites =
          fn.task_runtime_cancellation_gate_blocked_sites;
      info.task_runtime_cancellation_contract_violation_sites =
          fn.task_runtime_cancellation_contract_violation_sites;
      info.concurrency_replay_race_guard_profile_is_normalized =
          fn.concurrency_replay_race_guard_profile_is_normalized;
      info.deterministic_concurrency_replay_race_guard_handoff =
          fn.deterministic_concurrency_replay_race_guard_handoff;
      info.concurrency_replay_race_guard_sites =
          fn.concurrency_replay_race_guard_sites;
      info.concurrency_replay_sites = fn.concurrency_replay_sites;
      info.replay_proof_sites = fn.replay_proof_sites;
      info.race_guard_sites = fn.race_guard_sites;
      info.task_handoff_sites = fn.task_handoff_sites;
      info.actor_isolation_sites = fn.actor_isolation_sites;
      info.deterministic_schedule_sites = fn.deterministic_schedule_sites;
      info.concurrency_replay_guard_blocked_sites =
          fn.concurrency_replay_guard_blocked_sites;
      info.concurrency_replay_contract_violation_sites =
          fn.concurrency_replay_contract_violation_sites;
      info.ns_error_bridging_profile_is_normalized =
          fn.ns_error_bridging_profile_is_normalized;
      info.deterministic_ns_error_bridging_lowering_handoff =
          fn.deterministic_ns_error_bridging_lowering_handoff;
      info.ns_error_bridging_sites = fn.ns_error_bridging_sites;
      info.ns_error_parameter_sites = fn.ns_error_parameter_sites;
      info.ns_error_out_parameter_sites = fn.ns_error_out_parameter_sites;
      info.ns_error_bridge_path_sites = fn.ns_error_bridge_path_sites;
      info.failable_call_sites = fn.failable_call_sites;
      info.ns_error_bridging_normalized_sites =
          fn.ns_error_bridging_normalized_sites;
      info.ns_error_bridge_boundary_sites = fn.ns_error_bridge_boundary_sites;
      info.ns_error_bridging_contract_violation_sites =
          fn.ns_error_bridging_contract_violation_sites;
      info.has_definition = !fn.is_prototype;
      info.is_pure_annotation = fn.is_pure;
      const bool has_macro_marker = fn.objc_macro_declared;
      const bool has_macro_package = fn.objc_macro_package_declared;
      const bool has_macro_provenance = fn.objc_macro_provenance_declared;
      if (has_macro_package || has_macro_provenance) {
        if (!has_macro_marker) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S321",
              "macro package/provenance markers require objc_macro on function '" +
                  fn.name + "'"));
        }
      }
      if (has_macro_marker) {
        if (!has_macro_package || !has_macro_provenance) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S320",
              "macro callable '" + fn.name +
                  "' requires both objc_macro_package and objc_macro_provenance"));
        }
        if (has_macro_package &&
            !IsSupportedMetaprogrammingMacroPackageName(fn.objc_macro_package_name)) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S322",
              "macro sandbox rejected package '" +
                  NormalizeMetaprogrammingRawMacroToken(fn.objc_macro_package_name) +
                  "' on function '" + fn.name + "'"));
        }
        if (has_macro_provenance &&
            !IsSupportedMetaprogrammingMacroProvenanceToken(
                fn.objc_macro_provenance_name)) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S323",
              "macro provenance must be a lowercase sha256 digest on function '" +
                  fn.name + "'"));
        }
        if (!fn.is_pure || fn.is_prototype || fn.async_declared ||
            fn.throws_declared) {
          diagnostics.push_back(MakeDiag(
              fn.line, fn.column, "O3S324",
              "macro callable '" + fn.name +
                  "' must be pure, body-backed, non-async, and non-throws"));
        }
      }
      surface.functions.emplace(fn.name, std::move(info));
      continue;
    }

    FunctionInfo &existing = it->second;
    bool compatible = existing.arity == fn.params.size() && existing.return_type == fn.return_type &&
                      existing.return_is_vector == fn.return_vector_spelling &&
                      existing.return_has_ownership_qualifier == fn.has_return_ownership_qualifier &&
                      existing.return_ownership_insert_retain == fn.return_ownership_insert_retain &&
                      existing.return_ownership_insert_release == fn.return_ownership_insert_release &&
                      existing.return_ownership_insert_autorelease == fn.return_ownership_insert_autorelease &&
                      existing.return_ownership_is_weak_reference == fn.return_ownership_is_weak_reference &&
                      existing.return_ownership_is_unowned_reference == fn.return_ownership_is_unowned_reference &&
                      existing.return_ownership_is_unowned_safe_reference ==
                          fn.return_ownership_is_unowned_safe_reference &&
                      existing.return_ownership_arc_diagnostic_candidate ==
                          fn.return_ownership_arc_diagnostic_candidate &&
                      existing.return_ownership_arc_fixit_available == fn.return_ownership_arc_fixit_available &&
                      existing.return_ownership_arc_diagnostic_profile ==
                          fn.return_ownership_arc_diagnostic_profile &&
                      existing.return_ownership_arc_fixit_hint == fn.return_ownership_arc_fixit_hint;
    if (compatible && existing.return_is_vector) {
      compatible = existing.return_vector_base_spelling == fn.return_vector_base_spelling &&
                   existing.return_vector_lane_count == fn.return_vector_lane_count;
    }
    const ProtocolCompositionInfo return_protocol_composition =
        BuildProtocolCompositionInfoFromFunctionReturn(fn,
                                                      &generic_interface_names);
    if (compatible && !AreEquivalentProtocolCompositions(existing.return_has_protocol_composition,
                                                         existing.return_protocol_composition_lexicographic,
                                                         return_protocol_composition.has_protocol_composition,
                                                         return_protocol_composition.names_lexicographic)) {
      compatible = false;
    }
    if (compatible) {
      for (std::size_t i = 0; i < fn.params.size(); ++i) {
        const ProtocolCompositionInfo param_protocol_composition =
            BuildProtocolCompositionInfoFromParam(fn.params[i],
                                                 &generic_interface_names);
        if (i >= existing.param_types.size() || i >= existing.param_is_vector.size() ||
            i >= existing.param_vector_base_spelling.size() || i >= existing.param_vector_lane_count.size() ||
            i >= existing.param_has_ownership_qualifier.size() ||
            i >= existing.param_ownership_insert_retain.size() ||
            i >= existing.param_ownership_insert_release.size() ||
            i >= existing.param_ownership_insert_autorelease.size() ||
            i >= existing.param_ownership_is_weak_reference.size() ||
            i >= existing.param_ownership_is_unowned_reference.size() ||
            i >= existing.param_ownership_is_unowned_safe_reference.size() ||
            i >= existing.param_ownership_arc_diagnostic_candidate.size() ||
            i >= existing.param_ownership_arc_fixit_available.size() ||
            i >= existing.param_ownership_arc_diagnostic_profile.size() ||
            i >= existing.param_ownership_arc_fixit_hint.size() ||
            i >= existing.param_has_protocol_composition.size() ||
            i >= existing.param_protocol_composition_lexicographic.size() ||
            existing.param_types[i] != fn.params[i].type ||
            existing.param_is_vector[i] != fn.params[i].vector_spelling) {
          compatible = false;
          break;
        }
        if (existing.param_has_ownership_qualifier[i] != fn.params[i].has_ownership_qualifier) {
          compatible = false;
          break;
        }
        if (existing.param_ownership_insert_retain[i] != fn.params[i].ownership_insert_retain ||
            existing.param_ownership_insert_release[i] != fn.params[i].ownership_insert_release ||
            existing.param_ownership_insert_autorelease[i] != fn.params[i].ownership_insert_autorelease ||
            existing.param_ownership_is_weak_reference[i] != fn.params[i].ownership_is_weak_reference ||
            existing.param_ownership_is_unowned_reference[i] != fn.params[i].ownership_is_unowned_reference ||
            existing.param_ownership_is_unowned_safe_reference[i] !=
                fn.params[i].ownership_is_unowned_safe_reference ||
            existing.param_ownership_arc_diagnostic_candidate[i] !=
                fn.params[i].ownership_arc_diagnostic_candidate ||
            existing.param_ownership_arc_fixit_available[i] !=
                fn.params[i].ownership_arc_fixit_available ||
            existing.param_ownership_arc_diagnostic_profile[i] !=
                fn.params[i].ownership_arc_diagnostic_profile ||
            existing.param_ownership_arc_fixit_hint[i] != fn.params[i].ownership_arc_fixit_hint) {
          compatible = false;
          break;
        }
        if (existing.param_is_vector[i] &&
            (existing.param_vector_base_spelling[i] != fn.params[i].vector_base_spelling ||
             existing.param_vector_lane_count[i] != fn.params[i].vector_lane_count)) {
          compatible = false;
          break;
        }
        if (!AreEquivalentProtocolCompositions(existing.param_has_protocol_composition[i],
                                               existing.param_protocol_composition_lexicographic[i],
                                               param_protocol_composition.has_protocol_composition,
                                               param_protocol_composition.names_lexicographic)) {
          compatible = false;
          break;
        }
      }
    }
    if (!compatible) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S206",
                                     "type mismatch: incompatible function signature for '" + fn.name + "'"));
      continue;
    }

    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_generic_suffix.size(); ++i) {
      existing.param_has_generic_suffix[i] =
          existing.param_has_generic_suffix[i] || fn.params[i].has_generic_suffix;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_pointer_declarator.size(); ++i) {
      existing.param_has_pointer_declarator[i] =
          existing.param_has_pointer_declarator[i] || fn.params[i].has_pointer_declarator;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_nullability_suffix.size(); ++i) {
      existing.param_has_nullability_suffix[i] =
          existing.param_has_nullability_suffix[i] || !fn.params[i].nullability_suffix_tokens.empty();
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_ownership_qualifier.size(); ++i) {
      existing.param_has_ownership_qualifier[i] =
          existing.param_has_ownership_qualifier[i] || fn.params[i].has_ownership_qualifier;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_id_spelling.size(); ++i) {
      existing.param_id_spelling[i] = existing.param_id_spelling[i] || fn.params[i].id_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_class_spelling.size(); ++i) {
      existing.param_class_spelling[i] = existing.param_class_spelling[i] || fn.params[i].class_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_instancetype_spelling.size(); ++i) {
      existing.param_instancetype_spelling[i] =
          existing.param_instancetype_spelling[i] || fn.params[i].instancetype_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_object_pointer_type_spelling.size(); ++i) {
      existing.param_object_pointer_type_spelling[i] =
          existing.param_object_pointer_type_spelling[i] || fn.params[i].object_pointer_type_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_generic_suffix.size(); ++i) {
      existing.param_has_invalid_generic_suffix[i] =
          existing.param_has_invalid_generic_suffix[i] || HasInvalidGenericParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_pointer_declarator.size(); ++i) {
      existing.param_has_invalid_pointer_declarator[i] =
          existing.param_has_invalid_pointer_declarator[i] || HasInvalidPointerParamTypeDeclarator(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_nullability_suffix.size(); ++i) {
      existing.param_has_invalid_nullability_suffix[i] =
          existing.param_has_invalid_nullability_suffix[i] || HasInvalidNullabilityParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_ownership_qualifier.size(); ++i) {
      existing.param_has_invalid_ownership_qualifier[i] =
          existing.param_has_invalid_ownership_qualifier[i] ||
          HasInvalidOwnershipQualifierParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_type_suffix.size(); ++i) {
      existing.param_has_invalid_type_suffix[i] =
          existing.param_has_invalid_type_suffix[i] || HasInvalidParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_protocol_composition.size(); ++i) {
      const ProtocolCompositionInfo param_protocol_composition =
          BuildProtocolCompositionInfoFromParam(fn.params[i],
                                               &generic_interface_names);
      existing.param_has_invalid_protocol_composition[i] =
          existing.param_has_invalid_protocol_composition[i] || param_protocol_composition.has_invalid_protocol_composition;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_canonical_types.size(); ++i) {
      existing.param_canonical_types[i] = BuildCanonicalSemanticTypeFromParam(fn.params[i]);
    }
    existing.return_has_generic_suffix = existing.return_has_generic_suffix || fn.has_return_generic_suffix;
    existing.return_has_pointer_declarator = existing.return_has_pointer_declarator || fn.has_return_pointer_declarator;
    existing.return_has_nullability_suffix =
        existing.return_has_nullability_suffix || !fn.return_nullability_suffix_tokens.empty();
    existing.return_has_ownership_qualifier =
        existing.return_has_ownership_qualifier || fn.has_return_ownership_qualifier;
    existing.return_id_spelling = existing.return_id_spelling || fn.return_id_spelling;
    existing.return_class_spelling = existing.return_class_spelling || fn.return_class_spelling;
    existing.return_instancetype_spelling =
        existing.return_instancetype_spelling || fn.return_instancetype_spelling;
    existing.return_object_pointer_type_spelling =
        existing.return_object_pointer_type_spelling || fn.return_object_pointer_type_spelling;
    existing.return_has_invalid_generic_suffix =
        existing.return_has_invalid_generic_suffix || HasInvalidGenericReturnTypeSuffix(fn);
    existing.return_has_invalid_pointer_declarator =
        existing.return_has_invalid_pointer_declarator || HasInvalidPointerReturnTypeDeclarator(fn);
    existing.return_has_invalid_nullability_suffix =
        existing.return_has_invalid_nullability_suffix || HasInvalidNullabilityReturnTypeSuffix(fn);
    existing.return_has_invalid_ownership_qualifier =
        existing.return_has_invalid_ownership_qualifier || HasInvalidOwnershipQualifierReturnTypeSuffix(fn);
    existing.return_has_invalid_type_suffix = existing.return_has_invalid_type_suffix ||
                                             existing.return_has_invalid_generic_suffix ||
                                             existing.return_has_invalid_pointer_declarator ||
                                             existing.return_has_invalid_nullability_suffix ||
                                             existing.return_has_invalid_ownership_qualifier;
    existing.return_has_invalid_protocol_composition =
        existing.return_has_invalid_protocol_composition || return_protocol_composition.has_invalid_protocol_composition;
    existing.return_canonical_type =
        BuildCanonicalSemanticTypeFromFunctionReturn(fn);
    existing.async_continuation_profile_is_normalized =
        existing.async_continuation_profile_is_normalized ||
        fn.async_continuation_profile_is_normalized;
    existing.deterministic_async_continuation_handoff =
        existing.deterministic_async_continuation_handoff ||
        fn.deterministic_async_continuation_handoff;
    existing.async_continuation_sites =
        std::max(existing.async_continuation_sites, fn.async_continuation_sites);
    existing.async_keyword_sites =
        std::max(existing.async_keyword_sites, fn.async_keyword_sites);
    existing.async_function_sites =
        std::max(existing.async_function_sites, fn.async_function_sites);
    existing.continuation_allocation_sites = std::max(
        existing.continuation_allocation_sites, fn.continuation_allocation_sites);
    existing.continuation_resume_sites = std::max(
        existing.continuation_resume_sites, fn.continuation_resume_sites);
    existing.continuation_suspend_sites = std::max(
        existing.continuation_suspend_sites, fn.continuation_suspend_sites);
    existing.async_state_machine_sites = std::max(
        existing.async_state_machine_sites, fn.async_state_machine_sites);
    existing.async_continuation_normalized_sites = std::max(
        existing.async_continuation_normalized_sites,
        fn.async_continuation_normalized_sites);
    existing.async_continuation_gate_blocked_sites = std::max(
        existing.async_continuation_gate_blocked_sites,
        fn.async_continuation_gate_blocked_sites);
    existing.async_continuation_contract_violation_sites = std::max(
        existing.async_continuation_contract_violation_sites,
        fn.async_continuation_contract_violation_sites);
    existing.actor_isolation_sendability_profile_is_normalized =
        existing.actor_isolation_sendability_profile_is_normalized ||
        fn.actor_isolation_sendability_profile_is_normalized;
    existing.deterministic_actor_isolation_sendability_handoff =
        existing.deterministic_actor_isolation_sendability_handoff ||
        fn.deterministic_actor_isolation_sendability_handoff;
    existing.actor_isolation_sendability_sites = std::max(
        existing.actor_isolation_sendability_sites,
        fn.actor_isolation_sendability_sites);
    existing.actor_isolation_decl_sites = std::max(
        existing.actor_isolation_decl_sites, fn.actor_isolation_decl_sites);
    existing.actor_hop_sites =
        std::max(existing.actor_hop_sites, fn.actor_hop_sites);
    existing.sendable_annotation_sites = std::max(
        existing.sendable_annotation_sites, fn.sendable_annotation_sites);
    existing.non_sendable_crossing_sites = std::max(
        existing.non_sendable_crossing_sites, fn.non_sendable_crossing_sites);
    existing.isolation_boundary_sites = std::max(
        existing.isolation_boundary_sites, fn.isolation_boundary_sites);
    existing.actor_isolation_sendability_normalized_sites = std::max(
        existing.actor_isolation_sendability_normalized_sites,
        fn.actor_isolation_sendability_normalized_sites);
    existing.actor_isolation_sendability_gate_blocked_sites = std::max(
        existing.actor_isolation_sendability_gate_blocked_sites,
        fn.actor_isolation_sendability_gate_blocked_sites);
    existing.actor_isolation_sendability_contract_violation_sites = std::max(
        existing.actor_isolation_sendability_contract_violation_sites,
        fn.actor_isolation_sendability_contract_violation_sites);
    existing.task_runtime_cancellation_profile_is_normalized =
        existing.task_runtime_cancellation_profile_is_normalized ||
        fn.task_runtime_cancellation_profile_is_normalized;
    existing.deterministic_task_runtime_cancellation_handoff =
        existing.deterministic_task_runtime_cancellation_handoff ||
        fn.deterministic_task_runtime_cancellation_handoff;
    existing.task_runtime_interop_sites = std::max(
        existing.task_runtime_interop_sites, fn.task_runtime_interop_sites);
    existing.runtime_hook_sites =
        std::max(existing.runtime_hook_sites, fn.runtime_hook_sites);
    existing.cancellation_check_sites = std::max(
        existing.cancellation_check_sites, fn.cancellation_check_sites);
    existing.cancellation_handler_sites = std::max(
        existing.cancellation_handler_sites, fn.cancellation_handler_sites);
    existing.suspension_point_sites = std::max(
        existing.suspension_point_sites, fn.suspension_point_sites);
    existing.cancellation_propagation_sites = std::max(
        existing.cancellation_propagation_sites,
        fn.cancellation_propagation_sites);
    existing.task_runtime_cancellation_normalized_sites = std::max(
        existing.task_runtime_cancellation_normalized_sites,
        fn.task_runtime_cancellation_normalized_sites);
    existing.task_runtime_cancellation_gate_blocked_sites = std::max(
        existing.task_runtime_cancellation_gate_blocked_sites,
        fn.task_runtime_cancellation_gate_blocked_sites);
    existing.task_runtime_cancellation_contract_violation_sites = std::max(
        existing.task_runtime_cancellation_contract_violation_sites,
        fn.task_runtime_cancellation_contract_violation_sites);
    existing.concurrency_replay_race_guard_profile_is_normalized =
        existing.concurrency_replay_race_guard_profile_is_normalized ||
        fn.concurrency_replay_race_guard_profile_is_normalized;
    existing.deterministic_concurrency_replay_race_guard_handoff =
        existing.deterministic_concurrency_replay_race_guard_handoff ||
        fn.deterministic_concurrency_replay_race_guard_handoff;
    existing.concurrency_replay_race_guard_sites = std::max(
        existing.concurrency_replay_race_guard_sites,
        fn.concurrency_replay_race_guard_sites);
    existing.concurrency_replay_sites =
        std::max(existing.concurrency_replay_sites, fn.concurrency_replay_sites);
    existing.replay_proof_sites =
        std::max(existing.replay_proof_sites, fn.replay_proof_sites);
    existing.race_guard_sites =
        std::max(existing.race_guard_sites, fn.race_guard_sites);
    existing.task_handoff_sites =
        std::max(existing.task_handoff_sites, fn.task_handoff_sites);
    existing.actor_isolation_sites =
        std::max(existing.actor_isolation_sites, fn.actor_isolation_sites);
    existing.deterministic_schedule_sites = std::max(
        existing.deterministic_schedule_sites, fn.deterministic_schedule_sites);
    existing.concurrency_replay_guard_blocked_sites = std::max(
        existing.concurrency_replay_guard_blocked_sites,
        fn.concurrency_replay_guard_blocked_sites);
    existing.concurrency_replay_contract_violation_sites = std::max(
        existing.concurrency_replay_contract_violation_sites,
        fn.concurrency_replay_contract_violation_sites);
    existing.ns_error_bridging_profile_is_normalized =
        existing.ns_error_bridging_profile_is_normalized ||
        fn.ns_error_bridging_profile_is_normalized;
    existing.deterministic_ns_error_bridging_lowering_handoff =
        existing.deterministic_ns_error_bridging_lowering_handoff ||
        fn.deterministic_ns_error_bridging_lowering_handoff;
    existing.ns_error_bridging_sites =
        std::max(existing.ns_error_bridging_sites, fn.ns_error_bridging_sites);
    existing.ns_error_parameter_sites = std::max(existing.ns_error_parameter_sites,
                                                 fn.ns_error_parameter_sites);
    existing.ns_error_out_parameter_sites = std::max(
        existing.ns_error_out_parameter_sites, fn.ns_error_out_parameter_sites);
    existing.ns_error_bridge_path_sites = std::max(
        existing.ns_error_bridge_path_sites, fn.ns_error_bridge_path_sites);
    existing.failable_call_sites =
        std::max(existing.failable_call_sites, fn.failable_call_sites);
    existing.ns_error_bridging_normalized_sites = std::max(
        existing.ns_error_bridging_normalized_sites,
        fn.ns_error_bridging_normalized_sites);
    existing.ns_error_bridge_boundary_sites =
        std::max(existing.ns_error_bridge_boundary_sites,
                 fn.ns_error_bridge_boundary_sites);
    existing.ns_error_bridging_contract_violation_sites = std::max(
        existing.ns_error_bridging_contract_violation_sites,
        fn.ns_error_bridging_contract_violation_sites);
    existing.is_pure_annotation = existing.is_pure_annotation || fn.is_pure;

    if (!fn.is_prototype) {
      if (existing.has_definition) {
        diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S200", "duplicate function '" + fn.name + "'"));
      } else {
        existing.has_definition = true;
      }
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    const bool is_category_interface = interface_decl.has_category;
    const std::string container_name = FormatObjcContainerName(
        interface_decl.name, interface_decl.has_category,
        interface_decl.category_name);
    const std::string container_label =
        interface_container_label(is_category_interface);

    auto interface_it = surface.interfaces.end();
    if (!is_category_interface) {
      interface_it = surface.interfaces.find(interface_decl.name);
    }
    if (!is_category_interface && interface_it != surface.interfaces.end()) {
      diagnostics.push_back(
          MakeDiag(interface_decl.line, interface_decl.column, "O3S200", "duplicate interface '" + interface_decl.name + "'"));
      continue;
    }

    Objc3InterfaceInfo interface_info;
    // completeness anchor: sema preserves deterministic interface
    // super-name and implementation-pairing inputs for class/metaclass graph
    // materialization in the frontend pipeline.
    // completion anchor: sema must preserve protocol/category/member
    // declaration inventories without reordering so export graph closure stays
    // deterministic for protocol/category/property/ivar packets.
    // freeze anchor: the same deterministic inventories remain the
    // semantic-consistency boundary inputs for fail-closed lane-B admission.
    // accessor legality anchor: interface/category-interface
    // property containers also own one fail-closed effective accessor selector
    // namespace so later executable accessors never inherit ambiguous getter
    // or setter bindings from source.
    interface_info.super_name = interface_decl.super_name;
    interface_info.adopted_protocols_lexicographic =
        BuildSortedUniqueStringsLocal(interface_decl.adopted_protocols);
    for (const auto &generic_param : interface_decl.generic_params) {
      interface_info.generic_parameter_names_source_order.push_back(
          generic_param.name);
      interface_info.generic_parameter_variance_source_order.push_back(
          generic_param.variance_spelling);
    }
    interface_info.objc_direct_members_declared =
        interface_decl.objc_direct_members_declared;
    interface_info.objc_final_declared = interface_decl.objc_final_declared;
    interface_info.objc_sealed_declared = interface_decl.objc_sealed_declared;
    if (interface_decl.objc_derive_declared) {
      const std::string derive_spelling =
          NormalizeMetaprogrammingRawDeriveToken(interface_decl.objc_derive_name);
      const std::string canonical_derive_name =
          NormalizeMetaprogrammingSupportedDeriveName(derive_spelling);
      if (is_category_interface) {
        diagnostics.push_back(MakeDiag(
            interface_decl.line, interface_decl.column, "O3S318",
            "derive expansion is only supported on primary interfaces, not category interface '" +
                container_name + "'"));
      } else if (canonical_derive_name.empty()) {
        diagnostics.push_back(MakeDiag(
            interface_decl.line, interface_decl.column, "O3S317",
            "unsupported derive '" + derive_spelling +
                "' on interface '" + container_name + "'"));
      } else {
        const std::string derived_selector =
            BuildMetaprogrammingDerivedSelectorForInventoryRow(canonical_derive_name);
        if (!derived_selector.empty()) {
          const bool selector_conflict = std::any_of(
              interface_decl.methods.begin(), interface_decl.methods.end(),
              [&derived_selector](const Objc3MethodDecl &method_decl) {
                return BuildMethodSelectorNormalizationContractInfo(method_decl)
                           .normalized_selector == derived_selector;
              });
          if (selector_conflict) {
            diagnostics.push_back(MakeDiag(
                interface_decl.line, interface_decl.column, "O3S319",
                "derive expansion selector conflict: interface '" +
                    container_name + "' already declares selector '" +
                    derived_selector + "'"));
          }
        }
      }
    }
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        getter_owner_states;
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        setter_owner_states;
    for (const auto &property_decl : interface_decl.properties) {
      ValidatePropertyTypeSuffixes(property_decl, container_name,
                                   container_label, diagnostics,
                                   &generic_interface_names);
      Objc3PropertyInfo property_info =
          BuildPropertyInfo(property_decl, container_name, container_label,
                            arc_mode_enabled,
                            diagnostics);
      const auto property_insert = interface_info.properties.emplace(property_decl.name, std::move(property_info));
      if (!property_insert.second) {
        diagnostics.push_back(
            MakeDiag(property_decl.line,
                     property_decl.column,
                     "O3S200",
                     "duplicate " + container_label + " property '" +
                         property_decl.name + "' in " + container_label +
                         " '" + container_name + "'"));
      } else {
        ValidatePropertyAccessorSelectorUniqueness(
            property_insert.first->second,
            property_decl.name,
            container_name,
            container_label,
            interface_info.properties,
            getter_owner_states,
            setter_owner_states,
            diagnostics);
      }
    }

    for (const auto &method_decl : interface_decl.methods) {
      const MethodSelectorNormalizationContractInfo selector_contract =
          BuildMethodSelectorNormalizationContractInfo(method_decl);
      ValidateMethodSelectorNormalizationContract(method_decl, container_name,
                                                 container_label,
                                                 selector_contract,
                                                 diagnostics);
      ValidateMethodReturnTypeSuffixes(method_decl, container_name,
                                       container_label, diagnostics,
                                       &generic_interface_names);
      ValidateMethodParameterTypeSuffixes(method_decl, container_name,
                                          container_label, diagnostics,
                                          &generic_interface_names);
      if (method_decl.objc_macro_declared ||
          method_decl.objc_macro_package_declared ||
          method_decl.objc_macro_provenance_declared) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S325",
            "macro expansion is currently limited to free functions; " +
                container_label + " selector in '" + container_name +
                "' is not sandbox-admitted"));
      }

      const std::string selector = selector_contract.normalized_selector;
      if (method_decl.has_body) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: " + container_label +
                                           " selector '" + selector + "' in '" +
                                           container_name +
                                           "' must not define a body"));
      }

      const auto method_insert =
          interface_info.methods.emplace(
              selector, BuildMethodInfo(method_decl, selector_contract,
                                        arc_mode_enabled,
                                        &generic_interface_names));
      if (!method_insert.second) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S200",
                                       "duplicate " + container_label +
                                           " selector '" + selector + "' in " +
                                           container_label + " '" +
                                           container_name + "'"));
        continue;
      }

      method_insert.first->second.effective_direct_dispatch =
          IsDispatchEffectivelyDirectMethod(method_insert.first->second,
                                         interface_info);

      if (!is_category_interface) {
        ++interface_implementation_summary.interface_method_symbols;
      }
    }

    if (!is_category_interface) {
      surface.interfaces.emplace(interface_decl.name, std::move(interface_info));
    } else {
      const std::string category_owner_identity =
          BuildCategoryOwnerIdentity(interface_decl.name,
                                     interface_decl.category_name);
      if (!surface.category_interfaces
               .emplace(category_owner_identity, std::move(interface_info))
               .second) {
        diagnostics.push_back(MakeDiag(interface_decl.line,
                                       interface_decl.column,
                                       "O3S200",
                                       "duplicate category interface '" +
                                           container_name + "'"));
      }
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    const bool is_category_implementation = implementation_decl.has_category;
    const std::string container_name = FormatObjcContainerName(
        implementation_decl.name, implementation_decl.has_category,
        implementation_decl.category_name);
    const std::string container_label =
        implementation_container_label(is_category_implementation);

    auto implementation_it = surface.implementations.end();
    if (!is_category_implementation) {
      implementation_it = surface.implementations.find(implementation_decl.name);
    }
    if (!is_category_implementation &&
        implementation_it != surface.implementations.end()) {
      diagnostics.push_back(MakeDiag(implementation_decl.line, implementation_decl.column, "O3S200",
                                     "duplicate implementation '" + implementation_decl.name + "'"));
      continue;
    }

    Objc3ImplementationInfo implementation_info;
    const Objc3InterfaceInfo *matched_interface = nullptr;
    if (!is_category_implementation) {
      const auto interface_it = surface.interfaces.find(implementation_decl.name);
      if (interface_it == surface.interfaces.end()) {
        diagnostics.push_back(MakeDiag(implementation_decl.line, implementation_decl.column, "O3S206",
                                       "type mismatch: missing interface declaration for implementation '" +
                                           implementation_decl.name + "'"));
      } else {
        implementation_info.has_matching_interface = true;
        matched_interface = &interface_it->second;
      }
    } else {
      const std::string category_owner_identity =
          BuildCategoryOwnerIdentity(implementation_decl.name,
                                     implementation_decl.category_name);
      const auto interface_it =
          surface.category_interfaces.find(category_owner_identity);
      if (interface_it != surface.category_interfaces.end()) {
        implementation_info.has_matching_interface = true;
        matched_interface = &interface_it->second;
      }
    }

    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        getter_owner_states;
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        setter_owner_states;
    for (const auto &property_decl : implementation_decl.properties) {
      ValidatePropertyTypeSuffixes(property_decl, container_name,
                                   container_label, diagnostics,
                                   &generic_interface_names);
      Objc3PropertyInfo property_info =
          BuildPropertyInfo(property_decl, container_name, container_label,
                            arc_mode_enabled,
                            diagnostics);
      const auto property_insert =
          implementation_info.properties.emplace(property_decl.name,
                                                std::move(property_info));
      if (!property_insert.second) {
        diagnostics.push_back(MakeDiag(
            property_decl.line, property_decl.column, "O3S200",
            "duplicate " + container_label + " property '" +
                property_decl.name + "' in " + container_label + " '" +
                container_name + "'"));
        continue;
      }
      ValidatePropertyAccessorSelectorUniqueness(
          property_insert.first->second,
          property_decl.name,
          container_name,
          container_label,
          implementation_info.properties,
          getter_owner_states,
          setter_owner_states,
          diagnostics);

      if (matched_interface == nullptr) {
        continue;
      }

      const auto interface_property_it =
          matched_interface->properties.find(property_decl.name);
      if (interface_property_it == matched_interface->properties.end()) {
        diagnostics.push_back(MakeDiag(property_decl.line,
                                       property_decl.column,
                                       "O3S206",
                                       "type mismatch: implementation property '" + property_decl.name + "' in '" +
                                           implementation_decl.name + "' is not declared in interface"));
        continue;
      }
      if (!IsCompatiblePropertySignature(interface_property_it->second, property_insert.first->second)) {
        if (!EmitImplementationPropertyReflectionAccessorCompatibilityDiagnostics(
                interface_property_it->second,
                property_insert.first->second,
                property_decl.name,
                implementation_decl.name,
                diagnostics)) {
          diagnostics.push_back(MakeDiag(
              property_decl.line,
              property_decl.column,
              "O3S206",
              "type mismatch: incompatible property signature for '" +
                  property_decl.name + "' in implementation '" +
                  implementation_decl.name + "'"));
        }
        continue;
      }
      OverlayAuthoritativeDefaultIvarBinding(property_insert.first->second,
                                             interface_property_it->second);
    }

    // property-synthesis enforcement anchor: matched class
    // implementations resolve default ivar bindings from interface-declared
    // properties first, then fold in optional implementation redeclarations
    // without making redeclaration a prerequisite for authoritative binding.
    if (!is_category_implementation && matched_interface != nullptr) {
      for (const auto &interface_property_entry : matched_interface->properties) {
        const Objc3PropertyInfo &interface_property = interface_property_entry.second;
        if (!HasResolvedDefaultIvarBinding(interface_property)) {
          diagnostics.push_back(MakeDiag(
              implementation_decl.line,
              implementation_decl.column,
              "O3S206",
              "type mismatch: property synthesis for '" +
                  interface_property_entry.first + "' in implementation '" +
                  implementation_decl.name +
                  "' lacks an authoritative default ivar binding"));
          continue;
        }

        const Objc3PropertyInfo *implementation_property =
            FindPropertyInImplementationInfo(implementation_info,
                                            interface_property_entry.first);
        if (implementation_property != nullptr &&
            (!HasResolvedDefaultIvarBinding(*implementation_property) ||
             !HasCompatibleResolvedDefaultIvarBinding(interface_property,
                                                     *implementation_property))) {
          diagnostics.push_back(MakeDiag(
              implementation_decl.line,
              implementation_decl.column,
              "O3S206",
              "type mismatch: property synthesis for '" +
                  interface_property_entry.first + "' in implementation '" +
                  implementation_decl.name +
                  "' drifted from the interface default ivar binding"));
        }
      }
    }

    for (const auto &method_decl : implementation_decl.methods) {
      const MethodSelectorNormalizationContractInfo selector_contract =
          BuildMethodSelectorNormalizationContractInfo(method_decl);
      ValidateMethodSelectorNormalizationContract(method_decl, container_name,
                                                 container_label,
                                                 selector_contract,
                                                 diagnostics);
      ValidateMethodReturnTypeSuffixes(method_decl, container_name,
                                       container_label, diagnostics,
                                       &generic_interface_names);
      ValidateMethodParameterTypeSuffixes(method_decl, container_name,
                                          container_label, diagnostics,
                                          &generic_interface_names);
      if (method_decl.objc_macro_declared ||
          method_decl.objc_macro_package_declared ||
          method_decl.objc_macro_provenance_declared) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S325",
            "macro expansion is currently limited to free functions; " +
                container_label + " selector in '" + container_name +
                "' is not sandbox-admitted"));
      }

      const std::string selector = selector_contract.normalized_selector;
      if (!method_decl.has_body) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: " + container_label +
                                           " selector '" + selector + "' in '" +
