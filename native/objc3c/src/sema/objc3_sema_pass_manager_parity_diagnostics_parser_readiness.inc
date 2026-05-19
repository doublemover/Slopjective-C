  result.parity_surface.diagnostics_accounting_consistent = result.diagnostics_accounting_consistent;
  result.parity_surface.diagnostics_bus_publish_consistent = result.diagnostics_bus_publish_consistent;
  result.parity_surface.diagnostics_canonicalized = result.diagnostics_canonicalized;
  result.parity_surface.diagnostics_after_pass_monotonic =
      IsMonotonicObjc3SemaDiagnosticsAfterPass(result.diagnostics_after_pass);
  result.parity_surface.diagnostics_hardening_satisfied =
      result.diagnostics_hardening_satisfied &&
      result.parity_surface.diagnostics_accounting_consistent &&
      result.parity_surface.diagnostics_bus_publish_consistent &&
      result.parity_surface.diagnostics_canonicalized &&
      result.parity_surface.diagnostics_after_pass_monotonic;
  result.parity_surface.pass_flow_recovery_replay_contract_satisfied =
      result.sema_pass_flow_summary.recovery_replay_contract_satisfied;
  result.parity_surface.pass_flow_recovery_replay_key =
      result.sema_pass_flow_summary.recovery_replay_key;
  result.parity_surface.pass_flow_recovery_replay_key_deterministic =
      result.sema_pass_flow_summary.recovery_replay_key_deterministic;
  result.parity_surface.pass_flow_recovery_determinism_hardening_satisfied =
      result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied;
  result.parity_surface.deterministic_diagnostics_publication_record =
      result.deterministic_diagnostics_publication_record &&
      IsReadyObjc3SemaDiagnosticsPublicationRecord(
          result.parity_surface.diagnostics_publication_record);
  result.parity_surface.deterministic_pass_flow_recovery_record =
      result.deterministic_pass_flow_recovery_record &&
      IsReadyObjc3SemaPassFlowRecoveryRecord(
          result.parity_surface.pass_flow_recovery_record);
  result.parity_surface.deterministic_pass_manager_publication_record =
      result.deterministic_pass_manager_publication_record &&
      IsReadyObjc3SemaPassManagerPublicationRecord(
          result.parity_surface.pass_manager_publication_record);
  result.parity_surface.deterministic_type_metadata_publication_record =
      result.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          result.parity_surface.type_metadata_publication_record);
  result.parity_surface.parser_sema_handoff_publication_transfer_record =
      result.parser_sema_handoff_publication_transfer_record;
  result.parity_surface
      .deterministic_parser_sema_handoff_publication_transfer_record =
      result.deterministic_parser_sema_handoff_publication_transfer_record &&
      IsReadyObjc3ParserSemaHandoffPublicationTransferRecord(
          result.parity_surface
              .parser_sema_handoff_publication_transfer_record);
  result.parser_sema_parity_publication_readiness_record =
      BuildObjc3ParserSemaParityPublicationReadinessRecord(
          input,
          result.parity_surface
              .parser_sema_handoff_publication_transfer_record,
          result.parity_surface
              .deterministic_parser_sema_handoff_publication_transfer_record);
  result.deterministic_parser_sema_parity_publication_readiness_record =
      IsReadyObjc3ParserSemaParityPublicationReadinessRecord(
          result.parser_sema_parity_publication_readiness_record);
  result.parity_surface.parser_sema_parity_publication_readiness_record =
      result.parser_sema_parity_publication_readiness_record;
  result.parity_surface
      .deterministic_parser_sema_parity_publication_readiness_record =
      result.deterministic_parser_sema_parity_publication_readiness_record;
  result.parity_surface.deterministic_parser_sema_conformance_matrix =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .conformance_matrix_ready;
  result.parity_surface.deterministic_parser_sema_conformance_corpus =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .conformance_corpus_ready;
  result.parity_surface.deterministic_parser_sema_performance_quality_guardrails =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .performance_quality_guardrails_ready;
  result.parity_surface.deterministic_parser_sema_cross_lane_integration_sync =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .cross_lane_integration_sync_ready;
  result.parity_surface.deterministic_parser_sema_docs_runbook_sync =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .docs_runbook_sync_ready;
  result.parity_surface.deterministic_parser_sema_release_candidate_replay_dry_run =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .release_candidate_replay_ready;
  result.parity_surface.deterministic_parser_sema_advanced_core_shard1 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_core_shard1_ready;
  result.parity_surface.deterministic_parser_sema_advanced_contract_rejection_shard1 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_contract_rejection_shard1_ready;
  result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard1 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_diagnostics_shard1_ready;
  result.parity_surface.deterministic_parser_sema_advanced_conformance_shard1 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_conformance_shard1_ready;
  result.parity_surface.deterministic_parser_sema_advanced_integration_shard1 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_integration_shard1_ready;
  result.parity_surface.deterministic_parser_sema_advanced_performance_shard1 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_performance_shard1_ready;
  result.parity_surface.deterministic_parser_sema_advanced_core_shard2 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_core_shard2_ready;
  result.parity_surface.deterministic_parser_sema_advanced_contract_rejection_shard2 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_contract_rejection_shard2_ready;
  result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard2 =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .advanced_diagnostics_shard2_ready;
  result.parity_surface.deterministic_parser_sema_integration_closeout_signoff =
      result.parity_surface.parser_sema_parity_publication_readiness_record
          .integration_closeout_ready;
