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
  record.strict_no_fallback = handoff.owner_record.strict_no_fallback;
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
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
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

}  // namespace

Objc3SemaParserHandoffPublication PublishObjc3ParserSemaHandoff(
    const Objc3ParserSemaHandoffScaffold &handoff,
    Objc3SemaPassManagerResult &result) {
  Objc3SemaParserHandoffPublication publication;

  publication.owner_record = handoff.owner_record;
  publication.owner_record_deterministic =
      IsReadyObjc3ParserSemaHandoffOwnerRecord(handoff.owner_record);
  result.parser_sema_handoff_owner_record = handoff.owner_record;
  result.deterministic_parser_sema_handoff_owner_record =
      publication.owner_record_deterministic;
  if (!result.deterministic_parser_sema_handoff_owner_record) {
    return publication;
  }

  result.parser_contract_snapshot = handoff.parser_contract_snapshot;
  result.parser_sema_conformance_matrix = handoff.parser_sema_conformance_matrix;
  result.deterministic_parser_sema_conformance_matrix =
      handoff.parser_sema_conformance_matrix.deterministic;
  if (!result.deterministic_parser_sema_conformance_matrix) {
    return publication;
  }

  result.parser_sema_conformance_corpus = handoff.parser_sema_conformance_corpus;
  result.deterministic_parser_sema_conformance_corpus =
      handoff.parser_sema_conformance_corpus.deterministic;
  if (!result.deterministic_parser_sema_conformance_corpus) {
    return publication;
  }

  result.parser_sema_performance_quality_guardrails =
      handoff.parser_sema_performance_quality_guardrails;
  result.deterministic_parser_sema_performance_quality_guardrails =
      handoff.parser_sema_performance_quality_guardrails.deterministic;
  if (!result.deterministic_parser_sema_performance_quality_guardrails) {
    return publication;
  }

  result.parser_sema_cross_lane_integration_sync =
      handoff.parser_sema_cross_lane_integration_sync;
  result.deterministic_parser_sema_cross_lane_integration_sync =
      handoff.parser_sema_cross_lane_integration_sync.deterministic;
  if (!result.deterministic_parser_sema_cross_lane_integration_sync) {
    return publication;
  }

  result.parser_sema_docs_runbook_sync = handoff.parser_sema_docs_runbook_sync;
  result.deterministic_parser_sema_docs_runbook_sync =
      handoff.parser_sema_docs_runbook_sync.deterministic;
  if (!result.deterministic_parser_sema_docs_runbook_sync) {
    return publication;
  }

  result.parser_sema_release_candidate_replay_dry_run =
      handoff.parser_sema_release_candidate_replay_dry_run;
  result.deterministic_parser_sema_release_candidate_replay_dry_run =
      handoff.parser_sema_release_candidate_replay_dry_run.deterministic;
  if (!result.deterministic_parser_sema_release_candidate_replay_dry_run) {
    return publication;
  }

  result.parser_sema_advanced_core_shard1 =
      handoff.parser_sema_advanced_core_shard1;
  result.deterministic_parser_sema_advanced_core_shard1 =
      handoff.parser_sema_advanced_core_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_core_shard1) {
    return publication;
  }

  result.parser_sema_advanced_contract_rejection_shard1 =
      handoff.parser_sema_advanced_contract_rejection_shard1;
  result.deterministic_parser_sema_advanced_contract_rejection_shard1 =
      handoff.parser_sema_advanced_contract_rejection_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_contract_rejection_shard1) {
    return publication;
  }

  result.parser_sema_advanced_diagnostics_shard1 =
      handoff.parser_sema_advanced_diagnostics_shard1;
  result.deterministic_parser_sema_advanced_diagnostics_shard1 =
      handoff.parser_sema_advanced_diagnostics_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_diagnostics_shard1) {
    return publication;
  }

  result.parser_sema_advanced_conformance_shard1 =
      handoff.parser_sema_advanced_conformance_shard1;
  result.deterministic_parser_sema_advanced_conformance_shard1 =
      handoff.parser_sema_advanced_conformance_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_conformance_shard1) {
    return publication;
  }

  result.parser_sema_advanced_integration_shard1 =
      handoff.parser_sema_advanced_integration_shard1;
  result.deterministic_parser_sema_advanced_integration_shard1 =
      handoff.parser_sema_advanced_integration_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_integration_shard1) {
    return publication;
  }

  result.parser_sema_advanced_performance_shard1 =
      handoff.parser_sema_advanced_performance_shard1;
  result.deterministic_parser_sema_advanced_performance_shard1 =
      handoff.parser_sema_advanced_performance_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_performance_shard1) {
    return publication;
  }

  result.parser_sema_advanced_core_shard2 =
      handoff.parser_sema_advanced_core_shard2;
  result.deterministic_parser_sema_advanced_core_shard2 =
      handoff.parser_sema_advanced_core_shard2.deterministic;
  if (!result.deterministic_parser_sema_advanced_core_shard2) {
    return publication;
  }

  result.parser_sema_advanced_contract_rejection_shard2 =
      handoff.parser_sema_advanced_contract_rejection_shard2;
  result.deterministic_parser_sema_advanced_contract_rejection_shard2 =
      handoff.parser_sema_advanced_contract_rejection_shard2.deterministic;
  if (!result.deterministic_parser_sema_advanced_contract_rejection_shard2) {
    return publication;
  }

  result.parser_sema_advanced_diagnostics_shard2 =
      handoff.parser_sema_advanced_diagnostics_shard2;
  result.deterministic_parser_sema_advanced_diagnostics_shard2 =
      handoff.parser_sema_advanced_diagnostics_shard2.deterministic;
  if (!result.deterministic_parser_sema_advanced_diagnostics_shard2) {
    return publication;
  }

  result.parser_sema_integration_closeout_signoff =
      handoff.parser_sema_integration_closeout_signoff;
  result.deterministic_parser_sema_integration_closeout_signoff =
      handoff.parser_sema_integration_closeout_signoff.deterministic;
  if (!result.deterministic_parser_sema_integration_closeout_signoff) {
    return publication;
  }

  result.deterministic_parser_sema_handoff = handoff.deterministic;
  if (!handoff.deterministic) {
    return publication;
  }

  publication.evidence_record =
      BuildObjc3ParserSemaHandoffPublicationEvidenceRecord(handoff);
  publication.evidence_record_deterministic =
      IsReadyObjc3ParserSemaHandoffPublicationEvidenceRecord(
          publication.evidence_record);
  result.parser_sema_handoff_publication_evidence_record =
      publication.evidence_record;
  result.deterministic_parser_sema_handoff_publication_evidence_record =
      publication.evidence_record_deterministic;
  publication.ready = publication.evidence_record_deterministic;
  return publication;
}
