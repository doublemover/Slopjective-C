#pragma once

#include "parser/contracts/canonical_literal_handoff.h"
#include "sema/objc3_parser_sema_handoff_scaffold_surfaces.h"

inline Objc3ParserSemaHandoffScaffold BuildObjc3ParserSemaHandoffScaffold(const Objc3SemaPassManagerInput &input) {
  Objc3ParserSemaHandoffScaffold scaffold;
  scaffold.program = input.program;
  scaffold.owner_record = BuildObjc3ParserSemaHandoffOwnerRecord(input);
  scaffold.validation_options = input.validation_options;
  scaffold.language_profile = input.language_profile;
  scaffold.canonical_literal_rejection_counts =
      input.canonical_literal_rejection_counts;
  scaffold.diagnostics_bus = input.diagnostics_bus;
  if (input.program == nullptr) {
    return scaffold;
  }

  const Objc3ParserContractSnapshot resolved_snapshot = ResolveObjc3ParserContractSnapshotForSemaHandoff(input);
  scaffold.parser_contract_snapshot_normalization_candidate_detected =
      IsObjc3ParserContractSnapshotNormalizationCandidate(
          resolved_snapshot, *input.program);
  scaffold.parser_contract_snapshot_normalization_rejected =
      IsObjc3ParserContractSnapshotNormalizationRejectedForSemaHandoff(
          resolved_snapshot, *input.program, input.language_profile);
  scaffold.parser_contract_snapshot = NormalizeObjc3ParserContractSnapshotForSemaHandoff(
      resolved_snapshot,
      *input.program,
      input.language_profile,
      scaffold.parser_contract_snapshot_normalization_applied);
  scaffold.canonical_literal_rejection_handoff =
      BuildObjc3ParserCanonicalLiteralRejectionHandoff(
          scaffold.parser_contract_snapshot,
          scaffold.canonical_literal_rejection_counts);
  scaffold.expected_ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(*input.program);
  scaffold.parser_contract_ast_shape_fingerprint_matches =
      scaffold.parser_contract_snapshot.ast_shape_fingerprint == scaffold.expected_ast_shape_fingerprint;
  scaffold.expected_ast_top_level_layout_fingerprint = BuildObjc3ParsedProgramTopLevelLayoutFingerprint(*input.program);
  scaffold.parser_contract_ast_top_level_layout_fingerprint_matches =
      scaffold.parser_contract_snapshot.ast_top_level_layout_fingerprint ==
      scaffold.expected_ast_top_level_layout_fingerprint;
  const Objc3ParserContractSnapshot expected_snapshot = BuildObjc3ParserContractSnapshot(
      *input.program,
      scaffold.parser_contract_snapshot.parser_diagnostic_count,
      scaffold.parser_contract_snapshot.token_count);
  scaffold.parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(scaffold.parser_contract_snapshot);
  scaffold.expected_parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(expected_snapshot);
  scaffold.parser_contract_snapshot_fingerprint_matches =
      scaffold.parser_contract_snapshot_fingerprint == scaffold.expected_parser_contract_snapshot_fingerprint;
  scaffold.parser_sema_conformance_matrix = BuildObjc3ParserSemaConformanceMatrix(
      scaffold.parser_contract_snapshot, *input.program);
  scaffold.parser_sema_conformance_corpus =
      BuildObjc3ParserSemaConformanceCorpus(scaffold.parser_sema_conformance_matrix);
  scaffold.parser_sema_performance_quality_guardrails =
      BuildObjc3ParserSemaPerformanceQualityGuardrails(
          scaffold.parser_sema_conformance_matrix,
          scaffold.parser_sema_conformance_corpus);
  scaffold.parser_sema_cross_lane_integration_sync =
      BuildObjc3ParserSemaCrossLaneIntegrationSync(
          scaffold.parser_sema_conformance_matrix,
          scaffold.parser_sema_conformance_corpus,
          scaffold.parser_sema_performance_quality_guardrails);
  scaffold.parser_sema_docs_runbook_sync =
      BuildObjc3ParserSemaDocsRunbookSync(
          scaffold.parser_sema_cross_lane_integration_sync);
  scaffold.parser_sema_release_candidate_replay_dry_run =
      BuildObjc3ParserSemaReleaseCandidateReplayDryRun(
          scaffold.parser_sema_docs_runbook_sync);
  scaffold.parser_sema_advanced_core_shard1 =
      BuildObjc3ParserSemaAdvancedCoreShard1(
          scaffold.parser_sema_release_candidate_replay_dry_run);
  scaffold.parser_sema_advanced_contract_rejection_shard1 =
      BuildObjc3ParserSemaAdvancedContractRejectionShard1(
          scaffold.parser_sema_advanced_core_shard1);
  scaffold.parser_sema_advanced_diagnostics_shard1 =
      BuildObjc3ParserSemaAdvancedDiagnosticsShard1(
          scaffold.parser_sema_advanced_contract_rejection_shard1);
  scaffold.parser_sema_advanced_conformance_shard1 =
      BuildObjc3ParserSemaAdvancedConformanceShard1(
          scaffold.parser_sema_advanced_diagnostics_shard1);
  scaffold.parser_sema_advanced_integration_shard1 =
      BuildObjc3ParserSemaAdvancedIntegrationShard1(
          scaffold.parser_sema_advanced_conformance_shard1);
  scaffold.parser_sema_advanced_performance_shard1 =
      BuildObjc3ParserSemaAdvancedPerformanceShard1(
          scaffold.parser_sema_advanced_integration_shard1);
  scaffold.parser_sema_advanced_core_shard2 =
      BuildObjc3ParserSemaAdvancedCoreShard2(
          scaffold.parser_sema_advanced_performance_shard1);
  scaffold.parser_sema_advanced_contract_rejection_shard2 =
      BuildObjc3ParserSemaAdvancedContractRejectionShard2(
          scaffold.parser_sema_advanced_core_shard2);
  scaffold.parser_sema_advanced_diagnostics_shard2 =
      BuildObjc3ParserSemaAdvancedDiagnosticsShard2(
          scaffold.parser_sema_advanced_contract_rejection_shard2);
  scaffold.parser_sema_integration_closeout_signoff =
      BuildObjc3ParserSemaIntegrationCloseoutSignoff(
          scaffold.parser_sema_advanced_diagnostics_shard2);
  scaffold.parser_sema_conformance_evidence_record =
      BuildObjc3ParserSemaHandoffScaffoldConformanceEvidenceRecord(
          input, scaffold);
  scaffold.deterministic_parser_sema_conformance_evidence_record =
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          scaffold.parser_sema_conformance_evidence_record);
  scaffold.parser_sema_contract_readiness_record =
      BuildObjc3ParserSemaContractReadinessRecord(
          input,
          BuildObjc3ParserSemaHandoffScaffoldContractSurface(scaffold));
  scaffold.deterministic_parser_sema_contract_readiness_record =
      IsReadyObjc3ParserSemaContractReadinessRecord(
          scaffold.parser_sema_contract_readiness_record);
  scaffold.parser_contract_snapshot_matches_program =
      scaffold.deterministic_parser_sema_conformance_evidence_record &&
      scaffold.parser_sema_conformance_evidence_record
          .conformance_matrix_deterministic;
  scaffold.readiness_record =
      BuildObjc3ParserSemaHandoffScaffoldReadinessRecord(input, scaffold);
  scaffold.readiness_record_deterministic =
      IsReadyObjc3ParserSemaHandoffScaffoldReadinessRecord(
          scaffold.readiness_record);
  scaffold.deterministic = scaffold.readiness_record_deterministic;
  return scaffold;
}
