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

Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary
BuildMetaprogrammingMacroSafetySandboxDeterminismSummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary summary;

  const auto count_diagnostic = [&diagnostics](std::string_view code,
                                               std::string_view needle) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code, needle](const std::string &entry) {
          return entry.find(code) != std::string::npos &&
                 entry.find(needle) != std::string::npos;
        }));
  };

  const auto accumulate_function = [&summary](const FunctionDecl &fn) {
    const bool has_macro_marker = fn.objc_macro_declared;
    const bool has_macro_package = fn.objc_macro_package_declared;
    const bool has_macro_provenance = fn.objc_macro_provenance_declared;
    if (has_macro_marker) {
      ++summary.macro_marker_sites;
    }
    if (has_macro_package) {
      ++summary.macro_package_sites;
    }
    if (has_macro_provenance) {
      ++summary.macro_provenance_sites;
    }
    if (has_macro_marker && has_macro_package && has_macro_provenance) {
      ++summary.expansion_visible_macro_sites;
    }

    if (!has_macro_marker && !(has_macro_package || has_macro_provenance)) {
      return;
    }
    if (!has_macro_marker && (has_macro_package || has_macro_provenance)) {
      ++summary.orphan_macro_metadata_sites;
      return;
    }
    if (!has_macro_package || !has_macro_provenance) {
      ++summary.incomplete_macro_metadata_sites;
      return;
    }
    if (!IsSupportedMetaprogrammingMacroPackageName(fn.objc_macro_package_name)) {
      ++summary.invalid_package_sites;
      return;
    }
    if (!IsSupportedMetaprogrammingMacroProvenanceToken(fn.objc_macro_provenance_name)) {
      ++summary.invalid_provenance_sites;
      return;
    }
    if (!fn.is_pure || fn.is_prototype || fn.async_declared ||
        fn.throws_declared) {
      ++summary.nondeterministic_callable_sites;
      return;
    }
    ++summary.safe_macro_callable_sites;
  };

  const auto accumulate_method = [&summary](const Objc3MethodDecl &method) {
    if (method.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
    if (method.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (method.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (method.objc_macro_declared && method.objc_macro_package_declared &&
        method.objc_macro_provenance_declared) {
      ++summary.expansion_visible_macro_sites;
    }
    if (method.objc_macro_declared || method.objc_macro_package_declared ||
        method.objc_macro_provenance_declared) {
      ++summary.unsupported_callable_topology_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_function(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_method(method_decl);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      accumulate_method(method_decl);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_method(method_decl);
    }
  }

  summary.semantic_dependency_required =
      IsReadyObjc3MetaprogrammingDeriveExpansionInventorySummary(dependency_summary);
  summary.metadata_completeness_enforced =
      summary.incomplete_macro_metadata_sites ==
          count_diagnostic("O3S320", "requires both objc_macro_package and objc_macro_provenance") &&
      summary.orphan_macro_metadata_sites ==
          count_diagnostic("O3S321", "macro package/provenance markers require objc_macro");
  summary.sandbox_namespace_enforced =
      summary.invalid_package_sites ==
      count_diagnostic("O3S322", "macro sandbox rejected package");
  summary.provenance_determinism_enforced =
      summary.invalid_provenance_sites ==
      count_diagnostic("O3S323", "macro provenance must be a lowercase sha256 digest");
  summary.callable_determinism_enforced =
      summary.nondeterministic_callable_sites ==
          count_diagnostic("O3S324", "must be pure, body-backed, non-async, and non-throws") &&
      summary.unsupported_callable_topology_sites ==
          count_diagnostic("O3S325", "not sandbox-admitted");
  summary.macro_execution_deferred = true;
  summary.deterministic =
      summary.semantic_dependency_required &&
      summary.metadata_completeness_enforced &&
      summary.sandbox_namespace_enforced &&
      summary.provenance_determinism_enforced &&
      summary.callable_determinism_enforced &&
      summary.safe_macro_callable_sites <= summary.expansion_visible_macro_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "metaprogramming macro safety/sandbox/determinism semantics must remain deterministic and fail closed";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << dependency_summary.contract_id
      << ";macro-sites=" << summary.macro_marker_sites << ":"
      << summary.macro_package_sites << ":" << summary.macro_provenance_sites
      << ":" << summary.expansion_visible_macro_sites
      << ";safe=" << summary.safe_macro_callable_sites
      << ";incomplete=" << summary.incomplete_macro_metadata_sites
      << ";orphan=" << summary.orphan_macro_metadata_sites
      << ";invalid-package=" << summary.invalid_package_sites
      << ";invalid-provenance=" << summary.invalid_provenance_sites
      << ";nondeterministic=" << summary.nondeterministic_callable_sites
      << ";unsupported-topology=" << summary.unsupported_callable_topology_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummary(
    const Objc3Program &program,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary summary;

  const auto count_diagnostic = [&diagnostics](std::string_view code,
                                               std::string_view needle) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code, needle](const std::string &entry) {
          return entry.find(code) != std::string::npos &&
                 entry.find(needle) != std::string::npos;
        }));
  };
  const auto property_is_object_like =
      [](const Objc3PropertyDecl &property_decl) {
        return property_decl.id_spelling || property_decl.instancetype_spelling ||
               property_decl.object_pointer_type_spelling ||
               property_decl.type == ValueType::ObjCId ||
               property_decl.type == ValueType::ObjCClass ||
               property_decl.type == ValueType::ObjCInstancetype ||
               property_decl.type == ValueType::ObjCObjectPtr;
      };
  const auto accumulate_property =
      [&](const Objc3PropertyDecl &property_decl, std::string_view owner_kind) {
        if (!property_decl.property_behavior_declared) {
          return;
        }
        ++summary.property_behavior_sites;
        const std::string canonical_behavior =
            NormalizeMetaprogrammingSupportedPropertyBehaviorName(
                property_decl.property_behavior_name);
        if (canonical_behavior.empty()) {
          ++summary.unsupported_behavior_sites;
          return;
        }
        ++summary.supported_behavior_sites;
        if (canonical_behavior == "Observed") {
          ++summary.observed_behavior_sites;
          if (owner_kind == "protocol") {
            ++summary.observed_on_protocol_sites;
          }
          if (property_decl.is_readonly ||
              !property_decl.effective_setter_available) {
            ++summary.observed_readonly_conflict_sites;
          }
        } else if (canonical_behavior == "Projected") {
          ++summary.projected_behavior_sites;
          if (!property_decl.is_readonly || property_decl.has_setter ||
              property_decl.effective_setter_available) {
            ++summary.projected_writable_conflict_sites;
          }
        }
        if (!property_is_object_like(property_decl)) {
          ++summary.non_object_behavior_sites;
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &property_decl : interface_decl.properties) {
      accumulate_property(property_decl, interface_decl.has_category
                                             ? "category-interface"
                                             : "interface");
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property_decl : protocol_decl.properties) {
      accumulate_property(property_decl, "protocol");
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &property_decl : implementation_decl.properties) {
      accumulate_property(property_decl, "implementation");
    }
  }

  summary.semantic_dependency_required =
      IsReadyObjc3MetaprogrammingMacroSafetySandboxDeterminismSummary(
          dependency_summary);
  summary.supported_behavior_inventory_landed =
      summary.supported_behavior_sites ==
          summary.observed_behavior_sites + summary.projected_behavior_sites &&
      summary.supported_behavior_sites + summary.unsupported_behavior_sites ==
          summary.property_behavior_sites;
  summary.unsupported_behavior_fail_closed =
      summary.unsupported_behavior_sites ==
      count_diagnostic("O3S326", "unsupported property behavior");
  summary.owner_topology_fail_closed =
      summary.observed_on_protocol_sites ==
      count_diagnostic("O3S328",
                       "observed property behavior requires a concrete interface or implementation property");
  summary.interaction_legality_fail_closed =
      summary.observed_readonly_conflict_sites ==
          count_diagnostic("O3S329",
                           "observed property behavior requires writable setter-visible storage") &&
      summary.projected_writable_conflict_sites ==
          count_diagnostic("O3S330",
                           "projected property behavior requires a readonly getter-only property");
  summary.storage_legality_fail_closed =
      summary.non_object_behavior_sites ==
      count_diagnostic("O3S327",
                       "requires an Objective-C object property");
  summary.runtime_materialization_deferred = true;
  summary.deterministic =
      summary.semantic_dependency_required &&
      summary.supported_behavior_inventory_landed &&
      summary.unsupported_behavior_fail_closed &&
      summary.owner_topology_fail_closed &&
      summary.interaction_legality_fail_closed &&
      summary.storage_legality_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "metaprogramming property behavior legality and interaction semantics must remain deterministic and fail closed";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << dependency_summary.contract_id
      << ";behavior-sites=" << summary.property_behavior_sites << ":"
      << summary.supported_behavior_sites << ":"
      << summary.unsupported_behavior_sites
      << ";supported-behaviors=" << summary.observed_behavior_sites << ":"
      << summary.projected_behavior_sites
      << ";compatibility-conflicts=" << summary.observed_on_protocol_sites
      << ":" << summary.observed_readonly_conflict_sites << ":"
      << summary.projected_writable_conflict_sites << ":"
      << summary.non_object_behavior_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

template <typename CallableDecl>
static void AccumulateErrorHandlingErrorBridgeLegalityForCallable(
    const CallableDecl &decl, const Objc3Program &program,
    Objc3ErrorHandlingErrorBridgeLegalitySummary &summary,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics) {
  const bool has_nserror = decl.objc_nserror_declared;
  const bool has_status_code = decl.objc_status_code_declared;
  const bool has_marker = has_nserror || has_status_code;
  const bool has_profile_bridge_sites =
      decl.ns_error_bridging_profile_is_normalized &&
      decl.ns_error_bridging_sites != 0u;
  const bool has_bridge_surface =
      has_marker || has_profile_bridge_sites;
  if (!has_bridge_surface) {
    return;
  }

  ++summary.bridge_callable_sites;
  if (has_nserror) {
    ++summary.objc_nserror_callable_sites;
  }
  if (has_status_code) {
    ++summary.objc_status_code_callable_sites;
  }

  const bool has_error_out = CallableHasNSErrorOutParameterForErrorHandling(decl);
  const bool valid_nserror_return =
      CallableReturnsSupportedNSErrorBoolForErrorHandling(decl);
  const bool valid_status_return =
      CallableReturnsSupportedStatusCodeForErrorHandling(decl);
  const bool throws_conflict = has_marker && decl.throws_declared;
  const bool marker_conflict = has_nserror && has_status_code;
  const bool valid_error_type =
      !has_status_code ||
      objc3c::support::LowercaseAscii(
          decl.objc_status_code_error_type_spelling) ==
          "nserror";
  const FunctionDecl *mapping =
      has_status_code
          ? FindFunctionDeclByName(program, decl.objc_status_code_mapping_symbol)
          : nullptr;
  const bool mapping_exists = !has_status_code || mapping != nullptr;
  const bool mapping_signature_valid =
      !has_status_code ||
      (mapping != nullptr && mapping->params.size() == 1u &&
       mapping->params.front().type == decl.return_type &&
       CallableReturnsNSErrorObjectForErrorHandling(*mapping));

  if (!allow_source_only_error_runtime_surface && has_marker) {
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S285",
        "unsupported feature claim: NSError/status bridge legality is not yet runnable in Objective-C 3 native mode",
        diagnostics);
    ++summary.unsupported_combination_sites;
  }
  if (has_nserror && !has_error_out) {
    ++summary.missing_error_out_parameter_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S275",
        "objc_nserror requires an NSError out parameter",
        diagnostics);
  }
  if (has_nserror && !valid_nserror_return) {
    ++summary.invalid_nserror_return_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S276",
        "objc_nserror currently requires a BOOL-like success return",
        diagnostics);
  }
  if (throws_conflict) {
    ++summary.throws_bridge_conflict_sites;
    ++summary.unsupported_combination_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S277",
        "NSError/status bridge markers cannot currently be combined with throws",
        diagnostics);
  }
  if (marker_conflict) {
    ++summary.marker_conflict_sites;
    ++summary.unsupported_combination_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S278",
        "objc_nserror and objc_status_code cannot appear on the same callable",
        diagnostics);
  }
  if (has_status_code && !has_error_out) {
    ++summary.missing_error_out_parameter_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S279",
        "objc_status_code requires an NSError out parameter",
        diagnostics);
  }
  if (has_status_code && !valid_error_type) {
    ++summary.invalid_error_type_sites;
    ++summary.unsupported_combination_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S280",
        "objc_status_code currently requires error_type: NSError",
        diagnostics);
  }
  if (has_status_code && !valid_status_return) {
    ++summary.invalid_status_return_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S281",
        "objc_status_code requires a BOOL-like or integer status return",
        diagnostics);
  }
  if (has_status_code && !mapping_exists) {
    ++summary.missing_mapping_symbol_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S282",
        "objc_status_code mapping symbol must resolve to a declared function",
        diagnostics);
  }
  if (has_status_code && mapping_exists && !mapping_signature_valid) {
    ++summary.invalid_mapping_signature_sites;
    RecordErrorHandlingSemanticDiagnostic(
        summary.contract_violation_sites, decl.line, decl.column, "O3S283",
        "objc_status_code mapping function must accept one matching status parameter and return NSError",
        diagnostics);
  }

  if (CallableHasSemanticallyValidNSErrorStatusBridge(decl, program)) {
    ++summary.semantically_valid_bridge_callable_sites;
    ++summary.try_eligible_bridge_callable_sites;
  }
}

Objc3ErrorHandlingErrorBridgeLegalitySummary BuildErrorHandlingErrorBridgeLegalitySummary(
    const Objc3Program &program,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics) {
  Objc3ErrorHandlingErrorBridgeLegalitySummary summary;
  for (const auto &fn : program.functions) {
    AccumulateErrorHandlingErrorBridgeLegalityForCallable(
        fn, program, summary, allow_source_only_error_runtime_surface,
        diagnostics);
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      AccumulateErrorHandlingErrorBridgeLegalityForCallable(
          method, program, summary, allow_source_only_error_runtime_surface,
          diagnostics);
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      AccumulateErrorHandlingErrorBridgeLegalityForCallable(
          method, program, summary, allow_source_only_error_runtime_surface,
          diagnostics);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      AccumulateErrorHandlingErrorBridgeLegalityForCallable(
          method, program, summary, allow_source_only_error_runtime_surface,
          diagnostics);
    }
  }

  summary.bridge_legality_landed =
      summary.semantically_valid_bridge_callable_sites <=
          summary.bridge_callable_sites &&
      summary.objc_nserror_callable_sites <= summary.bridge_callable_sites &&
      summary.objc_status_code_callable_sites <= summary.bridge_callable_sites;
  summary.try_bridge_filter_landed =
      summary.try_eligible_bridge_callable_sites ==
      summary.semantically_valid_bridge_callable_sites;
  summary.unsupported_combinations_fail_closed =
      summary.unsupported_combination_sites <= summary.contract_violation_sites;
  summary.deterministic =
      summary.bridge_legality_landed && summary.try_bridge_filter_landed &&
      summary.unsupported_combinations_fail_closed &&
      summary.contract_violation_sites <=
          summary.bridge_callable_sites + summary.unsupported_combination_sites;
  summary.ready_for_lowering_and_runtime = false;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";bridges=" << summary.bridge_callable_sites
      << ";valid=" << summary.semantically_valid_bridge_callable_sites
      << ";unsupported=" << summary.unsupported_combination_sites
      << ";violations=" << summary.contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

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

static void AccumulatePropertyAttributeSummaryFromPropertyInfo(const Objc3PropertyInfo &property,
                                                               Objc3PropertyAttributeSummary &summary) {
  ++summary.properties_total;
  summary.attribute_entries += property.attribute_entries;
  if (property.is_readonly) {
    ++summary.readonly_modifiers;
  }
  if (property.is_readwrite) {
    ++summary.readwrite_modifiers;
  }
  if (property.is_atomic) {
    ++summary.atomic_modifiers;
  }
  if (property.is_nonatomic) {
    ++summary.nonatomic_modifiers;
  }
  if (property.is_copy) {
    ++summary.copy_modifiers;
  }
  if (property.is_strong || property.is_retain) {
    ++summary.strong_modifiers;
  }
  if (property.is_weak) {
    ++summary.weak_modifiers;
  }
  if (property.is_assign || property.is_unsafe_unretained) {
    ++summary.assign_modifiers;
  }
  if (property.has_getter) {
    ++summary.getter_modifiers;
  }
  if (property.has_setter) {
    ++summary.setter_modifiers;
  }
  summary.invalid_attribute_entries += property.invalid_attribute_entries;
  summary.property_contract_violations += property.property_contract_violations;

  if (property.attribute_entries != property.attribute_names_lexicographic.size() ||
      !std::is_sorted(property.attribute_names_lexicographic.begin(), property.attribute_names_lexicographic.end())) {
    summary.deterministic = false;
  }
  if (property.has_readwrite_conflict != (property.is_readonly && property.is_readwrite)) {
    summary.deterministic = false;
  }
  if (property.has_atomicity_conflict != (property.is_atomic && property.is_nonatomic)) {
    summary.deterministic = false;
  }
  const std::size_t ownership_modifiers =
      (property.is_copy ? 1u : 0u) +
      ((property.is_strong || property.is_retain) ? 1u : 0u) +
      (property.is_weak ? 1u : 0u) +
      (property.is_unowned ? 1u : 0u) +
      ((property.is_assign || property.is_unsafe_unretained) ? 1u : 0u);
  if (property.has_ownership_conflict != (ownership_modifiers > 1u)) {
    summary.deterministic = false;
  }
  if (property.has_weak_unowned_conflict !=
      (property.is_weak && (property.is_unowned || property.is_unsafe_unretained))) {
    summary.deterministic = false;
  }
  if (property.has_setter && property.setter_selector.empty()) {
    summary.deterministic = false;
  }
  if (property.has_getter && property.getter_selector.empty()) {
    summary.deterministic = false;
  }
  const bool expected_invalid_contract =
      property.has_unknown_attribute || property.has_duplicate_attribute || property.has_readwrite_conflict ||
      property.has_atomicity_conflict || property.has_ownership_conflict ||
      property.has_weak_unowned_conflict ||
      property.has_accessor_selector_contract_violation || property.invalid_attribute_entries > 0 ||
      property.property_contract_violations > 0;
  if (property.has_invalid_attribute_contract != expected_invalid_contract) {
    summary.deterministic = false;
  }
}

static Objc3PropertyAttributeSummary BuildPropertyAttributeSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3PropertyAttributeSummary summary;
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &property_entry : interface_entry.second.properties) {
      AccumulatePropertyAttributeSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &property_entry : implementation_entry.second.properties) {
      AccumulatePropertyAttributeSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.invalid_attribute_entries <= summary.attribute_entries &&
                          summary.getter_modifiers <= summary.properties_total &&
                          summary.setter_modifiers <= summary.properties_total;
  return summary;
}

static void AccumulateTypeAnnotationSummaryFromFunctionInfo(const FunctionInfo &function_info,
                                                            Objc3TypeAnnotationSurfaceSummary &summary) {
  const std::size_t arity = function_info.arity;
  if (function_info.param_has_generic_suffix.size() != arity ||
      function_info.param_has_pointer_declarator.size() != arity ||
      function_info.param_has_nullability_suffix.size() != arity ||
      function_info.param_has_ownership_qualifier.size() != arity ||
      function_info.param_id_spelling.size() != arity ||
      function_info.param_class_spelling.size() != arity ||
      function_info.param_instancetype_spelling.size() != arity ||
      function_info.param_object_pointer_type_spelling.size() != arity ||
      function_info.param_has_invalid_generic_suffix.size() != arity ||
      function_info.param_has_invalid_pointer_declarator.size() != arity ||
      function_info.param_has_invalid_nullability_suffix.size() != arity ||
      function_info.param_has_invalid_ownership_qualifier.size() != arity ||
      function_info.param_has_invalid_type_suffix.size() != arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < arity; ++i) {
    if (function_info.param_has_generic_suffix[i]) {
      ++summary.generic_suffix_sites;
    }
    if (function_info.param_has_pointer_declarator[i]) {
      ++summary.pointer_declarator_sites;
    }
    if (function_info.param_has_nullability_suffix[i]) {
      ++summary.nullability_suffix_sites;
    }
    if (function_info.param_has_ownership_qualifier[i]) {
      ++summary.ownership_qualifier_sites;
    }
    if (IsObjCReferenceAnnotationSite(function_info.param_id_spelling[i],
                                      function_info.param_class_spelling[i],
                                      function_info.param_instancetype_spelling[i],
                                      function_info.param_object_pointer_type_spelling[i])) {
      ++summary.object_pointer_type_sites;
    }
    if (function_info.param_has_invalid_generic_suffix[i]) {
      ++summary.invalid_generic_suffix_sites;
    }
    if (function_info.param_has_invalid_pointer_declarator[i]) {
      ++summary.invalid_pointer_declarator_sites;
    }
    if (function_info.param_has_invalid_nullability_suffix[i]) {
      ++summary.invalid_nullability_suffix_sites;
    }
    if (function_info.param_has_invalid_ownership_qualifier[i]) {
      ++summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_invalid =
        function_info.param_has_invalid_generic_suffix[i] ||
        function_info.param_has_invalid_pointer_declarator[i] ||
        function_info.param_has_invalid_nullability_suffix[i] ||
        function_info.param_has_invalid_ownership_qualifier[i];
    if (function_info.param_has_invalid_type_suffix[i] != expected_invalid) {
      summary.deterministic = false;
    }
  }

  if (function_info.return_has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (function_info.return_has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (function_info.return_has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (function_info.return_has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (IsObjCReferenceAnnotationSite(function_info.return_id_spelling,
                                    function_info.return_class_spelling,
                                    function_info.return_instancetype_spelling,
                                    function_info.return_object_pointer_type_spelling)) {
    ++summary.object_pointer_type_sites;
  }
  if (function_info.return_has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (function_info.return_has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (function_info.return_has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (function_info.return_has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }

  const bool expected_return_invalid = function_info.return_has_invalid_generic_suffix ||
                                       function_info.return_has_invalid_pointer_declarator ||
                                       function_info.return_has_invalid_nullability_suffix ||
                                       function_info.return_has_invalid_ownership_qualifier;
  if (function_info.return_has_invalid_type_suffix != expected_return_invalid) {
    summary.deterministic = false;
  }
}

static void AccumulateTypeAnnotationSummaryFromMethodInfo(const Objc3MethodInfo &method_info,
                                                          Objc3TypeAnnotationSurfaceSummary &summary) {
  const std::size_t arity = method_info.arity;
  if (method_info.param_has_generic_suffix.size() != arity ||
      method_info.param_has_pointer_declarator.size() != arity ||
      method_info.param_has_nullability_suffix.size() != arity ||
      method_info.param_has_ownership_qualifier.size() != arity ||
      method_info.param_id_spelling.size() != arity ||
      method_info.param_class_spelling.size() != arity ||
      method_info.param_instancetype_spelling.size() != arity ||
      method_info.param_object_pointer_type_spelling.size() != arity ||
      method_info.param_has_invalid_generic_suffix.size() != arity ||
      method_info.param_has_invalid_pointer_declarator.size() != arity ||
      method_info.param_has_invalid_nullability_suffix.size() != arity ||
      method_info.param_has_invalid_ownership_qualifier.size() != arity ||
      method_info.param_has_invalid_type_suffix.size() != arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < arity; ++i) {
    if (method_info.param_has_generic_suffix[i]) {
      ++summary.generic_suffix_sites;
    }
    if (method_info.param_has_pointer_declarator[i]) {
      ++summary.pointer_declarator_sites;
    }
    if (method_info.param_has_nullability_suffix[i]) {
      ++summary.nullability_suffix_sites;
    }
    if (method_info.param_has_ownership_qualifier[i]) {
      ++summary.ownership_qualifier_sites;
    }
    if (IsObjCReferenceAnnotationSite(method_info.param_id_spelling[i],
                                      method_info.param_class_spelling[i],
                                      method_info.param_instancetype_spelling[i],
                                      method_info.param_object_pointer_type_spelling[i])) {
      ++summary.object_pointer_type_sites;
    }
    if (method_info.param_has_invalid_generic_suffix[i]) {
      ++summary.invalid_generic_suffix_sites;
    }
    if (method_info.param_has_invalid_pointer_declarator[i]) {
      ++summary.invalid_pointer_declarator_sites;
    }
    if (method_info.param_has_invalid_nullability_suffix[i]) {
      ++summary.invalid_nullability_suffix_sites;
    }
    if (method_info.param_has_invalid_ownership_qualifier[i]) {
      ++summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_invalid = method_info.param_has_invalid_generic_suffix[i] ||
                                  method_info.param_has_invalid_pointer_declarator[i] ||
                                  method_info.param_has_invalid_nullability_suffix[i] ||
                                  method_info.param_has_invalid_ownership_qualifier[i];
    if (method_info.param_has_invalid_type_suffix[i] != expected_invalid) {
      summary.deterministic = false;
    }
  }

  if (method_info.return_has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (method_info.return_has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (method_info.return_has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (method_info.return_has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (IsObjCReferenceAnnotationSite(method_info.return_id_spelling,
                                    method_info.return_class_spelling,
                                    method_info.return_instancetype_spelling,
                                    method_info.return_object_pointer_type_spelling)) {
    ++summary.object_pointer_type_sites;
  }
  if (method_info.return_has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (method_info.return_has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (method_info.return_has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (method_info.return_has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }
  const bool expected_return_invalid = method_info.return_has_invalid_generic_suffix ||
                                       method_info.return_has_invalid_pointer_declarator ||
                                       method_info.return_has_invalid_nullability_suffix ||
                                       method_info.return_has_invalid_ownership_qualifier;
  if (method_info.return_has_invalid_type_suffix != expected_return_invalid) {
    summary.deterministic = false;
  }
}

static void AccumulateTypeAnnotationSummaryFromPropertyInfo(const Objc3PropertyInfo &property_info,
                                                            Objc3TypeAnnotationSurfaceSummary &summary) {
  if (property_info.has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (property_info.has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (property_info.has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (property_info.has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (IsObjCReferenceAnnotationSite(property_info.id_spelling,
                                    property_info.class_spelling,
                                    property_info.instancetype_spelling,
                                    property_info.object_pointer_type_spelling)) {
    ++summary.object_pointer_type_sites;
  }
  if (property_info.has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (property_info.has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (property_info.has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (property_info.has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }
  const bool expected_invalid = property_info.has_invalid_generic_suffix ||
                                property_info.has_invalid_pointer_declarator ||
                                property_info.has_invalid_nullability_suffix ||
                                property_info.has_invalid_ownership_qualifier;
  if (property_info.has_invalid_type_suffix != expected_invalid) {
    summary.deterministic = false;
  }
}

static Objc3TypeAnnotationSurfaceSummary BuildTypeAnnotationSurfaceSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3TypeAnnotationSurfaceSummary summary;
  for (const auto &function_entry : surface.functions) {
    AccumulateTypeAnnotationSummaryFromFunctionInfo(function_entry.second, summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateTypeAnnotationSummaryFromMethodInfo(method_entry.second, summary);
    }
    for (const auto &property_entry : interface_entry.second.properties) {
      AccumulateTypeAnnotationSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateTypeAnnotationSummaryFromMethodInfo(method_entry.second, summary);
    }
    for (const auto &property_entry : implementation_entry.second.properties) {
      AccumulateTypeAnnotationSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.invalid_generic_suffix_sites <= summary.generic_suffix_sites &&
                          summary.invalid_pointer_declarator_sites <= summary.pointer_declarator_sites &&
                          summary.invalid_nullability_suffix_sites <= summary.nullability_suffix_sites &&
                          summary.invalid_ownership_qualifier_sites <= summary.ownership_qualifier_sites &&
                          summary.invalid_type_annotation_sites() <= summary.total_type_annotation_sites();
  return summary;
}

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
