void NormalizeErrorDiagnosticsRecoveryProfileContract(
    Objc3ErrorDiagnosticsRecoveryProfile &profile,
    bool gate_open) {
  if (profile.diagnostic_emit_sites >
          profile.error_diagnostics_recovery_sites ||
      profile.recovery_anchor_sites >
          profile.error_diagnostics_recovery_sites ||
      profile.recovery_boundary_sites >
          profile.error_diagnostics_recovery_sites ||
      profile.fail_closed_diagnostic_sites >
          profile.error_diagnostics_recovery_sites ||
      profile.normalized_sites > profile.error_diagnostics_recovery_sites ||
      profile.gate_blocked_sites > profile.error_diagnostics_recovery_sites ||
      profile.contract_violation_sites >
          profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites += 1u;
  }
  std::size_t normalized_total = 0u;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.normalized_sites,
          profile.gate_blocked_sites,
          normalized_total) ||
      normalized_total != profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (gate_open && profile.gate_blocked_sites != 0u) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites >
      profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites =
        profile.error_diagnostics_recovery_sites;
  }
  profile.deterministic_error_diagnostics_recovery_handoff =
      profile.contract_violation_sites == 0u;
}
