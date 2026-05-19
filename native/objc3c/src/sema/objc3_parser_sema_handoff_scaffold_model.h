#pragma once

#include <cstdint>

#include "parser/contracts/canonical_literal_handoff.h"
#include "sema/objc3_parser_sema_handoff_scaffold_sync.h"

struct Objc3ParserSemaHandoffScaffold {
  const Objc3ParsedProgram *program = nullptr;
  Objc3ParserSemaHandoffOwnerRecord owner_record;
  Objc3SemanticValidationOptions validation_options;
  Objc3SemaLanguageProfile language_profile = Objc3SemaLanguageProfile::Canonical;
  Objc3SemaCanonicalLiteralRejectionCounts
      canonical_literal_rejection_counts;
  Objc3ParserCanonicalLiteralRejectionHandoff
      canonical_literal_rejection_handoff;
  Objc3SemaDiagnosticsBus diagnostics_bus;
  Objc3ParserContractSnapshot parser_contract_snapshot;
  bool parser_contract_snapshot_normalization_candidate_detected = false;
  bool parser_contract_snapshot_normalization_applied = false;
  bool parser_contract_snapshot_normalization_rejected = false;
  std::uint64_t expected_ast_shape_fingerprint = 0;
  bool parser_contract_ast_shape_fingerprint_matches = false;
  std::uint64_t expected_ast_top_level_layout_fingerprint = 0;
  bool parser_contract_ast_top_level_layout_fingerprint_matches = false;
  std::uint64_t expected_parser_contract_snapshot_fingerprint = 0;
  std::uint64_t parser_contract_snapshot_fingerprint = 0;
  bool parser_contract_snapshot_fingerprint_matches = false;
  Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;
  Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;
  Objc3ParserSemaConformanceEvidenceRecord
      parser_sema_conformance_evidence_record;
  bool deterministic_parser_sema_conformance_evidence_record = false;
  Objc3ParserSemaPerformanceQualityGuardrails parser_sema_performance_quality_guardrails;
  Objc3ParserSemaCrossLaneIntegrationSync parser_sema_cross_lane_integration_sync;
  Objc3ParserSemaDocsRunbookSync parser_sema_docs_runbook_sync;
  Objc3ParserSemaReleaseCandidateReplayDryRun parser_sema_release_candidate_replay_dry_run;
  Objc3ParserSemaAdvancedCoreShard1 parser_sema_advanced_core_shard1;
  Objc3ParserSemaAdvancedContractRejectionShard1 parser_sema_advanced_contract_rejection_shard1;
  Objc3ParserSemaAdvancedDiagnosticsShard1 parser_sema_advanced_diagnostics_shard1;
  Objc3ParserSemaAdvancedConformanceShard1 parser_sema_advanced_conformance_shard1;
  Objc3ParserSemaAdvancedIntegrationShard1 parser_sema_advanced_integration_shard1;
  Objc3ParserSemaAdvancedPerformanceShard1 parser_sema_advanced_performance_shard1;
  Objc3ParserSemaAdvancedCoreShard2 parser_sema_advanced_core_shard2;
  Objc3ParserSemaAdvancedContractRejectionShard2 parser_sema_advanced_contract_rejection_shard2;
  Objc3ParserSemaAdvancedDiagnosticsShard2 parser_sema_advanced_diagnostics_shard2;
  Objc3ParserSemaIntegrationCloseoutSignoff parser_sema_integration_closeout_signoff;
  Objc3ParserSemaContractReadinessRecord parser_sema_contract_readiness_record;
  bool deterministic_parser_sema_contract_readiness_record = false;
  Objc3ParserSemaHandoffScaffoldReadinessRecord readiness_record;
  bool readiness_record_deterministic = false;
  bool parser_contract_snapshot_matches_program = false;
  bool deterministic = false;
};
