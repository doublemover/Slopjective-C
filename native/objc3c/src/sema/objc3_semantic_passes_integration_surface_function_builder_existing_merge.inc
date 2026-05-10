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
