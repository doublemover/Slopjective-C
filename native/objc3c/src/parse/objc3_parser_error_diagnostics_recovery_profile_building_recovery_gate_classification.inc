bool ClassifyErrorDiagnosticsRecoveryGate(
    Objc3ErrorDiagnosticsRecoveryProfile &profile) {
  const bool gate_open =
      profile.diagnostic_emit_sites > 0u && profile.recovery_anchor_sites > 0u;
  std::size_t blocked_total = profile.recovery_boundary_sites;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          blocked_total,
          profile.fail_closed_diagnostic_sites,
          blocked_total)) {
    blocked_total = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  }
  profile.gate_blocked_sites =
      gate_open ? 0u
                : std::min(profile.error_diagnostics_recovery_sites,
                           blocked_total);
  profile.normalized_sites =
      profile.error_diagnostics_recovery_sites - profile.gate_blocked_sites;
  return gate_open;
}
