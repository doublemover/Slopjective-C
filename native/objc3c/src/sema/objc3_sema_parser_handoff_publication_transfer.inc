std::size_t Objc3ParserSemaPublicationTransferCount(bool ready) {
  return ready ? 1u : 0u;
}

Objc3ParserSemaHandoffPublicationTransferRecord
BuildObjc3ParserSemaHandoffPublicationTransferRecord(
    const Objc3ParserSemaHandoffScaffold &handoff,
    const Objc3ParserSemaHandoffPublicationEvidenceRecord &evidence_record) {
  Objc3ParserSemaHandoffPublicationTransferRecord record;
  const Objc3ParserSemaConformanceEvidenceRecord &conformance_evidence =
      handoff.parser_sema_conformance_evidence_record;
  const Objc3ParserSemaContractReadinessRecord &contract_readiness =
      handoff.parser_sema_contract_readiness_record;
  const bool conformance_evidence_ready =
      handoff.deterministic_parser_sema_conformance_evidence_record &&
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(conformance_evidence);
  const bool contract_readiness_ready =
      handoff.deterministic_parser_sema_contract_readiness_record &&
      IsReadyObjc3ParserSemaContractReadinessRecord(contract_readiness);
  record.stage_input_owner = handoff.owner_record.stage_input_owner;
  record.parser_sema_contract_handoff_owner =
      handoff.owner_record.parser_sema_contract_handoff_owner;
  record.owner_model = handoff.owner_record.owner_model;
  record.strict_no_retired_route = handoff.owner_record.strict_no_retired_route;
  record.strict_no_compatibility =
      handoff.owner_record.strict_no_compatibility;
  record.owner_record_ready =
      IsReadyObjc3ParserSemaHandoffOwnerRecord(handoff.owner_record);
  record.conformance_matrix_ready =
      conformance_evidence_ready &&
      conformance_evidence.conformance_matrix_deterministic;
  record.conformance_corpus_ready =
      conformance_evidence_ready &&
      conformance_evidence.conformance_corpus_deterministic;
  record.performance_quality_guardrails_ready =
      contract_readiness_ready &&
      contract_readiness.performance_quality_guardrails_ready;
  record.cross_lane_integration_sync_ready =
      contract_readiness_ready &&
      contract_readiness.cross_lane_integration_sync_ready;
  record.docs_runbook_sync_ready =
      contract_readiness_ready && contract_readiness.docs_runbook_sync_ready;
  record.release_candidate_replay_ready =
      contract_readiness_ready &&
      contract_readiness.release_candidate_replay_dry_run_ready;
  record.advanced_core_shard1_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_core_shard1_ready;
  record.advanced_contract_rejection_shard1_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_contract_rejection_shard1_ready;
  record.advanced_diagnostics_shard1_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_diagnostics_shard1_ready;
  record.advanced_conformance_shard1_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_conformance_shard1_ready;
  record.advanced_integration_shard1_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_integration_shard1_ready;
  record.advanced_performance_shard1_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_performance_shard1_ready;
  record.advanced_core_shard2_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_core_shard2_ready;
  record.advanced_contract_rejection_shard2_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_contract_rejection_shard2_ready;
  record.advanced_diagnostics_shard2_ready =
      contract_readiness_ready &&
      contract_readiness.advanced_diagnostics_shard2_ready;
  record.integration_closeout_ready =
      contract_readiness_ready && contract_readiness.integration_closeout_ready;
  record.scaffold_readiness_ready =
      handoff.readiness_record_deterministic &&
      IsReadyObjc3ParserSemaHandoffScaffoldReadinessRecord(
          handoff.readiness_record) &&
      handoff.deterministic;
  record.evidence_record_ready =
      IsReadyObjc3ParserSemaHandoffPublicationEvidenceRecord(evidence_record);
  record.passed_transfer_count =
      Objc3ParserSemaPublicationTransferCount(record.owner_record_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.conformance_matrix_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.conformance_corpus_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.performance_quality_guardrails_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.cross_lane_integration_sync_ready) +
      Objc3ParserSemaPublicationTransferCount(record.docs_runbook_sync_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.release_candidate_replay_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_core_shard1_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_contract_rejection_shard1_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_diagnostics_shard1_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_conformance_shard1_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_integration_shard1_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_performance_shard1_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_core_shard2_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_contract_rejection_shard2_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.advanced_diagnostics_shard2_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.integration_closeout_ready) +
      Objc3ParserSemaPublicationTransferCount(
          record.scaffold_readiness_ready) +
      Objc3ParserSemaPublicationTransferCount(record.evidence_record_ready);
  record.failed_transfer_count =
      record.required_transfer_count >= record.passed_transfer_count
          ? (record.required_transfer_count - record.passed_transfer_count)
          : record.required_transfer_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.handoff_publication_transfer_owner) &&
      Objc3SemaOwnerIsExplicit(record.handoff_publication_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.handoff_scaffold_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_transfer_count == 19u &&
      record.passed_transfer_count == record.required_transfer_count &&
      record.failed_transfer_count == 0u;
  return record;
}
