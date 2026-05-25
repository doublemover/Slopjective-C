// Legacy extraction anchor retained for contract tests:
// BuildMethodInfo(const Objc3MethodDecl &method)
static Objc3MethodInfo BuildMethodInfo(const Objc3MethodDecl &method,
                                       const MethodSelectorNormalizationContractInfo &selector_contract,
                                       bool arc_mode_enabled,
                                       const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  Objc3MethodInfo info;
  info.selector_normalized = selector_contract.normalized_selector;
  info.selector_piece_count = selector_contract.selector_piece_count;
  info.selector_parameter_piece_count = selector_contract.selector_parameter_piece_count;
  info.selector_contract_normalized = selector_contract.selector_contract_normalized;
  info.selector_had_pieceless_form = selector_contract.selector_had_pieceless_form;
  info.selector_has_spelling_mismatch = selector_contract.selector_has_spelling_mismatch;
  info.selector_has_arity_mismatch = selector_contract.selector_has_arity_mismatch;
  info.selector_has_parameter_linkage_mismatch = selector_contract.selector_has_parameter_linkage_mismatch;
  info.selector_has_normalization_flag_mismatch = selector_contract.selector_has_normalization_flag_mismatch;
  info.selector_has_missing_piece_keyword = selector_contract.selector_has_missing_piece_keyword;
  info.arity = method.params.size();
  info.generic_parameter_names_source_order =
      CollectGenericParameterNamesSourceOrder(method.generic_params);
  info.generic_parameter_variance_source_order =
      CollectGenericParameterVarianceSourceOrder(method.generic_params);
  info.generic_parameter_constraints_lexicographic =
      CollectGenericParameterConstraintsLexicographic(method.generic_params);
  info.generic_callable_reification_policy =
      Objc3GenericCallableReificationPolicyForMarker(
          method.generic_callable_reification_marker_declared);
  info.generic_callable_mangling_policy_id =
      Objc3GenericCallableManglingPolicyForReificationPolicy(
          info.generic_callable_reification_policy);
  info.generic_callable_signature_replay_key =
      BuildObjc3GenericCallableSignatureReplayKey(
          method.is_class_method ? "class-method" : "instance-method",
          method.selector, info.generic_parameter_names_source_order,
          info.generic_parameter_variance_source_order,
          info.generic_parameter_constraints_lexicographic,
          info.generic_callable_reification_policy,
          info.generic_callable_mangling_policy_id);
  info.generic_callable_contract_deterministic =
      info.generic_parameter_names_source_order.size() ==
          info.generic_parameter_variance_source_order.size() &&
      info.generic_parameter_names_source_order.size() ==
          info.generic_parameter_constraints_lexicographic.size() &&
      !info.generic_callable_signature_replay_key.empty() &&
      IsObjc3GenericCallableReificationPolicyClaimable(
          info.generic_callable_reification_policy) &&
      IsObjc3GenericCallableManglingPolicyClaimable(
          info.generic_callable_mangling_policy_id);
  info.param_types.reserve(method.params.size());
  info.param_canonical_types.reserve(method.params.size());
  info.param_is_vector.reserve(method.params.size());
  info.param_vector_base_spelling.reserve(method.params.size());
  info.param_vector_lane_count.reserve(method.params.size());
  info.param_has_generic_suffix.reserve(method.params.size());
  info.param_has_pointer_declarator.reserve(method.params.size());
  info.param_has_nullability_suffix.reserve(method.params.size());
  info.param_has_ownership_qualifier.reserve(method.params.size());
  info.param_id_spelling.reserve(method.params.size());
  info.param_class_spelling.reserve(method.params.size());
  info.param_instancetype_spelling.reserve(method.params.size());
  info.param_object_pointer_type_spelling.reserve(method.params.size());
  info.param_has_invalid_generic_suffix.reserve(method.params.size());
  info.param_has_invalid_pointer_declarator.reserve(method.params.size());
  info.param_has_invalid_nullability_suffix.reserve(method.params.size());
  info.param_has_invalid_ownership_qualifier.reserve(method.params.size());
  info.param_has_invalid_type_suffix.reserve(method.params.size());
  info.param_ownership_insert_retain.reserve(method.params.size());
  info.param_ownership_insert_release.reserve(method.params.size());
  info.param_ownership_insert_autorelease.reserve(method.params.size());
  info.param_ownership_is_weak_reference.reserve(method.params.size());
  info.param_ownership_is_unowned_reference.reserve(method.params.size());
  info.param_ownership_is_unowned_safe_reference.reserve(method.params.size());
  info.param_ownership_arc_diagnostic_candidate.reserve(method.params.size());
  info.param_ownership_arc_fixit_available.reserve(method.params.size());
  info.param_ownership_arc_diagnostic_profile.reserve(method.params.size());
  info.param_ownership_arc_fixit_hint.reserve(method.params.size());
  info.param_has_protocol_composition.reserve(method.params.size());
  info.param_protocol_composition_lexicographic.reserve(method.params.size());
  info.param_has_invalid_protocol_composition.reserve(method.params.size());
  for (const auto &param : method.params) {
    const ProtocolCompositionInfo protocol_composition =
        BuildProtocolCompositionInfoFromParam(param, generic_interface_names);
    info.param_types.push_back(param.type);
    info.param_canonical_types.push_back(
        BuildCanonicalSemanticTypeFromParam(param, generic_interface_names));
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
    const std::size_t param_index = info.param_has_ownership_qualifier.size() - 1u;
    const bool arc_inferred_object_param =
        arc_mode_enabled && !info.param_has_ownership_qualifier[param_index] &&
        (info.param_id_spelling[param_index] ||
         info.param_instancetype_spelling[param_index] ||
         info.param_object_pointer_type_spelling[param_index]);
    if (arc_inferred_object_param) {
      // ARC inference/lifetime implementation anchor: under
      // explicit ARC mode, unqualified object parameters now synthesize a
      // canonical strong-owned retain/release profile for the supported
      // runnable slice.
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
      BuildProtocolCompositionInfoFromMethodReturn(method, generic_interface_names);
  info.return_has_generic_suffix = method.has_return_generic_suffix;
  info.return_has_pointer_declarator = method.has_return_pointer_declarator;
  info.return_has_nullability_suffix = !method.return_nullability_suffix_tokens.empty();
  info.return_has_ownership_qualifier = method.has_return_ownership_qualifier;
  info.return_id_spelling = method.return_id_spelling;
  info.return_class_spelling = method.return_class_spelling;
  info.return_instancetype_spelling = method.return_instancetype_spelling;
  info.return_object_pointer_type_spelling = method.return_object_pointer_type_spelling;
  info.return_has_invalid_generic_suffix = HasInvalidGenericReturnTypeSuffix(method);
  info.return_has_invalid_pointer_declarator = HasInvalidPointerReturnTypeDeclarator(method);
  info.return_has_invalid_nullability_suffix = HasInvalidNullabilityReturnTypeSuffix(method);
  info.return_has_invalid_ownership_qualifier = HasInvalidOwnershipQualifierReturnTypeSuffix(method);
  info.return_has_invalid_type_suffix = info.return_has_invalid_generic_suffix ||
                                        info.return_has_invalid_pointer_declarator ||
                                        info.return_has_invalid_nullability_suffix ||
                                        info.return_has_invalid_ownership_qualifier;
  info.return_ownership_insert_retain = method.return_ownership_insert_retain;
  info.return_ownership_insert_release = method.return_ownership_insert_release;
  info.return_ownership_insert_autorelease = method.return_ownership_insert_autorelease;
  info.return_ownership_is_weak_reference = method.return_ownership_is_weak_reference;
  info.return_ownership_is_unowned_reference = method.return_ownership_is_unowned_reference;
  info.return_ownership_is_unowned_safe_reference = method.return_ownership_is_unowned_safe_reference;
  info.return_ownership_arc_diagnostic_candidate = method.return_ownership_arc_diagnostic_candidate;
  info.return_ownership_arc_fixit_available = method.return_ownership_arc_fixit_available;
  info.return_ownership_arc_diagnostic_profile = method.return_ownership_arc_diagnostic_profile;
  info.return_ownership_arc_fixit_hint = method.return_ownership_arc_fixit_hint;
  info.return_type = method.return_type;
  info.return_canonical_type =
      BuildCanonicalSemanticTypeFromMethodReturn(method, generic_interface_names);
  info.return_is_vector = method.return_vector_spelling;
  info.return_vector_base_spelling = method.return_vector_base_spelling;
  info.return_vector_lane_count = method.return_vector_lane_count;
  info.return_has_protocol_composition = return_protocol_composition.has_protocol_composition;
  info.return_protocol_composition_lexicographic = return_protocol_composition.names_lexicographic;
  info.return_has_invalid_protocol_composition = return_protocol_composition.has_invalid_protocol_composition;
  const bool arc_inferred_object_return =
      arc_mode_enabled && !info.return_has_ownership_qualifier &&
      (info.return_id_spelling || info.return_instancetype_spelling ||
       info.return_object_pointer_type_spelling);
  if (arc_inferred_object_return) {
    // ARC inference/lifetime implementation anchor: under explicit
    // ARC mode, unqualified object returns now synthesize a canonical
    // strong-owned retain/release profile for the supported runnable slice.
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
      method.async_continuation_profile_is_normalized;
  info.deterministic_async_continuation_handoff =
      method.deterministic_async_continuation_handoff;
  info.async_continuation_sites = method.async_continuation_sites;
  info.async_keyword_sites = method.async_keyword_sites;
  info.async_function_sites = method.async_function_sites;
  info.continuation_allocation_sites = method.continuation_allocation_sites;
  info.continuation_resume_sites = method.continuation_resume_sites;
  info.continuation_suspend_sites = method.continuation_suspend_sites;
  info.async_state_machine_sites = method.async_state_machine_sites;
  info.async_continuation_normalized_sites =
      method.async_continuation_normalized_sites;
  info.async_continuation_gate_blocked_sites =
      method.async_continuation_gate_blocked_sites;
  info.async_continuation_contract_violation_sites =
      method.async_continuation_contract_violation_sites;
  info.actor_isolation_sendability_profile_is_normalized =
      method.actor_isolation_sendability_profile_is_normalized;
  info.deterministic_actor_isolation_sendability_handoff =
      method.deterministic_actor_isolation_sendability_handoff;
  info.actor_isolation_sendability_sites =
      method.actor_isolation_sendability_sites;
  info.actor_isolation_decl_sites = method.actor_isolation_decl_sites;
  info.actor_hop_sites = method.actor_hop_sites;
  info.sendable_annotation_sites = method.sendable_annotation_sites;
  info.non_sendable_crossing_sites = method.non_sendable_crossing_sites;
  info.isolation_boundary_sites = method.isolation_boundary_sites;
  info.actor_isolation_sendability_normalized_sites =
      method.actor_isolation_sendability_normalized_sites;
  info.actor_isolation_sendability_gate_blocked_sites =
      method.actor_isolation_sendability_gate_blocked_sites;
  info.actor_isolation_sendability_contract_violation_sites =
      method.actor_isolation_sendability_contract_violation_sites;
  info.task_runtime_cancellation_profile_is_normalized =
      method.task_runtime_cancellation_profile_is_normalized;
  info.deterministic_task_runtime_cancellation_handoff =
      method.deterministic_task_runtime_cancellation_handoff;
  info.task_runtime_interop_sites = method.task_runtime_interop_sites;
  info.runtime_hook_sites = method.runtime_hook_sites;
  info.cancellation_check_sites = method.cancellation_check_sites;
  info.cancellation_handler_sites = method.cancellation_handler_sites;
  info.suspension_point_sites = method.suspension_point_sites;
  info.cancellation_propagation_sites = method.cancellation_propagation_sites;
  info.task_runtime_cancellation_normalized_sites =
      method.task_runtime_cancellation_normalized_sites;
  info.task_runtime_cancellation_gate_blocked_sites =
      method.task_runtime_cancellation_gate_blocked_sites;
  info.task_runtime_cancellation_contract_violation_sites =
      method.task_runtime_cancellation_contract_violation_sites;
  info.concurrency_replay_race_guard_profile_is_normalized =
      method.concurrency_replay_race_guard_profile_is_normalized;
  info.deterministic_concurrency_replay_race_guard_handoff =
      method.deterministic_concurrency_replay_race_guard_handoff;
  info.concurrency_replay_race_guard_sites =
      method.concurrency_replay_race_guard_sites;
  info.concurrency_replay_sites = method.concurrency_replay_sites;
  info.replay_proof_sites = method.replay_proof_sites;
  info.race_guard_sites = method.race_guard_sites;
  info.task_handoff_sites = method.task_handoff_sites;
  info.actor_isolation_sites = method.actor_isolation_sites;
  info.deterministic_schedule_sites = method.deterministic_schedule_sites;
  info.concurrency_replay_guard_blocked_sites =
      method.concurrency_replay_guard_blocked_sites;
  info.concurrency_replay_contract_violation_sites =
      method.concurrency_replay_contract_violation_sites;
  info.ns_error_bridging_profile_is_normalized =
      method.ns_error_bridging_profile_is_normalized;
  info.deterministic_ns_error_bridging_lowering_handoff =
      method.deterministic_ns_error_bridging_lowering_handoff;
  info.ns_error_bridging_sites = method.ns_error_bridging_sites;
  info.ns_error_parameter_sites = method.ns_error_parameter_sites;
  info.ns_error_out_parameter_sites = method.ns_error_out_parameter_sites;
  info.ns_error_bridge_path_sites = method.ns_error_bridge_path_sites;
  info.failable_call_sites = method.failable_call_sites;
  info.ns_error_bridging_normalized_sites =
      method.ns_error_bridging_normalized_sites;
  info.ns_error_bridge_boundary_sites = method.ns_error_bridge_boundary_sites;
  info.ns_error_bridging_contract_violation_sites =
      method.ns_error_bridging_contract_violation_sites;
  info.objc_direct_declared = method.objc_direct_declared;
  info.objc_final_declared = method.objc_final_declared;
  info.objc_dynamic_declared = method.objc_dynamic_declared;
  info.is_class_method = method.is_class_method;
  info.has_definition = method.has_body;
  info.throws_declared = method.throws_declared;
  info.typed_throws_declared = method.typed_throws_declared;
  info.typed_throws_error_type_spelling =
      method.typed_throws_payload.canonical_spelling;
  info.typed_throws_effect_signature_key =
      Objc3BuildTypedThrowsEffectSignatureKey(
          method.throws_declared,
          method.typed_throws_declared,
          method.typed_throws_payload.canonical_spelling);
  info.typed_throws_callable_compatibility_policy =
      Objc3BuildTypedThrowsCallableCompatibilityPolicy(
          method.throws_declared,
          method.typed_throws_declared);
  info.typed_throws_abi_lowering_ready =
      Objc3TypedThrowsAbiLoweringReady(
          method.throws_declared,
          method.typed_throws_declared,
          method.typed_throws_payload.canonical_spelling);
  return info;
}
