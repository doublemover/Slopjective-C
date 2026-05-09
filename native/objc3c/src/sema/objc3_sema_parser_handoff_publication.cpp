#include "sema/objc3_sema_parser_handoff_publication.h"

#include <sstream>

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

  publication.parser_recovery_replay_ready =
      result.parser_sema_conformance_matrix.parser_recovery_replay_ready;
  publication.parser_recovery_replay_case_present =
      result.parser_sema_conformance_corpus.has_recovery_replay_case;
  publication.parser_recovery_replay_case_passed =
      result.parser_sema_conformance_corpus.recovery_replay_case_passed;
  publication.parser_recovery_replay_contract_satisfied =
      publication.parser_recovery_replay_ready &&
      publication.parser_recovery_replay_case_present &&
      publication.parser_recovery_replay_case_passed &&
      result.parser_sema_conformance_corpus.required_case_count > 0u &&
      result.parser_sema_conformance_corpus.passed_case_count ==
          result.parser_sema_conformance_corpus.required_case_count &&
      result.parser_sema_conformance_corpus.failed_case_count == 0u;

  std::ostringstream recovery_replay_key_stream;
  recovery_replay_key_stream
      << "sema-pass-recovery:v1:"
      << "matrix_recovery_replay_ready="
      << (publication.parser_recovery_replay_ready ? "true" : "false")
      << ";corpus_recovery_replay_case_present="
      << (publication.parser_recovery_replay_case_present ? "true" : "false")
      << ";corpus_recovery_replay_case_passed="
      << (publication.parser_recovery_replay_case_passed ? "true" : "false")
      << ";corpus_required_case_count="
      << result.parser_sema_conformance_corpus.required_case_count
      << ";corpus_passed_case_count="
      << result.parser_sema_conformance_corpus.passed_case_count
      << ";corpus_failed_case_count="
      << result.parser_sema_conformance_corpus.failed_case_count;
  publication.recovery_replay_key = recovery_replay_key_stream.str();
  publication.recovery_replay_key_deterministic =
      result.deterministic_parser_sema_conformance_matrix &&
      result.deterministic_parser_sema_conformance_corpus &&
      !publication.recovery_replay_key.empty();
  publication.recovery_determinism_hardening_satisfied =
      publication.parser_recovery_replay_contract_satisfied &&
      publication.recovery_replay_key_deterministic;
  publication.ready = true;
  return publication;
}
