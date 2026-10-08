    std::vector<std::string> selectors;
    selectors.reserve(implementation_it->second.methods.size());
    for (const auto &method_entry : implementation_it->second.methods) {
      selectors.push_back(method_entry.first);
    }
    std::sort(selectors.begin(), selectors.end());

    metadata.methods_lexicographic.reserve(selectors.size());
    for (const std::string &selector : selectors) {
      const auto method_it = implementation_it->second.methods.find(selector);
      if (method_it == implementation_it->second.methods.end()) {
        continue;
      }
      const Objc3MethodInfo &source = method_it->second;
      Objc3SemanticMethodTypeMetadata method_metadata;
      method_metadata.selector = selector;
      method_metadata.selector_normalized = source.selector_normalized;
      method_metadata.selector_piece_count = source.selector_piece_count;
      method_metadata.selector_parameter_piece_count = source.selector_parameter_piece_count;
      method_metadata.selector_contract_normalized = source.selector_contract_normalized;
      method_metadata.selector_had_pieceless_form = source.selector_had_pieceless_form;
      method_metadata.selector_has_spelling_mismatch = source.selector_has_spelling_mismatch;
      method_metadata.selector_has_arity_mismatch = source.selector_has_arity_mismatch;
      method_metadata.selector_has_parameter_linkage_mismatch = source.selector_has_parameter_linkage_mismatch;
      method_metadata.selector_has_normalization_flag_mismatch = source.selector_has_normalization_flag_mismatch;
      method_metadata.selector_has_missing_piece_keyword = source.selector_has_missing_piece_keyword;
      method_metadata.arity = source.arity;
      method_metadata.throws_declared = source.throws_declared;
      method_metadata.typed_throws_declared = source.typed_throws_declared;
      method_metadata.typed_throws_error_type_spelling =
          source.typed_throws_error_type_spelling;
      method_metadata.typed_throws_effect_signature_key =
          source.typed_throws_effect_signature_key;
      method_metadata.typed_throws_callable_compatibility_policy =
          source.typed_throws_callable_compatibility_policy;
      method_metadata.typed_throws_abi_lowering_ready =
          source.typed_throws_abi_lowering_ready;
      method_metadata.generic_parameter_names_source_order =
          source.generic_parameter_names_source_order;
      method_metadata.generic_parameter_variance_source_order =
          source.generic_parameter_variance_source_order;
      method_metadata.generic_parameter_constraints_lexicographic =
          source.generic_parameter_constraints_lexicographic;
      method_metadata.generic_callable_signature_replay_key =
          source.generic_callable_signature_replay_key;
      method_metadata.generic_callable_reification_policy =
          source.generic_callable_reification_policy;
      method_metadata.generic_callable_mangling_policy_id =
          source.generic_callable_mangling_policy_id;
      method_metadata.generic_callable_contract_deterministic =
          source.generic_callable_contract_deterministic;
      method_metadata.param_types = source.param_types;
      method_metadata.param_canonical_types = source.param_canonical_types;
      method_metadata.param_is_vector = source.param_is_vector;
      method_metadata.param_vector_base_spelling = source.param_vector_base_spelling;
      method_metadata.param_vector_lane_count = source.param_vector_lane_count;
      method_metadata.param_has_generic_suffix = source.param_has_generic_suffix;
      method_metadata.param_has_pointer_declarator = source.param_has_pointer_declarator;
      method_metadata.param_has_nullability_suffix = source.param_has_nullability_suffix;
      method_metadata.param_has_ownership_qualifier = source.param_has_ownership_qualifier;
      method_metadata.param_id_spelling = source.param_id_spelling;
      method_metadata.param_class_spelling = source.param_class_spelling;
      method_metadata.param_instancetype_spelling = source.param_instancetype_spelling;
      method_metadata.param_object_pointer_type_spelling = source.param_object_pointer_type_spelling;
      method_metadata.param_has_invalid_generic_suffix = source.param_has_invalid_generic_suffix;
      method_metadata.param_has_invalid_pointer_declarator = source.param_has_invalid_pointer_declarator;
      method_metadata.param_has_invalid_nullability_suffix = source.param_has_invalid_nullability_suffix;
      method_metadata.param_has_invalid_ownership_qualifier = source.param_has_invalid_ownership_qualifier;
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.param_ownership_insert_retain = source.param_ownership_insert_retain;
      method_metadata.param_ownership_insert_release = source.param_ownership_insert_release;
      method_metadata.param_ownership_insert_autorelease = source.param_ownership_insert_autorelease;
      method_metadata.param_ownership_is_weak_reference = source.param_ownership_is_weak_reference;
      method_metadata.param_ownership_is_unowned_reference = source.param_ownership_is_unowned_reference;
      method_metadata.param_ownership_is_unowned_safe_reference = source.param_ownership_is_unowned_safe_reference;
      method_metadata.param_ownership_arc_diagnostic_candidate = source.param_ownership_arc_diagnostic_candidate;
      method_metadata.param_ownership_arc_fixit_available = source.param_ownership_arc_fixit_available;
      method_metadata.param_ownership_arc_diagnostic_profile = source.param_ownership_arc_diagnostic_profile;
      method_metadata.param_ownership_arc_fixit_hint = source.param_ownership_arc_fixit_hint;
      method_metadata.param_has_protocol_composition = source.param_has_protocol_composition;
      method_metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
      method_metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
      method_metadata.return_has_generic_suffix = source.return_has_generic_suffix;
      method_metadata.return_has_pointer_declarator = source.return_has_pointer_declarator;
      method_metadata.return_has_nullability_suffix = source.return_has_nullability_suffix;
      method_metadata.return_has_ownership_qualifier = source.return_has_ownership_qualifier;
      method_metadata.return_id_spelling = source.return_id_spelling;
      method_metadata.return_class_spelling = source.return_class_spelling;
      method_metadata.return_instancetype_spelling = source.return_instancetype_spelling;
      method_metadata.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
      method_metadata.return_has_invalid_generic_suffix = source.return_has_invalid_generic_suffix;
      method_metadata.return_has_invalid_pointer_declarator = source.return_has_invalid_pointer_declarator;
      method_metadata.return_has_invalid_nullability_suffix = source.return_has_invalid_nullability_suffix;
      method_metadata.return_has_invalid_ownership_qualifier = source.return_has_invalid_ownership_qualifier;
      method_metadata.return_has_invalid_type_suffix = source.return_has_invalid_type_suffix;
      method_metadata.return_ownership_insert_retain = source.return_ownership_insert_retain;
      method_metadata.return_ownership_insert_release = source.return_ownership_insert_release;
      method_metadata.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
      method_metadata.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
      method_metadata.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
      method_metadata.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
      method_metadata.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
      method_metadata.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
      method_metadata.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
      method_metadata.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
      method_metadata.return_type = source.return_type;
      method_metadata.return_canonical_type = source.return_canonical_type;
      method_metadata.return_is_vector = source.return_is_vector;
      method_metadata.return_vector_base_spelling = source.return_vector_base_spelling;
      method_metadata.return_vector_lane_count = source.return_vector_lane_count;
      method_metadata.return_has_protocol_composition = source.return_has_protocol_composition;
      method_metadata.return_protocol_composition_lexicographic = source.return_protocol_composition_lexicographic;
      method_metadata.return_has_invalid_protocol_composition = source.return_has_invalid_protocol_composition;
      method_metadata.async_continuation_profile_is_normalized =
          source.async_continuation_profile_is_normalized;
      method_metadata.deterministic_async_continuation_handoff =
          source.deterministic_async_continuation_handoff;
      method_metadata.async_continuation_sites =
          source.async_continuation_sites;
      method_metadata.async_keyword_sites = source.async_keyword_sites;
      method_metadata.async_function_sites = source.async_function_sites;
      method_metadata.continuation_allocation_sites =
          source.continuation_allocation_sites;
      method_metadata.continuation_resume_sites =
          source.continuation_resume_sites;
      method_metadata.continuation_suspend_sites =
          source.continuation_suspend_sites;
      method_metadata.async_state_machine_sites =
          source.async_state_machine_sites;
      method_metadata.async_continuation_normalized_sites =
          source.async_continuation_normalized_sites;
      method_metadata.async_continuation_gate_blocked_sites =
          source.async_continuation_gate_blocked_sites;
      method_metadata.async_continuation_contract_violation_sites =
          source.async_continuation_contract_violation_sites;
      method_metadata.actor_isolation_sendability_profile_is_normalized =
          source.actor_isolation_sendability_profile_is_normalized;
      method_metadata.deterministic_actor_isolation_sendability_handoff =
          source.deterministic_actor_isolation_sendability_handoff;
      method_metadata.actor_isolation_sendability_sites =
          source.actor_isolation_sendability_sites;
      method_metadata.actor_isolation_decl_sites =
          source.actor_isolation_decl_sites;
      method_metadata.actor_hop_sites = source.actor_hop_sites;
      method_metadata.sendable_annotation_sites =
          source.sendable_annotation_sites;
      method_metadata.non_sendable_crossing_sites =
          source.non_sendable_crossing_sites;
      method_metadata.isolation_boundary_sites = source.isolation_boundary_sites;
      method_metadata.actor_isolation_sendability_normalized_sites =
          source.actor_isolation_sendability_normalized_sites;
      method_metadata.actor_isolation_sendability_gate_blocked_sites =
          source.actor_isolation_sendability_gate_blocked_sites;
      method_metadata.actor_isolation_sendability_contract_violation_sites =
          source.actor_isolation_sendability_contract_violation_sites;
      method_metadata.task_runtime_cancellation_profile_is_normalized =
          source.task_runtime_cancellation_profile_is_normalized;
      method_metadata.deterministic_task_runtime_cancellation_handoff =
          source.deterministic_task_runtime_cancellation_handoff;
      method_metadata.task_runtime_interop_sites =
          source.task_runtime_interop_sites;
      method_metadata.runtime_hook_sites = source.runtime_hook_sites;
      method_metadata.cancellation_check_sites =
          source.cancellation_check_sites;
      method_metadata.cancellation_handler_sites =
          source.cancellation_handler_sites;
      method_metadata.suspension_point_sites = source.suspension_point_sites;
      method_metadata.cancellation_propagation_sites =
          source.cancellation_propagation_sites;
      method_metadata.task_runtime_cancellation_normalized_sites =
          source.task_runtime_cancellation_normalized_sites;
      method_metadata.task_runtime_cancellation_gate_blocked_sites =
          source.task_runtime_cancellation_gate_blocked_sites;
      method_metadata.task_runtime_cancellation_contract_violation_sites =
          source.task_runtime_cancellation_contract_violation_sites;
      method_metadata.concurrency_replay_race_guard_profile_is_normalized =
          source.concurrency_replay_race_guard_profile_is_normalized;
      method_metadata.deterministic_concurrency_replay_race_guard_handoff =
          source.deterministic_concurrency_replay_race_guard_handoff;
      method_metadata.concurrency_replay_race_guard_sites =
          source.concurrency_replay_race_guard_sites;
      method_metadata.concurrency_replay_sites = source.concurrency_replay_sites;
      method_metadata.replay_proof_sites = source.replay_proof_sites;
      method_metadata.race_guard_sites = source.race_guard_sites;
      method_metadata.task_handoff_sites = source.task_handoff_sites;
      method_metadata.actor_isolation_sites = source.actor_isolation_sites;
      method_metadata.deterministic_schedule_sites =
          source.deterministic_schedule_sites;
      method_metadata.concurrency_replay_guard_blocked_sites =
          source.concurrency_replay_guard_blocked_sites;
      method_metadata.concurrency_replay_contract_violation_sites =
          source.concurrency_replay_contract_violation_sites;
      method_metadata.ns_error_bridging_profile_is_normalized =
          source.ns_error_bridging_profile_is_normalized;
      method_metadata.deterministic_ns_error_bridging_lowering_handoff =
          source.deterministic_ns_error_bridging_lowering_handoff;
      method_metadata.ns_error_bridging_sites = source.ns_error_bridging_sites;
      method_metadata.ns_error_parameter_sites = source.ns_error_parameter_sites;
      method_metadata.ns_error_out_parameter_sites =
          source.ns_error_out_parameter_sites;
      method_metadata.ns_error_bridge_path_sites =
          source.ns_error_bridge_path_sites;
      method_metadata.failable_call_sites = source.failable_call_sites;
      method_metadata.ns_error_bridging_normalized_sites =
          source.ns_error_bridging_normalized_sites;
      method_metadata.ns_error_bridge_boundary_sites =
          source.ns_error_bridge_boundary_sites;
      method_metadata.ns_error_bridging_contract_violation_sites =
          source.ns_error_bridging_contract_violation_sites;
      method_metadata.is_class_method = source.is_class_method;
      method_metadata.has_definition = source.has_definition;
      metadata.methods_lexicographic.push_back(std::move(method_metadata));
    }
