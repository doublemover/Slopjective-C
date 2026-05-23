  std::vector<std::string> function_names;
  function_names.reserve(surface.functions.size());
  for (const auto &entry : surface.functions) {
    function_names.push_back(entry.first);
  }
  std::sort(function_names.begin(), function_names.end());

  handoff.functions_lexicographic.reserve(function_names.size());
  for (const std::string &name : function_names) {
    const auto function_it = surface.functions.find(name);
    if (function_it == surface.functions.end()) {
      continue;
    }
    const FunctionInfo &source = function_it->second;
    Objc3SemanticFunctionTypeMetadata metadata;
    metadata.name = name;
    metadata.arity = source.arity;
    metadata.throws_declared = source.throws_declared;
    metadata.typed_throws_declared = source.typed_throws_declared;
    metadata.typed_throws_error_type_spelling =
        source.typed_throws_error_type_spelling;
    metadata.typed_throws_effect_signature_key =
        source.typed_throws_effect_signature_key;
    metadata.typed_throws_callable_compatibility_policy =
        source.typed_throws_callable_compatibility_policy;
    metadata.typed_throws_abi_lowering_ready =
        source.typed_throws_abi_lowering_ready;
    metadata.generic_parameter_names_source_order =
        source.generic_parameter_names_source_order;
    metadata.generic_parameter_variance_source_order =
        source.generic_parameter_variance_source_order;
    metadata.generic_parameter_constraints_lexicographic =
        source.generic_parameter_constraints_lexicographic;
    metadata.generic_callable_signature_replay_key =
        source.generic_callable_signature_replay_key;
    metadata.generic_callable_reification_policy =
        source.generic_callable_reification_policy;
    metadata.generic_callable_mangling_policy_id =
        source.generic_callable_mangling_policy_id;
    metadata.generic_callable_contract_deterministic =
        source.generic_callable_contract_deterministic;
    metadata.param_types = source.param_types;
    metadata.param_canonical_types = source.param_canonical_types;
    metadata.param_is_vector = source.param_is_vector;
    metadata.param_vector_base_spelling = source.param_vector_base_spelling;
    metadata.param_vector_lane_count = source.param_vector_lane_count;
    metadata.param_has_generic_suffix = source.param_has_generic_suffix;
    metadata.param_has_pointer_declarator = source.param_has_pointer_declarator;
    metadata.param_has_nullability_suffix = source.param_has_nullability_suffix;
    metadata.param_has_ownership_qualifier = source.param_has_ownership_qualifier;
    metadata.param_id_spelling = source.param_id_spelling;
    metadata.param_class_spelling = source.param_class_spelling;
    metadata.param_instancetype_spelling = source.param_instancetype_spelling;
    metadata.param_object_pointer_type_spelling = source.param_object_pointer_type_spelling;
    metadata.param_has_invalid_generic_suffix = source.param_has_invalid_generic_suffix;
    metadata.param_has_invalid_pointer_declarator = source.param_has_invalid_pointer_declarator;
    metadata.param_has_invalid_nullability_suffix = source.param_has_invalid_nullability_suffix;
    metadata.param_has_invalid_ownership_qualifier = source.param_has_invalid_ownership_qualifier;
    metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
    metadata.param_ownership_insert_retain = source.param_ownership_insert_retain;
    metadata.param_ownership_insert_release = source.param_ownership_insert_release;
    metadata.param_ownership_insert_autorelease = source.param_ownership_insert_autorelease;
    metadata.param_ownership_is_weak_reference = source.param_ownership_is_weak_reference;
    metadata.param_ownership_is_unowned_reference = source.param_ownership_is_unowned_reference;
    metadata.param_ownership_is_unowned_safe_reference = source.param_ownership_is_unowned_safe_reference;
    metadata.param_ownership_arc_diagnostic_candidate = source.param_ownership_arc_diagnostic_candidate;
    metadata.param_ownership_arc_fixit_available = source.param_ownership_arc_fixit_available;
    metadata.param_ownership_arc_diagnostic_profile = source.param_ownership_arc_diagnostic_profile;
    metadata.param_ownership_arc_fixit_hint = source.param_ownership_arc_fixit_hint;
    metadata.param_has_protocol_composition = source.param_has_protocol_composition;
    metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
    metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
    metadata.return_has_generic_suffix = source.return_has_generic_suffix;
    metadata.return_has_pointer_declarator = source.return_has_pointer_declarator;
    metadata.return_has_nullability_suffix = source.return_has_nullability_suffix;
    metadata.return_has_ownership_qualifier = source.return_has_ownership_qualifier;
    metadata.return_id_spelling = source.return_id_spelling;
    metadata.return_class_spelling = source.return_class_spelling;
    metadata.return_instancetype_spelling = source.return_instancetype_spelling;
    metadata.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
    metadata.return_has_invalid_generic_suffix = source.return_has_invalid_generic_suffix;
    metadata.return_has_invalid_pointer_declarator = source.return_has_invalid_pointer_declarator;
    metadata.return_has_invalid_nullability_suffix = source.return_has_invalid_nullability_suffix;
    metadata.return_has_invalid_ownership_qualifier = source.return_has_invalid_ownership_qualifier;
    metadata.return_has_invalid_type_suffix = source.return_has_invalid_type_suffix;
    metadata.return_ownership_insert_retain = source.return_ownership_insert_retain;
    metadata.return_ownership_insert_release = source.return_ownership_insert_release;
    metadata.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
    metadata.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
    metadata.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
    metadata.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
    metadata.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
    metadata.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
    metadata.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
    metadata.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
    metadata.return_type = source.return_type;
    metadata.return_canonical_type = source.return_canonical_type;
    metadata.return_is_vector = source.return_is_vector;
    metadata.return_vector_base_spelling = source.return_vector_base_spelling;
    metadata.return_vector_lane_count = source.return_vector_lane_count;
    metadata.return_has_protocol_composition = source.return_has_protocol_composition;
    metadata.return_protocol_composition_lexicographic = source.return_protocol_composition_lexicographic;
    metadata.return_has_invalid_protocol_composition = source.return_has_invalid_protocol_composition;
    metadata.async_continuation_profile_is_normalized =
        source.async_continuation_profile_is_normalized;
    metadata.deterministic_async_continuation_handoff =
        source.deterministic_async_continuation_handoff;
    metadata.async_continuation_sites = source.async_continuation_sites;
    metadata.async_keyword_sites = source.async_keyword_sites;
    metadata.async_function_sites = source.async_function_sites;
    metadata.continuation_allocation_sites =
        source.continuation_allocation_sites;
    metadata.continuation_resume_sites = source.continuation_resume_sites;
    metadata.continuation_suspend_sites = source.continuation_suspend_sites;
    metadata.async_state_machine_sites = source.async_state_machine_sites;
    metadata.async_continuation_normalized_sites =
        source.async_continuation_normalized_sites;
    metadata.async_continuation_gate_blocked_sites =
        source.async_continuation_gate_blocked_sites;
    metadata.async_continuation_contract_violation_sites =
        source.async_continuation_contract_violation_sites;
    metadata.actor_isolation_sendability_profile_is_normalized =
        source.actor_isolation_sendability_profile_is_normalized;
    metadata.deterministic_actor_isolation_sendability_handoff =
        source.deterministic_actor_isolation_sendability_handoff;
    metadata.actor_isolation_sendability_sites =
        source.actor_isolation_sendability_sites;
    metadata.actor_isolation_decl_sites = source.actor_isolation_decl_sites;
    metadata.actor_hop_sites = source.actor_hop_sites;
    metadata.sendable_annotation_sites = source.sendable_annotation_sites;
    metadata.non_sendable_crossing_sites = source.non_sendable_crossing_sites;
    metadata.isolation_boundary_sites = source.isolation_boundary_sites;
    metadata.actor_isolation_sendability_normalized_sites =
        source.actor_isolation_sendability_normalized_sites;
    metadata.actor_isolation_sendability_gate_blocked_sites =
        source.actor_isolation_sendability_gate_blocked_sites;
    metadata.actor_isolation_sendability_contract_violation_sites =
        source.actor_isolation_sendability_contract_violation_sites;
    metadata.task_runtime_cancellation_profile_is_normalized =
        source.task_runtime_cancellation_profile_is_normalized;
    metadata.deterministic_task_runtime_cancellation_handoff =
        source.deterministic_task_runtime_cancellation_handoff;
    metadata.task_runtime_interop_sites = source.task_runtime_interop_sites;
    metadata.runtime_hook_sites = source.runtime_hook_sites;
    metadata.cancellation_check_sites = source.cancellation_check_sites;
    metadata.cancellation_handler_sites = source.cancellation_handler_sites;
    metadata.suspension_point_sites = source.suspension_point_sites;
    metadata.cancellation_propagation_sites =
        source.cancellation_propagation_sites;
    metadata.task_runtime_cancellation_normalized_sites =
        source.task_runtime_cancellation_normalized_sites;
    metadata.task_runtime_cancellation_gate_blocked_sites =
        source.task_runtime_cancellation_gate_blocked_sites;
    metadata.task_runtime_cancellation_contract_violation_sites =
        source.task_runtime_cancellation_contract_violation_sites;
    metadata.concurrency_replay_race_guard_profile_is_normalized =
        source.concurrency_replay_race_guard_profile_is_normalized;
    metadata.deterministic_concurrency_replay_race_guard_handoff =
        source.deterministic_concurrency_replay_race_guard_handoff;
    metadata.concurrency_replay_race_guard_sites =
        source.concurrency_replay_race_guard_sites;
    metadata.concurrency_replay_sites = source.concurrency_replay_sites;
    metadata.replay_proof_sites = source.replay_proof_sites;
    metadata.race_guard_sites = source.race_guard_sites;
    metadata.task_handoff_sites = source.task_handoff_sites;
    metadata.actor_isolation_sites = source.actor_isolation_sites;
    metadata.deterministic_schedule_sites = source.deterministic_schedule_sites;
    metadata.concurrency_replay_guard_blocked_sites =
        source.concurrency_replay_guard_blocked_sites;
    metadata.concurrency_replay_contract_violation_sites =
        source.concurrency_replay_contract_violation_sites;
    metadata.ns_error_bridging_profile_is_normalized =
        source.ns_error_bridging_profile_is_normalized;
    metadata.deterministic_ns_error_bridging_lowering_handoff =
        source.deterministic_ns_error_bridging_lowering_handoff;
    metadata.ns_error_bridging_sites = source.ns_error_bridging_sites;
    metadata.ns_error_parameter_sites = source.ns_error_parameter_sites;
    metadata.ns_error_out_parameter_sites = source.ns_error_out_parameter_sites;
    metadata.ns_error_bridge_path_sites = source.ns_error_bridge_path_sites;
    metadata.failable_call_sites = source.failable_call_sites;
    metadata.ns_error_bridging_normalized_sites =
        source.ns_error_bridging_normalized_sites;
    metadata.ns_error_bridge_boundary_sites =
        source.ns_error_bridge_boundary_sites;
    metadata.ns_error_bridging_contract_violation_sites =
        source.ns_error_bridging_contract_violation_sites;
    metadata.has_definition = source.has_definition;
    metadata.is_pure_annotation = source.is_pure_annotation;
    handoff.functions_lexicographic.push_back(std::move(metadata));
  }
