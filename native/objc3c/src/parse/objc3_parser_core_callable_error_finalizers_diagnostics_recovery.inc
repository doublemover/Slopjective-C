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
