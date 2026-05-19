void PopulateObjc3ParserSemaHandoffPublicationResult(
    const Objc3ParserSemaHandoffScaffold &handoff,
    const Objc3SemaParserHandoffPublication &publication,
    Objc3SemaPassManagerResult &result) {
  result.parser_sema_handoff_owner_record = handoff.owner_record;
  result.deterministic_parser_sema_handoff_owner_record =
      publication.owner_record_deterministic;
  result.parser_contract_snapshot = handoff.parser_contract_snapshot;

  result.parser_sema_conformance_matrix = handoff.parser_sema_conformance_matrix;
  result.deterministic_parser_sema_conformance_matrix =
      publication.transfer_record.conformance_matrix_ready;

  result.parser_sema_conformance_corpus = handoff.parser_sema_conformance_corpus;
  result.deterministic_parser_sema_conformance_corpus =
      publication.transfer_record.conformance_corpus_ready;

  result.parser_sema_performance_quality_guardrails =
      handoff.parser_sema_performance_quality_guardrails;
  result.deterministic_parser_sema_performance_quality_guardrails =
      publication.transfer_record.performance_quality_guardrails_ready;

  result.parser_sema_cross_lane_integration_sync =
      handoff.parser_sema_cross_lane_integration_sync;
  result.deterministic_parser_sema_cross_lane_integration_sync =
      publication.transfer_record.cross_lane_integration_sync_ready;

  result.parser_sema_docs_runbook_sync = handoff.parser_sema_docs_runbook_sync;
  result.deterministic_parser_sema_docs_runbook_sync =
      publication.transfer_record.docs_runbook_sync_ready;

  result.parser_sema_release_candidate_replay_dry_run =
      handoff.parser_sema_release_candidate_replay_dry_run;
  result.deterministic_parser_sema_release_candidate_replay_dry_run =
      publication.transfer_record.release_candidate_replay_ready;

  result.parser_sema_advanced_core_shard1 =
      handoff.parser_sema_advanced_core_shard1;
  result.deterministic_parser_sema_advanced_core_shard1 =
      publication.transfer_record.advanced_core_shard1_ready;

  result.parser_sema_advanced_contract_rejection_shard1 =
      handoff.parser_sema_advanced_contract_rejection_shard1;
  result.deterministic_parser_sema_advanced_contract_rejection_shard1 =
      publication.transfer_record.advanced_contract_rejection_shard1_ready;

  result.parser_sema_advanced_diagnostics_shard1 =
      handoff.parser_sema_advanced_diagnostics_shard1;
  result.deterministic_parser_sema_advanced_diagnostics_shard1 =
      publication.transfer_record.advanced_diagnostics_shard1_ready;

  result.parser_sema_advanced_conformance_shard1 =
      handoff.parser_sema_advanced_conformance_shard1;
  result.deterministic_parser_sema_advanced_conformance_shard1 =
      publication.transfer_record.advanced_conformance_shard1_ready;

  result.parser_sema_advanced_integration_shard1 =
      handoff.parser_sema_advanced_integration_shard1;
  result.deterministic_parser_sema_advanced_integration_shard1 =
      publication.transfer_record.advanced_integration_shard1_ready;

  result.parser_sema_advanced_performance_shard1 =
      handoff.parser_sema_advanced_performance_shard1;
  result.deterministic_parser_sema_advanced_performance_shard1 =
      publication.transfer_record.advanced_performance_shard1_ready;

  result.parser_sema_advanced_core_shard2 =
      handoff.parser_sema_advanced_core_shard2;
  result.deterministic_parser_sema_advanced_core_shard2 =
      publication.transfer_record.advanced_core_shard2_ready;

  result.parser_sema_advanced_contract_rejection_shard2 =
      handoff.parser_sema_advanced_contract_rejection_shard2;
  result.deterministic_parser_sema_advanced_contract_rejection_shard2 =
      publication.transfer_record.advanced_contract_rejection_shard2_ready;

  result.parser_sema_advanced_diagnostics_shard2 =
      handoff.parser_sema_advanced_diagnostics_shard2;
  result.deterministic_parser_sema_advanced_diagnostics_shard2 =
      publication.transfer_record.advanced_diagnostics_shard2_ready;

  result.parser_sema_integration_closeout_signoff =
      handoff.parser_sema_integration_closeout_signoff;
  result.deterministic_parser_sema_integration_closeout_signoff =
      publication.transfer_record.integration_closeout_ready;

  result.deterministic_parser_sema_handoff =
      publication.transfer_record.scaffold_readiness_ready;
  result.parser_sema_handoff_publication_evidence_record =
      publication.evidence_record;
  result.deterministic_parser_sema_handoff_publication_evidence_record =
      publication.evidence_record_deterministic;
  result.parser_sema_handoff_publication_transfer_record =
      publication.transfer_record;
  result.deterministic_parser_sema_handoff_publication_transfer_record =
      publication.transfer_record_deterministic;
}
