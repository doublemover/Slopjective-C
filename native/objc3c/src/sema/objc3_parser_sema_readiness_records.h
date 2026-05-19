#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

struct Objc3ParserSemaHandoffScaffoldReadinessRecord {
  std::string handoff_scaffold_readiness_owner =
      kObjc3ParserSemaHandoffScaffoldReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string parser_sema_conformance_evidence_owner =
      kObjc3ParserSemaConformanceEvidenceOwner;
  std::string parser_sema_contract_readiness_owner =
      kObjc3ParserSemaContractReadinessOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool owner_record_ready = false;
  bool snapshot_evidence_ready = false;
  bool snapshot_normalization_ready = false;
  bool canonical_rejection_ready = false;
  bool conformance_evidence_ready = false;
  bool parser_contract_readiness_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaHandoffScaffoldReadinessRecord(
    const Objc3ParserSemaHandoffScaffoldReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.handoff_scaffold_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_conformance_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_readiness_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.owner_record_ready && record.snapshot_evidence_ready &&
         record.snapshot_normalization_ready &&
         record.canonical_rejection_ready &&
         record.conformance_evidence_ready &&
         record.parser_contract_readiness_ready && record.deterministic;
}

struct Objc3ParserSemaPerformanceQualityGuardrails {
  std::size_t conformance_matrix_builder_max_lines = 0;
  std::size_t conformance_corpus_builder_max_lines = 0;
  std::size_t handoff_scaffold_builder_max_lines = 0;
  bool conformance_matrix_builder_budget_guarded = false;
  bool conformance_corpus_builder_budget_guarded = false;
  bool handoff_scaffold_builder_budget_guarded = false;
  bool matrix_diagnostic_budget_consistent = false;
  bool matrix_token_top_level_budget_consistent = false;
  bool matrix_subset_budget_consistent = false;
  bool corpus_case_budget_consistent = false;
  std::size_t required_guardrail_count = 0;
  std::size_t passed_guardrail_count = 0;
  std::size_t failed_guardrail_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaCrossLaneIntegrationSync {
  bool matrix_consistent = false;
  bool corpus_consistent = false;
  bool performance_quality_guardrails_consistent = false;
  bool pass_manager_contract_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaDocsRunbookSync {
  bool cross_lane_integration_sync_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool parity_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaReleaseCandidateReplayDryRun {
  bool docs_runbook_sync_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool replay_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedCoreShard1 {
  bool release_candidate_replay_dry_run_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedContractRejectionShard1 {
  bool advanced_core_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedDiagnosticsShard1 {
  bool advanced_contract_rejection_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedConformanceShard1 {
  bool advanced_diagnostics_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedIntegrationShard1 {
  bool advanced_conformance_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedPerformanceShard1 {
  bool advanced_integration_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedCoreShard2 {
  bool advanced_performance_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedContractRejectionShard2 {
  bool advanced_core_shard2_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedDiagnosticsShard2 {
  bool advanced_contract_rejection_shard2_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaIntegrationCloseoutSignoff {
  bool advanced_diagnostics_shard2_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool gate_signoff_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaContractReadinessRecord {
  std::string parser_sema_contract_readiness_owner =
      kObjc3ParserSemaContractReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string parity_validation_owner = kObjc3SemaParityValidationOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool conformance_evidence_ready = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_sync_ready = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_dry_run_ready = false;
  bool advanced_core_shard1_ready = false;
  bool advanced_contract_rejection_shard1_ready = false;
  bool advanced_diagnostics_shard1_ready = false;
  bool advanced_conformance_shard1_ready = false;
  bool advanced_integration_shard1_ready = false;
  bool advanced_performance_shard1_ready = false;
  bool advanced_core_shard2_ready = false;
  bool advanced_contract_rejection_shard2_ready = false;
  bool advanced_diagnostics_shard2_ready = false;
  bool integration_closeout_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaContractReadinessRecord(
    const Objc3ParserSemaContractReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.conformance_evidence_ready &&
         record.conformance_matrix_ready && record.conformance_corpus_ready &&
         record.performance_quality_guardrails_ready &&
         record.cross_lane_integration_sync_ready &&
         record.docs_runbook_sync_ready &&
         record.release_candidate_replay_dry_run_ready &&
         record.advanced_core_shard1_ready &&
         record.advanced_contract_rejection_shard1_ready &&
         record.advanced_diagnostics_shard1_ready &&
         record.advanced_conformance_shard1_ready &&
         record.advanced_integration_shard1_ready &&
         record.advanced_performance_shard1_ready &&
         record.advanced_core_shard2_ready &&
         record.advanced_contract_rejection_shard2_ready &&
         record.advanced_diagnostics_shard2_ready &&
         record.integration_closeout_ready && record.deterministic;
}
