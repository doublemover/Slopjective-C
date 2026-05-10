void PopulateSemanticIntegrationSurfaceFunctions(
    const Objc3Program &ast,
    Objc3SemanticIntegrationSurface &surface,
    const std::unordered_set<std::string> &generic_interface_names,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics) {
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
}
