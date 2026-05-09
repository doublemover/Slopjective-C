  }


  bool ParseOptionalContainerDispatchAttributes(Objc3InterfaceDecl &decl) {
    return objc3c::parse::ParseObjc3OptionalContainerDispatchAttributes(
        tokens_, index_, diagnostics_, decl);
  }

  template <typename TCallableDecl>
  bool ParseOptionalCallableBridgeAttributes(TCallableDecl &decl) {
    return objc3c::parse::ParseObjc3OptionalCallableBridgeAttributes(
        tokens_, index_, diagnostics_, decl);
  }

  template <typename TCallableDecl>
  void FinalizeErrorBridgeMarkerProfile(TCallableDecl &decl) {
    objc3c::parse::FinalizeObjc3ErrorBridgeMarkerProfile(decl);
  }

  void FinalizeThrowsDeclarationProfile(FunctionDecl &fn, bool has_return_annotation) {
    fn.throws_declaration_profile = BuildThrowsDeclarationProfile(
        fn.throws_declared,
        has_return_annotation,
        fn.is_prototype,
        !fn.is_prototype,
        false,
        false,
        fn.params.size(),
        0u);
    fn.throws_declaration_profile_is_normalized = IsThrowsDeclarationProfileNormalized(
        fn.is_prototype,
        !fn.is_prototype,
        false,
        0u);
  }

  void FinalizeThrowsDeclarationProfile(Objc3MethodDecl &method) {
    method.throws_declaration_profile = BuildThrowsDeclarationProfile(
        method.throws_declared,
        true,
        !method.has_body,
        method.has_body,
        true,
        method.is_class_method,
        method.params.size(),
        method.selector_pieces.size());
    method.throws_declaration_profile_is_normalized = IsThrowsDeclarationProfileNormalized(
        !method.has_body,
        method.has_body,
        true,
        method.selector_pieces.size());
  }

  void FinalizeResultLikeProfile(FunctionDecl &fn) {
    const Objc3ResultLikeProfile profile = BuildResultLikeProfileFromBody(fn.body);
    fn.result_like_sites = profile.result_like_sites;
    fn.result_success_sites = profile.result_success_sites;
    fn.result_failure_sites = profile.result_failure_sites;
    fn.result_branch_sites = profile.result_branch_sites;
    fn.result_payload_sites = profile.result_payload_sites;
    fn.result_normalized_sites = profile.normalized_sites;
    fn.result_branch_merge_sites = profile.branch_merge_sites;
    fn.result_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_result_like_lowering_handoff =
        profile.deterministic_result_like_lowering_handoff;
    fn.result_like_profile = BuildResultLikeProfile(
        fn.result_like_sites,
        fn.result_success_sites,
        fn.result_failure_sites,
        fn.result_branch_sites,
        fn.result_payload_sites,
        fn.result_normalized_sites,
        fn.result_branch_merge_sites,
        fn.result_contract_violation_sites,
        fn.deterministic_result_like_lowering_handoff);
    fn.result_like_profile_is_normalized = IsResultLikeProfileNormalized(
        fn.result_like_sites,
        fn.result_success_sites,
        fn.result_failure_sites,
        fn.result_branch_sites,
        fn.result_payload_sites,
        fn.result_normalized_sites,
        fn.result_branch_merge_sites,
        fn.result_contract_violation_sites);
  }

  void FinalizeResultLikeProfile(Objc3MethodDecl &method) {
    const Objc3ResultLikeProfile profile = BuildResultLikeProfileFromOpaqueBody(method.has_body);
    method.result_like_sites = profile.result_like_sites;
    method.result_success_sites = profile.result_success_sites;
    method.result_failure_sites = profile.result_failure_sites;
    method.result_branch_sites = profile.result_branch_sites;
    method.result_payload_sites = profile.result_payload_sites;
    method.result_normalized_sites = profile.normalized_sites;
    method.result_branch_merge_sites = profile.branch_merge_sites;
    method.result_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_result_like_lowering_handoff =
        profile.deterministic_result_like_lowering_handoff;
    method.result_like_profile = BuildResultLikeProfile(
        method.result_like_sites,
        method.result_success_sites,
        method.result_failure_sites,
        method.result_branch_sites,
        method.result_payload_sites,
        method.result_normalized_sites,
        method.result_branch_merge_sites,
        method.result_contract_violation_sites,
        method.deterministic_result_like_lowering_handoff);
    method.result_like_profile_is_normalized = IsResultLikeProfileNormalized(
        method.result_like_sites,
        method.result_success_sites,
        method.result_failure_sites,
        method.result_branch_sites,
        method.result_payload_sites,
        method.result_normalized_sites,
        method.result_branch_merge_sites,
        method.result_contract_violation_sites);
  }

  void FinalizeNSErrorBridgingProfile(FunctionDecl &fn) {
    const Objc3NSErrorBridgingProfile profile = BuildNSErrorBridgingProfileFromFunction(fn);
    fn.ns_error_bridging_sites = profile.ns_error_bridging_sites;
    fn.ns_error_parameter_sites = profile.ns_error_parameter_sites;
    fn.ns_error_out_parameter_sites = profile.ns_error_out_parameter_sites;
    fn.ns_error_bridge_path_sites = profile.ns_error_bridge_path_sites;
    fn.failable_call_sites = profile.failable_call_sites;
    fn.ns_error_bridging_normalized_sites = profile.normalized_sites;
    fn.ns_error_bridge_boundary_sites = profile.bridge_boundary_sites;
    fn.ns_error_bridging_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_ns_error_bridging_lowering_handoff =
        profile.deterministic_ns_error_bridging_lowering_handoff;
    fn.ns_error_bridging_profile = BuildNSErrorBridgingProfile(
        fn.ns_error_bridging_sites,
        fn.ns_error_parameter_sites,
        fn.ns_error_out_parameter_sites,
        fn.ns_error_bridge_path_sites,
        fn.failable_call_sites,
        fn.ns_error_bridging_normalized_sites,
        fn.ns_error_bridge_boundary_sites,
        fn.ns_error_bridging_contract_violation_sites,
        fn.deterministic_ns_error_bridging_lowering_handoff);
    fn.ns_error_bridging_profile_is_normalized = IsNSErrorBridgingProfileNormalized(
        fn.ns_error_bridging_sites,
        fn.ns_error_parameter_sites,
        fn.ns_error_out_parameter_sites,
        fn.ns_error_bridge_path_sites,
        fn.failable_call_sites,
        fn.ns_error_bridging_normalized_sites,
        fn.ns_error_bridge_boundary_sites,
        fn.ns_error_bridging_contract_violation_sites);
  }

  void FinalizeNSErrorBridgingProfile(Objc3MethodDecl &method) {
    const Objc3NSErrorBridgingProfile profile = BuildNSErrorBridgingProfileFromOpaqueBody(method);
    method.ns_error_bridging_sites = profile.ns_error_bridging_sites;
    method.ns_error_parameter_sites = profile.ns_error_parameter_sites;
    method.ns_error_out_parameter_sites = profile.ns_error_out_parameter_sites;
    method.ns_error_bridge_path_sites = profile.ns_error_bridge_path_sites;
    method.failable_call_sites = profile.failable_call_sites;
    method.ns_error_bridging_normalized_sites = profile.normalized_sites;
    method.ns_error_bridge_boundary_sites = profile.bridge_boundary_sites;
    method.ns_error_bridging_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_ns_error_bridging_lowering_handoff =
        profile.deterministic_ns_error_bridging_lowering_handoff;
    method.ns_error_bridging_profile = BuildNSErrorBridgingProfile(
        method.ns_error_bridging_sites,
        method.ns_error_parameter_sites,
        method.ns_error_out_parameter_sites,
        method.ns_error_bridge_path_sites,
        method.failable_call_sites,
        method.ns_error_bridging_normalized_sites,
        method.ns_error_bridge_boundary_sites,
        method.ns_error_bridging_contract_violation_sites,
        method.deterministic_ns_error_bridging_lowering_handoff);
    method.ns_error_bridging_profile_is_normalized = IsNSErrorBridgingProfileNormalized(
        method.ns_error_bridging_sites,
        method.ns_error_parameter_sites,
        method.ns_error_out_parameter_sites,
        method.ns_error_bridge_path_sites,
        method.failable_call_sites,
        method.ns_error_bridging_normalized_sites,
        method.ns_error_bridge_boundary_sites,
        method.ns_error_bridging_contract_violation_sites);
  }

  void FinalizeUnwindCleanupProfile(FunctionDecl &fn) {
    const Objc3UnwindCleanupProfile profile = BuildUnwindCleanupProfileFromFunction(fn);
    fn.unwind_cleanup_sites = profile.unwind_cleanup_sites;
    fn.exceptional_exit_sites = profile.exceptional_exit_sites;
    fn.cleanup_action_sites = profile.cleanup_action_sites;
    fn.cleanup_scope_sites = profile.cleanup_scope_sites;
    fn.cleanup_resume_sites = profile.cleanup_resume_sites;
    fn.unwind_cleanup_normalized_sites = profile.normalized_sites;
    fn.unwind_cleanup_fail_closed_sites = profile.fail_closed_sites;
    fn.unwind_cleanup_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_unwind_cleanup_handoff =
        profile.deterministic_unwind_cleanup_handoff;
    fn.unwind_cleanup_profile = BuildUnwindCleanupProfile(
        fn.unwind_cleanup_sites,
        fn.exceptional_exit_sites,
        fn.cleanup_action_sites,
        fn.cleanup_scope_sites,
        fn.cleanup_resume_sites,
        fn.unwind_cleanup_normalized_sites,
        fn.unwind_cleanup_fail_closed_sites,
        fn.unwind_cleanup_contract_violation_sites,
        fn.deterministic_unwind_cleanup_handoff);
    fn.unwind_cleanup_profile_is_normalized = IsUnwindCleanupProfileNormalized(
        fn.unwind_cleanup_sites,
        fn.exceptional_exit_sites,
        fn.cleanup_action_sites,
        fn.cleanup_scope_sites,
        fn.cleanup_resume_sites,
        fn.unwind_cleanup_normalized_sites,
        fn.unwind_cleanup_fail_closed_sites,
        fn.unwind_cleanup_contract_violation_sites);
  }

  void FinalizeUnwindCleanupProfile(Objc3MethodDecl &method) {
    const Objc3UnwindCleanupProfile profile = BuildUnwindCleanupProfileFromOpaqueBody(method);
    method.unwind_cleanup_sites = profile.unwind_cleanup_sites;
    method.exceptional_exit_sites = profile.exceptional_exit_sites;
    method.cleanup_action_sites = profile.cleanup_action_sites;
    method.cleanup_scope_sites = profile.cleanup_scope_sites;
    method.cleanup_resume_sites = profile.cleanup_resume_sites;
    method.unwind_cleanup_normalized_sites = profile.normalized_sites;
    method.unwind_cleanup_fail_closed_sites = profile.fail_closed_sites;
    method.unwind_cleanup_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_unwind_cleanup_handoff =
        profile.deterministic_unwind_cleanup_handoff;
    method.unwind_cleanup_profile = BuildUnwindCleanupProfile(
        method.unwind_cleanup_sites,
        method.exceptional_exit_sites,
        method.cleanup_action_sites,
        method.cleanup_scope_sites,
        method.cleanup_resume_sites,
        method.unwind_cleanup_normalized_sites,
        method.unwind_cleanup_fail_closed_sites,
        method.unwind_cleanup_contract_violation_sites,
        method.deterministic_unwind_cleanup_handoff);
    method.unwind_cleanup_profile_is_normalized = IsUnwindCleanupProfileNormalized(
        method.unwind_cleanup_sites,
        method.exceptional_exit_sites,
        method.cleanup_action_sites,
        method.cleanup_scope_sites,
        method.cleanup_resume_sites,
        method.unwind_cleanup_normalized_sites,
        method.unwind_cleanup_fail_closed_sites,
        method.unwind_cleanup_contract_violation_sites);
  }

  void FinalizeErrorDiagnosticsRecoveryProfile(FunctionDecl &fn) {
    const Objc3ErrorDiagnosticsRecoveryProfile profile =
        BuildErrorDiagnosticsRecoveryProfileFromFunction(fn);
    fn.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;
    fn.diagnostic_emit_sites = profile.diagnostic_emit_sites;
    fn.recovery_anchor_sites = profile.recovery_anchor_sites;
    fn.recovery_boundary_sites = profile.recovery_boundary_sites;
    fn.fail_closed_diagnostic_sites = profile.fail_closed_diagnostic_sites;
    fn.error_diagnostics_recovery_normalized_sites = profile.normalized_sites;
    fn.error_diagnostics_recovery_gate_blocked_sites = profile.gate_blocked_sites;
    fn.error_diagnostics_recovery_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_error_diagnostics_recovery_handoff =
        profile.deterministic_error_diagnostics_recovery_handoff;
    fn.error_diagnostics_recovery_profile = BuildErrorDiagnosticsRecoveryProfile(
        fn.error_diagnostics_recovery_sites,
        fn.diagnostic_emit_sites,
        fn.recovery_anchor_sites,
        fn.recovery_boundary_sites,
        fn.fail_closed_diagnostic_sites,
        fn.error_diagnostics_recovery_normalized_sites,
        fn.error_diagnostics_recovery_gate_blocked_sites,
        fn.error_diagnostics_recovery_contract_violation_sites,
        fn.deterministic_error_diagnostics_recovery_handoff);
    fn.error_diagnostics_recovery_profile_is_normalized =
        IsErrorDiagnosticsRecoveryProfileNormalized(
            fn.error_diagnostics_recovery_sites,
            fn.diagnostic_emit_sites,
            fn.recovery_anchor_sites,
            fn.recovery_boundary_sites,
            fn.fail_closed_diagnostic_sites,
            fn.error_diagnostics_recovery_normalized_sites,
            fn.error_diagnostics_recovery_gate_blocked_sites,
            fn.error_diagnostics_recovery_contract_violation_sites);
  }

  void FinalizeErrorDiagnosticsRecoveryProfile(Objc3MethodDecl &method) {
    const Objc3ErrorDiagnosticsRecoveryProfile profile =
        BuildErrorDiagnosticsRecoveryProfileFromOpaqueBody(method);
    method.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;
    method.diagnostic_emit_sites = profile.diagnostic_emit_sites;
    method.recovery_anchor_sites = profile.recovery_anchor_sites;
    method.recovery_boundary_sites = profile.recovery_boundary_sites;
    method.fail_closed_diagnostic_sites = profile.fail_closed_diagnostic_sites;
    method.error_diagnostics_recovery_normalized_sites = profile.normalized_sites;
    method.error_diagnostics_recovery_gate_blocked_sites = profile.gate_blocked_sites;
    method.error_diagnostics_recovery_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_error_diagnostics_recovery_handoff =
        profile.deterministic_error_diagnostics_recovery_handoff;
    method.error_diagnostics_recovery_profile = BuildErrorDiagnosticsRecoveryProfile(
        method.error_diagnostics_recovery_sites,
        method.diagnostic_emit_sites,
        method.recovery_anchor_sites,
        method.recovery_boundary_sites,
        method.fail_closed_diagnostic_sites,
        method.error_diagnostics_recovery_normalized_sites,
        method.error_diagnostics_recovery_gate_blocked_sites,
        method.error_diagnostics_recovery_contract_violation_sites,
        method.deterministic_error_diagnostics_recovery_handoff);
    method.error_diagnostics_recovery_profile_is_normalized =
        IsErrorDiagnosticsRecoveryProfileNormalized(
            method.error_diagnostics_recovery_sites,
            method.diagnostic_emit_sites,
            method.recovery_anchor_sites,
            method.recovery_boundary_sites,
            method.fail_closed_diagnostic_sites,
            method.error_diagnostics_recovery_normalized_sites,
            method.error_diagnostics_recovery_gate_blocked_sites,
            method.error_diagnostics_recovery_contract_violation_sites);
  }

  void FinalizeAsyncContinuationProfile(FunctionDecl &fn) {
    const Objc3AsyncContinuationProfile profile =
        BuildAsyncContinuationProfileFromFunction(fn);
    fn.async_continuation_sites = profile.async_continuation_sites;
    fn.async_keyword_sites = profile.async_keyword_sites;
    fn.async_function_sites = profile.async_function_sites;
    fn.continuation_allocation_sites = profile.continuation_allocation_sites;
    fn.continuation_resume_sites = profile.continuation_resume_sites;
    fn.continuation_suspend_sites = profile.continuation_suspend_sites;
    fn.async_state_machine_sites = profile.async_state_machine_sites;
    fn.async_continuation_normalized_sites = profile.normalized_sites;
    fn.async_continuation_gate_blocked_sites = profile.gate_blocked_sites;
    fn.async_continuation_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_async_continuation_handoff =
        profile.deterministic_async_continuation_handoff;
    fn.async_continuation_profile = BuildAsyncContinuationProfile(
        fn.async_continuation_sites,
        fn.async_keyword_sites,
        fn.async_function_sites,
        fn.continuation_allocation_sites,
        fn.continuation_resume_sites,
        fn.continuation_suspend_sites,
        fn.async_state_machine_sites,
        fn.async_continuation_normalized_sites,
        fn.async_continuation_gate_blocked_sites,
        fn.async_continuation_contract_violation_sites,
        fn.deterministic_async_continuation_handoff);
    fn.async_continuation_profile_is_normalized =
        IsAsyncContinuationProfileNormalized(
            fn.async_continuation_sites,
            fn.async_keyword_sites,
            fn.async_function_sites,
            fn.continuation_allocation_sites,
            fn.continuation_resume_sites,
            fn.continuation_suspend_sites,
            fn.async_state_machine_sites,
            fn.async_continuation_normalized_sites,
            fn.async_continuation_gate_blocked_sites,
            fn.async_continuation_contract_violation_sites);
  }

  void FinalizeAsyncContinuationProfile(Objc3MethodDecl &method) {
    const Objc3AsyncContinuationProfile profile =
        BuildAsyncContinuationProfileFromOpaqueBody(method);
    method.async_continuation_sites = profile.async_continuation_sites;
    method.async_keyword_sites = profile.async_keyword_sites;
    method.async_function_sites = profile.async_function_sites;
    method.continuation_allocation_sites = profile.continuation_allocation_sites;
    method.continuation_resume_sites = profile.continuation_resume_sites;
    method.continuation_suspend_sites = profile.continuation_suspend_sites;
    method.async_state_machine_sites = profile.async_state_machine_sites;
    method.async_continuation_normalized_sites = profile.normalized_sites;
    method.async_continuation_gate_blocked_sites = profile.gate_blocked_sites;
    method.async_continuation_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_async_continuation_handoff =
        profile.deterministic_async_continuation_handoff;
    method.async_continuation_profile = BuildAsyncContinuationProfile(
        method.async_continuation_sites,
        method.async_keyword_sites,
        method.async_function_sites,
        method.continuation_allocation_sites,
        method.continuation_resume_sites,
        method.continuation_suspend_sites,
        method.async_state_machine_sites,
        method.async_continuation_normalized_sites,
        method.async_continuation_gate_blocked_sites,
        method.async_continuation_contract_violation_sites,
        method.deterministic_async_continuation_handoff);
    method.async_continuation_profile_is_normalized =
        IsAsyncContinuationProfileNormalized(
            method.async_continuation_sites,
            method.async_keyword_sites,
            method.async_function_sites,
            method.continuation_allocation_sites,
            method.continuation_resume_sites,
            method.continuation_suspend_sites,
            method.async_state_machine_sites,
            method.async_continuation_normalized_sites,
            method.async_continuation_gate_blocked_sites,
            method.async_continuation_contract_violation_sites);
  }

  void FinalizeAwaitSuspensionProfile(FunctionDecl &fn) {
    const Objc3AwaitSuspensionProfile profile =
        BuildAwaitSuspensionProfileFromFunction(fn);
    fn.await_suspension_sites = profile.await_suspension_sites;
    fn.await_keyword_sites = profile.await_keyword_sites;
    fn.await_suspension_point_sites = profile.await_suspension_point_sites;
    fn.await_resume_sites = profile.await_resume_sites;
    fn.await_state_machine_sites = profile.await_state_machine_sites;
    fn.await_continuation_sites = profile.await_continuation_sites;
    fn.await_suspension_normalized_sites = profile.normalized_sites;
    fn.await_suspension_gate_blocked_sites = profile.gate_blocked_sites;
    fn.await_suspension_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_await_suspension_handoff =
        profile.deterministic_await_suspension_handoff;
    fn.await_suspension_profile = BuildAwaitSuspensionProfile(
        fn.await_suspension_sites,
        fn.await_keyword_sites,
        fn.await_suspension_point_sites,
        fn.await_resume_sites,
        fn.await_state_machine_sites,
        fn.await_continuation_sites,
        fn.await_suspension_normalized_sites,
        fn.await_suspension_gate_blocked_sites,
        fn.await_suspension_contract_violation_sites,
        fn.deterministic_await_suspension_handoff);
    fn.await_suspension_profile_is_normalized = IsAwaitSuspensionProfileNormalized(
        fn.await_suspension_sites,
        fn.await_keyword_sites,
        fn.await_suspension_point_sites,
        fn.await_resume_sites,
        fn.await_state_machine_sites,
        fn.await_continuation_sites,
        fn.await_suspension_normalized_sites,
        fn.await_suspension_gate_blocked_sites,
        fn.await_suspension_contract_violation_sites);
  }

  void FinalizeAwaitSuspensionProfile(Objc3MethodDecl &method) {
    const Objc3AwaitSuspensionProfile profile =
        BuildAwaitSuspensionProfileFromOpaqueBody(method);
    method.await_suspension_sites = profile.await_suspension_sites;
    method.await_keyword_sites = profile.await_keyword_sites;
    method.await_suspension_point_sites = profile.await_suspension_point_sites;
    method.await_resume_sites = profile.await_resume_sites;
    method.await_state_machine_sites = profile.await_state_machine_sites;
    method.await_continuation_sites = profile.await_continuation_sites;
    method.await_suspension_normalized_sites = profile.normalized_sites;
    method.await_suspension_gate_blocked_sites = profile.gate_blocked_sites;
    method.await_suspension_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_await_suspension_handoff =
        profile.deterministic_await_suspension_handoff;
    method.await_suspension_profile = BuildAwaitSuspensionProfile(
        method.await_suspension_sites,
        method.await_keyword_sites,
        method.await_suspension_point_sites,
        method.await_resume_sites,
        method.await_state_machine_sites,
        method.await_continuation_sites,
        method.await_suspension_normalized_sites,
        method.await_suspension_gate_blocked_sites,
        method.await_suspension_contract_violation_sites,
        method.deterministic_await_suspension_handoff);
    method.await_suspension_profile_is_normalized = IsAwaitSuspensionProfileNormalized(
        method.await_suspension_sites,
        method.await_keyword_sites,
        method.await_suspension_point_sites,
        method.await_resume_sites,
        method.await_state_machine_sites,
        method.await_continuation_sites,
        method.await_suspension_normalized_sites,
        method.await_suspension_gate_blocked_sites,
        method.await_suspension_contract_violation_sites);
  }

  void FinalizeActorIsolationSendabilityProfile(FunctionDecl &fn) {
    const Objc3ActorIsolationSendabilityProfile profile =
        BuildActorIsolationSendabilityProfileFromFunction(fn);
    fn.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;
    fn.actor_isolation_decl_sites = profile.actor_isolation_decl_sites;
    fn.actor_hop_sites = profile.actor_hop_sites;
    fn.sendable_annotation_sites = profile.sendable_annotation_sites;
    fn.non_sendable_crossing_sites = profile.non_sendable_crossing_sites;
    fn.isolation_boundary_sites = profile.isolation_boundary_sites;
    fn.actor_isolation_sendability_normalized_sites = profile.normalized_sites;
    fn.actor_isolation_sendability_gate_blocked_sites = profile.gate_blocked_sites;
    fn.actor_isolation_sendability_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_actor_isolation_sendability_handoff =
        profile.deterministic_actor_isolation_sendability_handoff;
    fn.actor_isolation_sendability_profile = BuildActorIsolationSendabilityProfile(
        fn.actor_isolation_sendability_sites,
        fn.actor_isolation_decl_sites,
        fn.actor_hop_sites,
        fn.sendable_annotation_sites,
        fn.non_sendable_crossing_sites,
        fn.isolation_boundary_sites,
        fn.actor_isolation_sendability_normalized_sites,
        fn.actor_isolation_sendability_gate_blocked_sites,
        fn.actor_isolation_sendability_contract_violation_sites,
        fn.deterministic_actor_isolation_sendability_handoff);
    fn.actor_isolation_sendability_profile_is_normalized =
        IsActorIsolationSendabilityProfileNormalized(
            fn.actor_isolation_sendability_sites,
            fn.actor_isolation_decl_sites,
            fn.actor_hop_sites,
            fn.sendable_annotation_sites,
            fn.non_sendable_crossing_sites,
            fn.isolation_boundary_sites,
            fn.actor_isolation_sendability_normalized_sites,
            fn.actor_isolation_sendability_gate_blocked_sites,
            fn.actor_isolation_sendability_contract_violation_sites);
  }

  void FinalizeActorIsolationSendabilityProfile(Objc3MethodDecl &method) {
    const Objc3ActorIsolationSendabilityProfile profile =
        BuildActorIsolationSendabilityProfileFromOpaqueBody(method);
    method.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;
    method.actor_isolation_decl_sites = profile.actor_isolation_decl_sites;
    method.actor_hop_sites = profile.actor_hop_sites;
    method.sendable_annotation_sites = profile.sendable_annotation_sites;
    method.non_sendable_crossing_sites = profile.non_sendable_crossing_sites;
    method.isolation_boundary_sites = profile.isolation_boundary_sites;
    method.actor_isolation_sendability_normalized_sites = profile.normalized_sites;
    method.actor_isolation_sendability_gate_blocked_sites = profile.gate_blocked_sites;
    method.actor_isolation_sendability_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_actor_isolation_sendability_handoff =
        profile.deterministic_actor_isolation_sendability_handoff;
    method.actor_isolation_sendability_profile = BuildActorIsolationSendabilityProfile(
        method.actor_isolation_sendability_sites,
        method.actor_isolation_decl_sites,
        method.actor_hop_sites,
        method.sendable_annotation_sites,
        method.non_sendable_crossing_sites,
        method.isolation_boundary_sites,
        method.actor_isolation_sendability_normalized_sites,
        method.actor_isolation_sendability_gate_blocked_sites,
        method.actor_isolation_sendability_contract_violation_sites,
        method.deterministic_actor_isolation_sendability_handoff);
    method.actor_isolation_sendability_profile_is_normalized =
        IsActorIsolationSendabilityProfileNormalized(
            method.actor_isolation_sendability_sites,
            method.actor_isolation_decl_sites,
            method.actor_hop_sites,
            method.sendable_annotation_sites,
            method.non_sendable_crossing_sites,
            method.isolation_boundary_sites,
            method.actor_isolation_sendability_normalized_sites,
            method.actor_isolation_sendability_gate_blocked_sites,
            method.actor_isolation_sendability_contract_violation_sites);
  }

  void FinalizeTaskRuntimeCancellationProfile(FunctionDecl &fn) {
    const Objc3TaskRuntimeCancellationProfile profile =
        BuildTaskRuntimeCancellationProfileFromFunction(fn);
    fn.task_runtime_interop_sites = profile.task_runtime_interop_sites;
    fn.runtime_hook_sites = profile.runtime_hook_sites;
    fn.cancellation_check_sites = profile.cancellation_check_sites;
    fn.cancellation_handler_sites = profile.cancellation_handler_sites;
    fn.suspension_point_sites = profile.suspension_point_sites;
    fn.cancellation_propagation_sites = profile.cancellation_propagation_sites;
    fn.task_runtime_normalized_sites = profile.normalized_sites;
    fn.task_runtime_gate_blocked_sites = profile.gate_blocked_sites;
    fn.task_runtime_contract_violation_sites = profile.contract_violation_sites;
    fn.task_runtime_cancellation_normalized_sites = profile.normalized_sites;
    fn.task_runtime_cancellation_gate_blocked_sites =
        profile.gate_blocked_sites;
    fn.task_runtime_cancellation_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_task_runtime_cancellation_handoff =
        profile.deterministic_task_runtime_cancellation_handoff;
    fn.task_runtime_cancellation_profile = BuildTaskRuntimeCancellationProfile(
        fn.task_runtime_interop_sites,
        fn.runtime_hook_sites,
        fn.cancellation_check_sites,
        fn.cancellation_handler_sites,
        fn.suspension_point_sites,
        fn.cancellation_propagation_sites,
        fn.task_runtime_normalized_sites,
        fn.task_runtime_gate_blocked_sites,
        fn.task_runtime_contract_violation_sites,
        fn.deterministic_task_runtime_cancellation_handoff);
    fn.task_runtime_cancellation_profile_is_normalized =
        IsTaskRuntimeCancellationProfileNormalized(
            fn.task_runtime_interop_sites,
            fn.runtime_hook_sites,
            fn.cancellation_check_sites,
            fn.cancellation_handler_sites,
            fn.suspension_point_sites,
            fn.cancellation_propagation_sites,
            fn.task_runtime_normalized_sites,
            fn.task_runtime_gate_blocked_sites,
            fn.task_runtime_contract_violation_sites);
  }

  void FinalizeTaskRuntimeCancellationProfile(Objc3MethodDecl &method) {
    const Objc3TaskRuntimeCancellationProfile profile =
        BuildTaskRuntimeCancellationProfileFromOpaqueBody(method);
    method.task_runtime_interop_sites = profile.task_runtime_interop_sites;
    method.runtime_hook_sites = profile.runtime_hook_sites;
    method.cancellation_check_sites = profile.cancellation_check_sites;
    method.cancellation_handler_sites = profile.cancellation_handler_sites;
    method.suspension_point_sites = profile.suspension_point_sites;
    method.cancellation_propagation_sites = profile.cancellation_propagation_sites;
    method.task_runtime_normalized_sites = profile.normalized_sites;
    method.task_runtime_gate_blocked_sites = profile.gate_blocked_sites;
    method.task_runtime_contract_violation_sites = profile.contract_violation_sites;
    method.task_runtime_cancellation_normalized_sites = profile.normalized_sites;
    method.task_runtime_cancellation_gate_blocked_sites =
        profile.gate_blocked_sites;
    method.task_runtime_cancellation_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_task_runtime_cancellation_handoff =
        profile.deterministic_task_runtime_cancellation_handoff;
    method.task_runtime_cancellation_profile = BuildTaskRuntimeCancellationProfile(
        method.task_runtime_interop_sites,
        method.runtime_hook_sites,
        method.cancellation_check_sites,
        method.cancellation_handler_sites,
        method.suspension_point_sites,
        method.cancellation_propagation_sites,
        method.task_runtime_normalized_sites,
        method.task_runtime_gate_blocked_sites,
        method.task_runtime_contract_violation_sites,
        method.deterministic_task_runtime_cancellation_handoff);
    method.task_runtime_cancellation_profile_is_normalized =
        IsTaskRuntimeCancellationProfileNormalized(
            method.task_runtime_interop_sites,
            method.runtime_hook_sites,
            method.cancellation_check_sites,
            method.cancellation_handler_sites,
            method.suspension_point_sites,
            method.cancellation_propagation_sites,
            method.task_runtime_normalized_sites,
            method.task_runtime_gate_blocked_sites,
            method.task_runtime_contract_violation_sites);
  }

  void FinalizeConcurrencyReplayRaceGuardProfile(FunctionDecl &fn) {
    const Objc3ConcurrencyReplayRaceGuardProfile profile =
        BuildConcurrencyReplayRaceGuardProfileFromFunction(fn);
    fn.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;
    fn.concurrency_replay_sites = profile.concurrency_replay_sites;
    fn.replay_proof_sites = profile.replay_proof_sites;
    fn.race_guard_sites = profile.race_guard_sites;
    fn.task_handoff_sites = profile.task_handoff_sites;
    fn.actor_isolation_sites = profile.actor_isolation_sites;
    fn.deterministic_schedule_sites = profile.deterministic_schedule_sites;
    fn.concurrency_replay_guard_blocked_sites = profile.guard_blocked_sites;
    fn.concurrency_replay_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_concurrency_replay_race_guard_handoff =
        profile.deterministic_concurrency_replay_race_guard_handoff;
    fn.concurrency_replay_race_guard_profile =
        BuildConcurrencyReplayRaceGuardProfile(
            fn.concurrency_replay_race_guard_sites,
            fn.concurrency_replay_sites,
            fn.replay_proof_sites,
            fn.race_guard_sites,
            fn.task_handoff_sites,
            fn.actor_isolation_sites,
            fn.deterministic_schedule_sites,
            fn.concurrency_replay_guard_blocked_sites,
            fn.concurrency_replay_contract_violation_sites,
            fn.deterministic_concurrency_replay_race_guard_handoff);
    fn.concurrency_replay_race_guard_profile_is_normalized =
        IsConcurrencyReplayRaceGuardProfileNormalized(
            fn.concurrency_replay_race_guard_sites,
            fn.concurrency_replay_sites,
            fn.replay_proof_sites,
            fn.race_guard_sites,
            fn.task_handoff_sites,
            fn.actor_isolation_sites,
            fn.deterministic_schedule_sites,
            fn.concurrency_replay_guard_blocked_sites,
            fn.concurrency_replay_contract_violation_sites);
  }

  void FinalizeConcurrencyReplayRaceGuardProfile(Objc3MethodDecl &method) {
    const Objc3ConcurrencyReplayRaceGuardProfile profile =
        BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(method);
    method.concurrency_replay_race_guard_sites =
        profile.concurrency_replay_race_guard_sites;
    method.concurrency_replay_sites = profile.concurrency_replay_sites;
    method.replay_proof_sites = profile.replay_proof_sites;
    method.race_guard_sites = profile.race_guard_sites;
    method.task_handoff_sites = profile.task_handoff_sites;
    method.actor_isolation_sites = profile.actor_isolation_sites;
    method.deterministic_schedule_sites = profile.deterministic_schedule_sites;
    method.concurrency_replay_guard_blocked_sites = profile.guard_blocked_sites;
    method.concurrency_replay_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_concurrency_replay_race_guard_handoff =
        profile.deterministic_concurrency_replay_race_guard_handoff;
    method.concurrency_replay_race_guard_profile =
        BuildConcurrencyReplayRaceGuardProfile(
            method.concurrency_replay_race_guard_sites,
            method.concurrency_replay_sites,
            method.replay_proof_sites,
            method.race_guard_sites,
            method.task_handoff_sites,
            method.actor_isolation_sites,
            method.deterministic_schedule_sites,
            method.concurrency_replay_guard_blocked_sites,
            method.concurrency_replay_contract_violation_sites,
            method.deterministic_concurrency_replay_race_guard_handoff);
    method.concurrency_replay_race_guard_profile_is_normalized =
        IsConcurrencyReplayRaceGuardProfileNormalized(
            method.concurrency_replay_race_guard_sites,
            method.concurrency_replay_sites,
            method.replay_proof_sites,
            method.race_guard_sites,
            method.task_handoff_sites,
            method.actor_isolation_sites,
            method.deterministic_schedule_sites,
            method.concurrency_replay_guard_blocked_sites,
            method.concurrency_replay_contract_violation_sites);
  }

  void FinalizeUnsafePointerExtensionProfile(FunctionDecl &fn) {
    const Objc3UnsafePointerExtensionProfile profile =
        BuildUnsafePointerExtensionProfileFromFunction(fn);
    fn.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;
    fn.unsafe_keyword_sites = profile.unsafe_keyword_sites;
    fn.pointer_arithmetic_sites = profile.pointer_arithmetic_sites;
    fn.raw_pointer_type_sites = profile.raw_pointer_type_sites;
    fn.unsafe_operation_sites = profile.unsafe_operation_sites;
    fn.unsafe_pointer_extension_normalized_sites = profile.normalized_sites;
    fn.unsafe_pointer_extension_gate_blocked_sites = profile.gate_blocked_sites;
    fn.unsafe_pointer_extension_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_unsafe_pointer_extension_handoff =
        profile.deterministic_unsafe_pointer_extension_handoff;
    fn.unsafe_pointer_extension_profile = BuildUnsafePointerExtensionProfile(
        fn.unsafe_pointer_extension_sites,
        fn.unsafe_keyword_sites,
        fn.pointer_arithmetic_sites,
        fn.raw_pointer_type_sites,
        fn.unsafe_operation_sites,
        fn.unsafe_pointer_extension_normalized_sites,
        fn.unsafe_pointer_extension_gate_blocked_sites,
        fn.unsafe_pointer_extension_contract_violation_sites,
        fn.deterministic_unsafe_pointer_extension_handoff);
    fn.unsafe_pointer_extension_profile_is_normalized =
        IsUnsafePointerExtensionProfileNormalized(
            fn.unsafe_pointer_extension_sites,
            fn.unsafe_keyword_sites,
            fn.pointer_arithmetic_sites,
            fn.raw_pointer_type_sites,
            fn.unsafe_operation_sites,
            fn.unsafe_pointer_extension_normalized_sites,
            fn.unsafe_pointer_extension_gate_blocked_sites,
            fn.unsafe_pointer_extension_contract_violation_sites);
  }

  void FinalizeUnsafePointerExtensionProfile(Objc3MethodDecl &method) {
    const Objc3UnsafePointerExtensionProfile profile =
        BuildUnsafePointerExtensionProfileFromOpaqueBody(method);
    method.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;
