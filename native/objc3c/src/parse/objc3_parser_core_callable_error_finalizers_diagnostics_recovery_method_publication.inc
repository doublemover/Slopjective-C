  void PublishErrorDiagnosticsRecoverySiteCounters(
      Objc3MethodDecl &method,
      const Objc3ErrorDiagnosticsRecoveryProfile &profile) {
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
  }
