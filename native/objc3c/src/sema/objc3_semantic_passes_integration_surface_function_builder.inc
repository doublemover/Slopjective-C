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

#include "sema/objc3_semantic_passes_integration_surface_function_builder_existing_merge.inc"
  }
}
