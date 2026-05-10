#include "sema/objc3_sema_parser_handoff_publication.h"

#include <sstream>

namespace {

std::string BuildObjc3ParserSemaHandoffPublicationRecoveryReplayKey(
    const Objc3ParserSemaConformanceMatrix &matrix,
    const Objc3ParserSemaConformanceCorpus &corpus) {
  std::ostringstream recovery_replay_key_stream;
  recovery_replay_key_stream
      << "sema-pass-recovery:v1:"
      << "matrix_recovery_replay_ready="
      << (matrix.parser_recovery_replay_ready ? "true" : "false")
      << ";corpus_recovery_replay_case_present="
      << (corpus.has_recovery_replay_case ? "true" : "false")
      << ";corpus_recovery_replay_case_passed="
      << (corpus.recovery_replay_case_passed ? "true" : "false")
      << ";corpus_required_case_count=" << corpus.required_case_count
      << ";corpus_passed_case_count=" << corpus.passed_case_count
      << ";corpus_failed_case_count=" << corpus.failed_case_count;
  return recovery_replay_key_stream.str();
}

Objc3ParserSemaHandoffPublicationEvidenceRecord
BuildObjc3ParserSemaHandoffPublicationEvidenceRecord(
    const Objc3ParserSemaHandoffScaffold &handoff) {
  Objc3ParserSemaHandoffPublicationEvidenceRecord record;
  const Objc3ParserSemaConformanceMatrix &matrix =
      handoff.parser_sema_conformance_matrix;
  const Objc3ParserSemaConformanceCorpus &corpus =
      handoff.parser_sema_conformance_corpus;
  record.stage_input_owner = handoff.owner_record.stage_input_owner;
  record.parser_sema_contract_handoff_owner =
      handoff.owner_record.parser_sema_contract_handoff_owner;
  record.owner_model = handoff.owner_record.owner_model;
  record.strict_no_retired_route = handoff.owner_record.strict_no_retired_route;
  record.strict_no_compatibility =
      handoff.owner_record.strict_no_compatibility;
  record.conformance_matrix_ready = matrix.deterministic;
  record.conformance_corpus_ready = corpus.deterministic;
  record.parser_recovery_replay_ready = matrix.parser_recovery_replay_ready;
  record.parser_recovery_replay_case_present =
      corpus.has_recovery_replay_case;
  record.parser_recovery_replay_case_passed =
      corpus.recovery_replay_case_passed;
  record.corpus_required_case_count = corpus.required_case_count;
  record.corpus_passed_case_count = corpus.passed_case_count;
  record.corpus_failed_case_count = corpus.failed_case_count;
  record.corpus_case_counts_ready =
      record.corpus_required_case_count > 0u &&
      record.corpus_passed_case_count == record.corpus_required_case_count &&
      record.corpus_failed_case_count == 0u;
  record.parser_recovery_replay_contract_satisfied =
      record.parser_recovery_replay_ready &&
      record.parser_recovery_replay_case_present &&
      record.parser_recovery_replay_case_passed &&
      record.corpus_case_counts_ready;
  record.recovery_replay_key =
      BuildObjc3ParserSemaHandoffPublicationRecoveryReplayKey(matrix, corpus);
  record.recovery_replay_key_deterministic =
      record.conformance_matrix_ready && record.conformance_corpus_ready &&
      !record.recovery_replay_key.empty();
  record.recovery_determinism_hardening_satisfied =
      record.parser_recovery_replay_contract_satisfied &&
      record.recovery_replay_key_deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.handoff_publication_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.conformance_matrix_ready && record.conformance_corpus_ready &&
      record.parser_recovery_replay_ready &&
      record.parser_recovery_replay_case_present &&
      record.parser_recovery_replay_case_passed &&
      record.corpus_case_counts_ready &&
      record.parser_recovery_replay_contract_satisfied &&
      record.recovery_replay_key.rfind("sema-pass-recovery:v1:", 0) == 0 &&
      record.recovery_replay_key_deterministic &&
      record.recovery_determinism_hardening_satisfied;
  return record;
}

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

}  // namespace

Objc3SemaParserHandoffPublication PublishObjc3ParserSemaHandoff(
    const Objc3ParserSemaHandoffScaffold &handoff,
    Objc3SemaPassManagerResult &result) {
  Objc3SemaParserHandoffPublication publication;

  publication.owner_record = handoff.owner_record;
  publication.evidence_record =
      BuildObjc3ParserSemaHandoffPublicationEvidenceRecord(handoff);
  publication.transfer_record =
      BuildObjc3ParserSemaHandoffPublicationTransferRecord(
          handoff, publication.evidence_record);
  publication.owner_record_deterministic =
      publication.transfer_record.owner_record_ready;
  publication.evidence_record_deterministic =
      publication.transfer_record.evidence_record_ready;
  publication.transfer_record_deterministic =
      IsReadyObjc3ParserSemaHandoffPublicationTransferRecord(
          publication.transfer_record);

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
  publication.ready = publication.transfer_record_deterministic;
  return publication;
}
