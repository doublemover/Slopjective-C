#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3ParserSemaParityPublicationReadinessRecord
BuildObjc3ParserSemaParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffPublicationTransferRecord &transfer_record,
    bool deterministic_transfer_record) {
  Objc3ParserSemaParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.handoff_publication_transfer_ready =
      deterministic_transfer_record &&
      IsReadyObjc3ParserSemaHandoffPublicationTransferRecord(transfer_record);
  record.conformance_matrix_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.conformance_matrix_ready;
  record.conformance_corpus_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.conformance_corpus_ready;
  record.performance_quality_guardrails_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.performance_quality_guardrails_ready;
  record.cross_lane_integration_sync_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.cross_lane_integration_sync_ready;
  record.docs_runbook_sync_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.docs_runbook_sync_ready;
  record.release_candidate_replay_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.release_candidate_replay_ready;
  record.advanced_core_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_core_shard1_ready;
  record.advanced_contract_rejection_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_contract_rejection_shard1_ready;
  record.advanced_diagnostics_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_diagnostics_shard1_ready;
  record.advanced_conformance_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_conformance_shard1_ready;
  record.advanced_integration_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_integration_shard1_ready;
  record.advanced_performance_shard1_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_performance_shard1_ready;
  record.advanced_core_shard2_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_core_shard2_ready;
  record.advanced_contract_rejection_shard2_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_contract_rejection_shard2_ready;
  record.advanced_diagnostics_shard2_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.advanced_diagnostics_shard2_ready;
  record.integration_closeout_ready =
      record.handoff_publication_transfer_ready &&
      transfer_record.integration_closeout_ready;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.handoff_publication_transfer_ready) +
      Objc3SemaEvidenceCount(record.conformance_matrix_ready) +
      Objc3SemaEvidenceCount(record.conformance_corpus_ready) +
      Objc3SemaEvidenceCount(record.performance_quality_guardrails_ready) +
      Objc3SemaEvidenceCount(record.cross_lane_integration_sync_ready) +
      Objc3SemaEvidenceCount(record.docs_runbook_sync_ready) +
      Objc3SemaEvidenceCount(record.release_candidate_replay_ready) +
      Objc3SemaEvidenceCount(record.advanced_core_shard1_ready) +
      Objc3SemaEvidenceCount(
          record.advanced_contract_rejection_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_diagnostics_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_conformance_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_integration_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_performance_shard1_ready) +
      Objc3SemaEvidenceCount(record.advanced_core_shard2_ready) +
      Objc3SemaEvidenceCount(
          record.advanced_contract_rejection_shard2_ready) +
      Objc3SemaEvidenceCount(record.advanced_diagnostics_shard2_ready) +
      Objc3SemaEvidenceCount(record.integration_closeout_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.handoff_publication_transfer_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_publication_count == 17u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}
