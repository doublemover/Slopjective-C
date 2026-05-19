bool IsErrorDiagnosticsRecoveryProfileNormalized(
    std::size_t error_diagnostics_recovery_sites,
    std::size_t diagnostic_emit_sites,
    std::size_t recovery_anchor_sites,
    std::size_t recovery_boundary_sites,
    std::size_t fail_closed_diagnostic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (diagnostic_emit_sites > error_diagnostics_recovery_sites ||
      recovery_anchor_sites > error_diagnostics_recovery_sites ||
      recovery_boundary_sites > error_diagnostics_recovery_sites ||
      fail_closed_diagnostic_sites > error_diagnostics_recovery_sites ||
      normalized_sites > error_diagnostics_recovery_sites ||
      gate_blocked_sites > error_diagnostics_recovery_sites ||
      contract_violation_sites > error_diagnostics_recovery_sites) {
    return false;
  }
  std::size_t normalized_total = 0u;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          normalized_sites, gate_blocked_sites, normalized_total)) {
    return false;
  }
  if (normalized_total != error_diagnostics_recovery_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}
