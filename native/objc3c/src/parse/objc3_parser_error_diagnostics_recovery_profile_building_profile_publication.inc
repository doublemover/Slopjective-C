Objc3ErrorDiagnosticsRecoveryProfile
BuildErrorDiagnosticsRecoveryProfileFromCounts(
    std::size_t diagnostic_emit_sites,
    std::size_t recovery_anchor_sites,
    std::size_t recovery_boundary_sites,
    std::size_t fail_closed_diagnostic_sites) {
  Objc3ErrorDiagnosticsRecoveryProfile profile;
  InitializeErrorDiagnosticsRecoveryProfileCounts(
      profile,
      diagnostic_emit_sites,
      recovery_anchor_sites,
      recovery_boundary_sites,
      fail_closed_diagnostic_sites);
  const bool gate_open = ClassifyErrorDiagnosticsRecoveryGate(profile);
  NormalizeErrorDiagnosticsRecoveryProfileContract(profile, gate_open);
  return profile;
}
