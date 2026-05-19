static std::string BuildBootstrapLegalityFailureContractReplayKey(
    const Objc3BootstrapLegalityFailureContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";surface_path=" << summary.surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol;
  return out.str();
}

static Objc3BootstrapLegalityFailureContractSummary
BuildBootstrapLegalityFailureContractSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapLegalityFailureContractSummary summary;
  // semantic migration anchor: canonical-mode legacy literal diagnostics stays
  // truthful only if the live fail-closed legacy-literal rejection surface is
  // preserved through the canonical selection claim summary exported here.
  summary.fail_closed = true;
  summary.duplicate_registration_policy_frozen = true;
  summary.image_order_invariant_frozen = true;
  summary.bootstrap_rejection_frozen = true;
  summary.restart_boundary_frozen = true;
  summary.semantic_diagnostics_required = true;
  summary.ready_for_lowering_and_runtime =
      surface.interface_implementation_summary.deterministic;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key = BuildBootstrapLegalityFailureContractReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapLegalityFailureContractSummary(summary)) {
    summary.failure_reason =
        "bootstrap legality duplicate/order/failure contract summary is incomplete";
  }
  return summary;
}

static std::string BuildBootstrapLegalitySemanticsReplayKey(
    const Objc3BootstrapLegalitySemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_failure_contract_id="
      << summary.bootstrap_legality_failure_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";surface_path=" << summary.surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";cross_image_legality_model=" << summary.cross_image_legality_model
      << ";semantic_diagnostic_model=" << summary.semantic_diagnostic_model
      << ";bootstrap_legality_failure_replay_key="
      << summary.bootstrap_legality_failure_replay_key;
  return out.str();
}

static Objc3BootstrapLegalitySemanticsSummary
BuildBootstrapLegalitySemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapLegalitySemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_legality_failure_contract_ready =
      IsReadyObjc3BootstrapLegalityFailureContractSummary(
          surface.bootstrap_legality_failure_contract_summary);
  if (summary.bootstrap_legality_failure_contract_ready) {
    summary.duplicate_registration_policy =
        surface.bootstrap_legality_failure_contract_summary
            .duplicate_registration_policy;
    summary.image_registration_order_invariant =
        surface.bootstrap_legality_failure_contract_summary
            .image_registration_order_invariant;
    summary.bootstrap_legality_failure_replay_key =
        surface.bootstrap_legality_failure_contract_summary.replay_key;
  }
  summary.duplicate_registration_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.image_order_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.cross_image_legality_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.semantic_diagnostics_landed =
      summary.bootstrap_legality_failure_contract_ready &&
      surface.bootstrap_legality_failure_contract_summary
          .semantic_diagnostics_required;
  summary.ready_for_lowering_and_runtime =
      summary.bootstrap_legality_failure_contract_ready &&
      surface.bootstrap_legality_failure_contract_summary
          .ready_for_lowering_and_runtime;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key = BuildBootstrapLegalitySemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapLegalitySemanticsSummary(summary)) {
    summary.failure_reason =
        "bootstrap legality duplicate/order semantics summary is incomplete";
  }
  return summary;
}

static std::string BuildBootstrapFailureRestartSemanticsReplayKey(
    const Objc3BootstrapFailureRestartSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_semantics_contract_id="
      << summary.bootstrap_legality_semantics_contract_id
      << ";bootstrap_reset_contract_id=" << summary.bootstrap_reset_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";surface_path=" << summary.surface_path
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";unsupported_topology_model=" << summary.unsupported_topology_model
      << ";bootstrap_legality_semantics_replay_key="
      << summary.bootstrap_legality_semantics_replay_key;
  return out.str();
}

static Objc3BootstrapFailureRestartSemanticsSummary
BuildBootstrapFailureRestartSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapFailureRestartSemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_legality_semantics_ready =
      IsReadyObjc3BootstrapLegalitySemanticsSummary(
          surface.bootstrap_legality_semantics_summary);
  if (summary.bootstrap_legality_semantics_ready) {
    summary.failure_mode =
        surface.bootstrap_legality_failure_contract_summary.failure_mode;
    summary.restart_lifecycle_model =
        surface.bootstrap_legality_failure_contract_summary
            .restart_lifecycle_model;
    summary.replay_order_model =
        surface.bootstrap_legality_failure_contract_summary.replay_order_model;
    summary.image_local_init_reset_model =
        surface.bootstrap_legality_failure_contract_summary
            .image_local_init_reset_model;
    summary.catalog_retention_model =
        surface.bootstrap_legality_failure_contract_summary
            .catalog_retention_model;
    summary.bootstrap_legality_semantics_replay_key =
        surface.bootstrap_legality_semantics_summary.replay_key;
  }
  summary.failure_mode_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.restart_semantics_landed = summary.bootstrap_legality_semantics_ready;
  summary.replay_semantics_landed = summary.bootstrap_legality_semantics_ready;
  summary.unsupported_topology_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.deterministic_recovery_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.ready_for_lowering_and_runtime =
      summary.bootstrap_legality_semantics_ready &&
      surface.bootstrap_legality_semantics_summary.ready_for_lowering_and_runtime;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildBootstrapFailureRestartSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapFailureRestartSemanticsSummary(summary)) {
    summary.failure_reason =
        "bootstrap failure-mode and restart semantics summary is incomplete";
  }
  return summary;
}
