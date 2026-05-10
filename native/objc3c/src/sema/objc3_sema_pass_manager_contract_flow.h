#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "sema/objc3_sema_canonical_literal_contract.h"
#include "sema/objc3_sema_closeout_readiness_contract.h"
#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"
#include "sema/objc3_sema_pass_flow_diagnostics_contract.h"
#include "sema/objc3_sema_pass_manager_publication_contract.h"

#include "sema/objc3_sema_pass_manager_type_metadata_records.h"

#include "sema/objc3_sema_pass_manager_closeout_surface_records.h"

#include "sema/objc3_sema_pass_manager_semantic_summary_records.h"

#include "sema/objc3_parser_sema_conformance_records.h"

struct Objc3ParserSemaHandoffPublicationEvidenceRecord {
  std::string handoff_publication_evidence_owner =
      kObjc3ParserSemaHandoffPublicationEvidenceOwner;
  std::string parser_sema_conformance_evidence_owner =
      kObjc3ParserSemaConformanceEvidenceOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool parser_recovery_replay_ready = false;
  bool parser_recovery_replay_case_present = false;
  bool parser_recovery_replay_case_passed = false;
  std::size_t corpus_required_case_count = 0;
  std::size_t corpus_passed_case_count = 0;
  std::size_t corpus_failed_case_count = 0;
  bool corpus_case_counts_ready = false;
  bool parser_recovery_replay_contract_satisfied = false;
  std::string recovery_replay_key;
  bool recovery_replay_key_deterministic = false;
  bool recovery_determinism_hardening_satisfied = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaHandoffPublicationEvidenceRecord(
    const Objc3ParserSemaHandoffPublicationEvidenceRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.handoff_publication_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_conformance_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
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
         record.recovery_determinism_hardening_satisfied &&
         record.deterministic;
}

struct Objc3ParserSemaHandoffPublicationTransferRecord {
  std::string handoff_publication_transfer_owner =
      kObjc3ParserSemaHandoffPublicationTransferOwner;
  std::string handoff_publication_evidence_owner =
      kObjc3ParserSemaHandoffPublicationEvidenceOwner;
  std::string handoff_scaffold_readiness_owner =
      kObjc3ParserSemaHandoffScaffoldReadinessOwner;
  std::string parser_sema_contract_readiness_owner =
      kObjc3ParserSemaContractReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_transfer_count = 19u;
  std::size_t passed_transfer_count = 0;
  std::size_t failed_transfer_count = 0;
  bool owner_record_ready = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_sync_ready = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_ready = false;
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
  bool scaffold_readiness_ready = false;
  bool evidence_record_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaHandoffPublicationTransferRecord(
    const Objc3ParserSemaHandoffPublicationTransferRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.handoff_publication_transfer_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.handoff_publication_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.handoff_scaffold_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_transfer_count == 19u &&
         record.passed_transfer_count == record.required_transfer_count &&
         record.failed_transfer_count == 0u && record.owner_record_ready &&
         record.conformance_matrix_ready && record.conformance_corpus_ready &&
         record.performance_quality_guardrails_ready &&
         record.cross_lane_integration_sync_ready &&
         record.docs_runbook_sync_ready &&
         record.release_candidate_replay_ready &&
         record.advanced_core_shard1_ready &&
         record.advanced_contract_rejection_shard1_ready &&
         record.advanced_diagnostics_shard1_ready &&
         record.advanced_conformance_shard1_ready &&
         record.advanced_integration_shard1_ready &&
         record.advanced_performance_shard1_ready &&
         record.advanced_core_shard2_ready &&
         record.advanced_contract_rejection_shard2_ready &&
         record.advanced_diagnostics_shard2_ready &&
         record.integration_closeout_ready &&
         record.scaffold_readiness_ready && record.evidence_record_ready &&
         record.deterministic;
}

struct Objc3ParserSemaParityPublicationReadinessRecord {
  std::string parser_sema_parity_publication_readiness_owner =
      kObjc3ParserSemaParityPublicationReadinessOwner;
  std::string handoff_publication_transfer_owner =
      kObjc3ParserSemaHandoffPublicationTransferOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 17u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool handoff_publication_transfer_ready = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_sync_ready = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_ready = false;
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

inline bool IsReadyObjc3ParserSemaParityPublicationReadinessRecord(
    const Objc3ParserSemaParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.parser_sema_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.handoff_publication_transfer_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 17u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.handoff_publication_transfer_ready &&
         record.conformance_matrix_ready && record.conformance_corpus_ready &&
         record.performance_quality_guardrails_ready &&
         record.cross_lane_integration_sync_ready &&
         record.docs_runbook_sync_ready &&
         record.release_candidate_replay_ready &&
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

struct Objc3SemaCoreSemanticParityPublicationReadinessRecord {
  std::string core_semantic_parity_publication_readiness_owner =
      kObjc3SemaCoreSemanticParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 12u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool semantic_diagnostics_ready = false;
  bool type_metadata_handoff_ready = false;
  bool interface_implementation_ready = false;
  bool protocol_category_composition_ready = false;
  bool class_protocol_category_linking_ready = false;
  bool selector_normalization_ready = false;
  bool property_attribute_ready = false;
  bool type_annotation_surface_ready = false;
  bool lightweight_generic_constraint_ready = false;
  bool nullability_flow_warning_precision_ready = false;
  bool protocol_qualified_object_type_ready = false;
  bool variance_bridge_cast_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaCoreSemanticParityPublicationReadinessRecord(
    const Objc3SemaCoreSemanticParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.core_semantic_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 12u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.semantic_diagnostics_ready &&
         record.type_metadata_handoff_ready &&
         record.interface_implementation_ready &&
         record.protocol_category_composition_ready &&
         record.class_protocol_category_linking_ready &&
         record.selector_normalization_ready &&
         record.property_attribute_ready &&
         record.type_annotation_surface_ready &&
         record.lightweight_generic_constraint_ready &&
         record.nullability_flow_warning_precision_ready &&
         record.protocol_qualified_object_type_ready &&
         record.variance_bridge_cast_ready && record.deterministic;
}

struct Objc3SemaModuleSemanticParityPublicationReadinessRecord {
  std::string module_semantic_parity_publication_readiness_owner =
      kObjc3SemaModuleSemanticParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 5u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool generic_metadata_abi_ready = false;
  bool module_import_graph_ready = false;
  bool namespace_collision_shadowing_ready = false;
  bool public_private_api_partition_ready = false;
  bool incremental_module_cache_invalidation_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaModuleSemanticParityPublicationReadinessRecord(
    const Objc3SemaModuleSemanticParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.module_semantic_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 5u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.generic_metadata_abi_ready &&
         record.module_import_graph_ready &&
         record.namespace_collision_shadowing_ready &&
         record.public_private_api_partition_ready &&
         record.incremental_module_cache_invalidation_ready &&
         record.deterministic;
}

struct Objc3SemaIntermoduleFlowParityPublicationReadinessRecord {
  std::string intermodule_flow_parity_publication_readiness_owner =
      kObjc3SemaIntermoduleFlowParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 2u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool cross_module_conformance_ready = false;
  bool throws_propagation_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
    const Objc3SemaIntermoduleFlowParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.intermodule_flow_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 2u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.cross_module_conformance_ready &&
         record.throws_propagation_ready && record.deterministic;
}

struct Objc3SemaConcurrencyParityPublicationReadinessRecord {
  std::string concurrency_parity_publication_readiness_owner =
      kObjc3SemaConcurrencyParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 3u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool actor_isolation_sendability_ready = false;
  bool task_runtime_cancellation_ready = false;
  bool concurrency_replay_race_guard_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaConcurrencyParityPublicationReadinessRecord(
    const Objc3SemaConcurrencyParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.concurrency_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 3u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.actor_isolation_sendability_ready &&
         record.task_runtime_cancellation_ready &&
         record.concurrency_replay_race_guard_ready && record.deterministic;
}

#include "sema/objc3_sema_pass_manager_parity_validation_records.h"

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
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
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
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
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
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
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

struct Objc3SemaParityContractSurface {
  Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;
  Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;
  Objc3ParserSemaConformanceEvidenceRecord
      parser_sema_conformance_evidence_record;
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
  Objc3ParserSemaHandoffPublicationTransferRecord
      parser_sema_handoff_publication_transfer_record;
  Objc3ParserSemaParityPublicationReadinessRecord
      parser_sema_parity_publication_readiness_record;
  Objc3SemaCoreSemanticParityPublicationReadinessRecord
      core_semantic_parity_publication_readiness_record;
  Objc3SemaCoreSemanticSummaryReadinessRecord
      core_semantic_summary_readiness_record;
  Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord
      selector_property_type_annotation_readiness_record;
  Objc3SemaTypeBoundarySummaryReadinessRecord
      type_boundary_summary_readiness_record;
  Objc3SemaModuleTypeAbiSummaryReadinessRecord
      module_type_abi_summary_readiness_record;
  Objc3SemaModuleBoundarySummaryReadinessRecord
      module_boundary_summary_readiness_record;
  Objc3SemaIntermoduleFlowSummaryReadinessRecord
      intermodule_flow_summary_readiness_record;
  Objc3SemaModuleSemanticParityPublicationReadinessRecord
      module_semantic_parity_publication_readiness_record;
  Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
      intermodule_flow_parity_publication_readiness_record;
  Objc3SemaConcurrencyParityPublicationReadinessRecord
      concurrency_parity_publication_readiness_record;
  Objc3SemaUnsafeErrorParityValidationReadinessRecord
      unsafe_error_parity_validation_readiness_record;
  Objc3SemaControlBindingParityValidationReadinessRecord
      control_binding_parity_validation_readiness_record;
  Objc3SemaAsyncBlockMessageParityValidationReadinessRecord
      async_block_message_parity_validation_readiness_record;
  Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
      dispatch_runtime_arc_parity_validation_readiness_record;
  Objc3ParserSemaContractReadinessRecord
      parser_sema_contract_readiness_record;
  Objc3SemaPassFlowSummary sema_pass_flow_summary;
  Objc3SemaDiagnosticsPublicationRecord diagnostics_publication_record;
  Objc3SemaPassFlowRecoveryRecord pass_flow_recovery_record;
  Objc3SemaPassManagerPublicationRecord pass_manager_publication_record;
  Objc3SemaTypeMetadataPublicationRecord type_metadata_publication_record;
  Objc3SemaTypeMetadataMappingReadinessRecord
      type_metadata_mapping_readiness_record;
  Objc3SemaAtomicVectorMappingPublicationRecord
      atomic_vector_mapping_publication_record;
  Objc3SemaTypedSemanticHandoffRecord typed_semantic_handoff_record;
  Objc3SemaParityCloseoutPublicationReadinessRecord
      parity_closeout_publication_readiness_record;
  Objc3SemaParityValidationRecord parity_validation_record;
  Objc3SemaCloseoutSurfaceReadinessRecord closeout_surface_readiness_record;
  Objc3SemaCloseoutSignoffRecord closeout_signoff_record;
  std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};
  std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};
  std::size_t diagnostics_total = 0;
  std::size_t globals_total = 0;
  std::size_t functions_total = 0;
  std::size_t interfaces_total = 0;
  std::size_t implementations_total = 0;
  std::size_t type_metadata_global_entries = 0;
  std::size_t type_metadata_function_entries = 0;
  std::size_t type_metadata_interface_entries = 0;
  std::size_t type_metadata_implementation_entries = 0;
  std::size_t interface_method_symbols_total = 0;
  std::size_t implementation_method_symbols_total = 0;
  std::size_t linked_implementation_symbols_total = 0;
  std::size_t protocol_composition_sites_total = 0;
  std::size_t protocol_composition_symbols_total = 0;
  std::size_t category_composition_sites_total = 0;
  std::size_t category_composition_symbols_total = 0;
  std::size_t invalid_protocol_composition_sites_total = 0;
  std::size_t selector_normalization_methods_total = 0;
  std::size_t selector_normalization_normalized_methods_total = 0;
  std::size_t selector_normalization_piece_entries_total = 0;
  std::size_t selector_normalization_parameter_piece_entries_total = 0;
  std::size_t selector_normalization_pieceless_methods_total = 0;
  std::size_t selector_normalization_spelling_mismatches_total = 0;
  std::size_t selector_normalization_arity_mismatches_total = 0;
  std::size_t selector_normalization_parameter_linkage_mismatches_total = 0;
  std::size_t selector_normalization_flag_mismatches_total = 0;
  std::size_t selector_normalization_missing_keyword_pieces_total = 0;
  std::size_t property_attribute_properties_total = 0;
  std::size_t property_attribute_entries_total = 0;
  std::size_t property_attribute_readonly_modifiers_total = 0;
  std::size_t property_attribute_readwrite_modifiers_total = 0;
  std::size_t property_attribute_atomic_modifiers_total = 0;
  std::size_t property_attribute_nonatomic_modifiers_total = 0;
  std::size_t property_attribute_copy_modifiers_total = 0;
  std::size_t property_attribute_strong_modifiers_total = 0;
  std::size_t property_attribute_weak_modifiers_total = 0;
  std::size_t property_attribute_assign_modifiers_total = 0;
  std::size_t property_attribute_getter_modifiers_total = 0;
  std::size_t property_attribute_setter_modifiers_total = 0;
  std::size_t property_attribute_invalid_attribute_entries_total = 0;
  std::size_t property_attribute_contract_violations_total = 0;
  std::size_t type_annotation_generic_suffix_sites_total = 0;
  std::size_t type_annotation_pointer_declarator_sites_total = 0;
  std::size_t type_annotation_nullability_suffix_sites_total = 0;
  std::size_t type_annotation_ownership_qualifier_sites_total = 0;
  std::size_t type_annotation_object_pointer_type_sites_total = 0;
  std::size_t type_annotation_invalid_generic_suffix_sites_total = 0;
  std::size_t type_annotation_invalid_pointer_declarator_sites_total = 0;
  std::size_t type_annotation_invalid_nullability_suffix_sites_total = 0;
  std::size_t type_annotation_invalid_ownership_qualifier_sites_total = 0;
  std::size_t lightweight_generic_constraint_sites_total = 0;
  std::size_t lightweight_generic_constraint_generic_suffix_sites_total = 0;
  std::size_t lightweight_generic_constraint_object_pointer_type_sites_total = 0;
  std::size_t lightweight_generic_constraint_terminated_generic_suffix_sites_total = 0;
  std::size_t lightweight_generic_constraint_pointer_declarator_sites_total = 0;
  std::size_t lightweight_generic_constraint_normalized_sites_total = 0;
  std::size_t lightweight_generic_constraint_contract_violation_sites_total = 0;
  std::size_t nullability_flow_sites_total = 0;
  std::size_t nullability_flow_object_pointer_type_sites_total = 0;
  std::size_t nullability_flow_nullability_suffix_sites_total = 0;
  std::size_t nullability_flow_nullable_suffix_sites_total = 0;
  std::size_t nullability_flow_nonnull_suffix_sites_total = 0;
  std::size_t nullability_flow_normalized_sites_total = 0;
  std::size_t nullability_flow_contract_violation_sites_total = 0;
  std::size_t protocol_qualified_object_type_sites_total = 0;
  std::size_t protocol_qualified_object_type_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_object_pointer_type_sites_total = 0;
  std::size_t protocol_qualified_object_type_terminated_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_pointer_declarator_sites_total = 0;
  std::size_t protocol_qualified_object_type_normalized_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_contract_violation_sites_total = 0;
  std::size_t variance_bridge_cast_sites_total = 0;
  std::size_t variance_bridge_cast_protocol_composition_sites_total = 0;
  std::size_t variance_bridge_cast_ownership_qualifier_sites_total = 0;
  std::size_t variance_bridge_cast_object_pointer_type_sites_total = 0;
  std::size_t variance_bridge_cast_pointer_declarator_sites_total = 0;
  std::size_t variance_bridge_cast_normalized_sites_total = 0;
  std::size_t variance_bridge_cast_contract_violation_sites_total = 0;
  std::size_t generic_metadata_abi_sites_total = 0;
  std::size_t generic_metadata_abi_generic_suffix_sites_total = 0;
  std::size_t generic_metadata_abi_protocol_composition_sites_total = 0;
  std::size_t generic_metadata_abi_ownership_qualifier_sites_total = 0;
  std::size_t generic_metadata_abi_object_pointer_type_sites_total = 0;
  std::size_t generic_metadata_abi_pointer_declarator_sites_total = 0;
  std::size_t generic_metadata_abi_normalized_sites_total = 0;
  std::size_t generic_metadata_abi_contract_violation_sites_total = 0;
  std::size_t module_import_graph_sites_total = 0;
  std::size_t module_import_graph_import_edge_candidate_sites_total = 0;
  std::size_t module_import_graph_namespace_segment_sites_total = 0;
  std::size_t module_import_graph_object_pointer_type_sites_total = 0;
  std::size_t module_import_graph_pointer_declarator_sites_total = 0;
  std::size_t module_import_graph_normalized_sites_total = 0;
  std::size_t module_import_graph_contract_violation_sites_total = 0;
  std::size_t namespace_collision_shadowing_sites_total = 0;
  std::size_t namespace_collision_shadowing_namespace_segment_sites_total = 0;
  std::size_t namespace_collision_shadowing_import_edge_candidate_sites_total = 0;
  std::size_t namespace_collision_shadowing_object_pointer_type_sites_total = 0;
  std::size_t namespace_collision_shadowing_pointer_declarator_sites_total = 0;
  std::size_t namespace_collision_shadowing_normalized_sites_total = 0;
  std::size_t namespace_collision_shadowing_contract_violation_sites_total = 0;
  std::size_t public_private_api_partition_sites_total = 0;
  std::size_t public_private_api_partition_namespace_segment_sites_total = 0;
  std::size_t public_private_api_partition_import_edge_candidate_sites_total = 0;
  std::size_t public_private_api_partition_object_pointer_type_sites_total = 0;
  std::size_t public_private_api_partition_pointer_declarator_sites_total = 0;
  std::size_t public_private_api_partition_normalized_sites_total = 0;
  std::size_t public_private_api_partition_contract_violation_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_namespace_segment_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_import_edge_candidate_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_object_pointer_type_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_pointer_declarator_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_normalized_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_contract_violation_sites_total = 0;
  std::size_t cross_module_conformance_sites_total = 0;
  std::size_t cross_module_conformance_namespace_segment_sites_total = 0;
  std::size_t cross_module_conformance_import_edge_candidate_sites_total = 0;
  std::size_t cross_module_conformance_object_pointer_type_sites_total = 0;
  std::size_t cross_module_conformance_pointer_declarator_sites_total = 0;
  std::size_t cross_module_conformance_normalized_sites_total = 0;
  std::size_t cross_module_conformance_cache_invalidation_candidate_sites_total = 0;
  std::size_t cross_module_conformance_contract_violation_sites_total = 0;
  std::size_t throws_propagation_sites_total = 0;
  std::size_t throws_propagation_namespace_segment_sites_total = 0;
  std::size_t throws_propagation_import_edge_candidate_sites_total = 0;
  std::size_t throws_propagation_object_pointer_type_sites_total = 0;
  std::size_t throws_propagation_pointer_declarator_sites_total = 0;
  std::size_t throws_propagation_normalized_sites_total = 0;
  std::size_t throws_propagation_cache_invalidation_candidate_sites_total = 0;
  std::size_t throws_propagation_contract_violation_sites_total = 0;
  std::size_t async_continuation_sites_total = 0;
  std::size_t async_continuation_async_keyword_sites_total = 0;
  std::size_t async_continuation_async_function_sites_total = 0;
  std::size_t async_continuation_allocation_sites_total = 0;
  std::size_t async_continuation_resume_sites_total = 0;
  std::size_t async_continuation_suspend_sites_total = 0;
  std::size_t async_continuation_state_machine_sites_total = 0;
  std::size_t async_continuation_normalized_sites_total = 0;
  std::size_t async_continuation_gate_blocked_sites_total = 0;
  std::size_t async_continuation_contract_violation_sites_total = 0;
  std::size_t actor_isolation_sendability_sites_total = 0;
  std::size_t actor_isolation_decl_sites_total = 0;
  std::size_t actor_hop_sites_total = 0;
  std::size_t sendable_annotation_sites_total = 0;
  std::size_t non_sendable_crossing_sites_total = 0;
  std::size_t actor_isolation_sendability_isolation_boundary_sites_total = 0;
  std::size_t actor_isolation_sendability_normalized_sites_total = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites_total = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites_total = 0;
  std::size_t task_runtime_cancellation_sites_total = 0;
  std::size_t task_runtime_cancellation_runtime_hook_sites_total = 0;
  std::size_t task_runtime_cancellation_cancellation_check_sites_total = 0;
  std::size_t task_runtime_cancellation_cancellation_handler_sites_total = 0;
  std::size_t task_runtime_cancellation_suspension_point_sites_total = 0;
  std::size_t task_runtime_cancellation_cancellation_propagation_sites_total =
      0;
  std::size_t task_runtime_cancellation_normalized_sites_total = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites_total = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites_total = 0;
  std::size_t concurrency_replay_race_guard_sites_total = 0;
  std::size_t concurrency_replay_race_guard_concurrency_replay_sites_total = 0;
  std::size_t concurrency_replay_race_guard_replay_proof_sites_total = 0;
  std::size_t concurrency_replay_race_guard_race_guard_sites_total = 0;
  std::size_t concurrency_replay_race_guard_task_handoff_sites_total = 0;
  std::size_t concurrency_replay_race_guard_actor_isolation_sites_total = 0;
  std::size_t concurrency_replay_race_guard_deterministic_schedule_sites_total = 0;
  std::size_t concurrency_replay_race_guard_guard_blocked_sites_total = 0;
  std::size_t concurrency_replay_race_guard_contract_violation_sites_total = 0;
  std::size_t unsafe_pointer_extension_sites_total = 0;
  std::size_t unsafe_pointer_extension_unsafe_keyword_sites_total = 0;
  std::size_t unsafe_pointer_extension_pointer_arithmetic_sites_total = 0;
  std::size_t unsafe_pointer_extension_raw_pointer_type_sites_total = 0;
  std::size_t unsafe_pointer_extension_unsafe_operation_sites_total = 0;
  std::size_t unsafe_pointer_extension_normalized_sites_total = 0;
  std::size_t unsafe_pointer_extension_gate_blocked_sites_total = 0;
  std::size_t unsafe_pointer_extension_contract_violation_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_inline_asm_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_intrinsic_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_governed_intrinsic_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_privileged_intrinsic_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_normalized_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_gate_blocked_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_contract_violation_sites_total = 0;
  std::size_t ns_error_bridging_sites_total = 0;
  std::size_t ns_error_bridging_ns_error_parameter_sites_total = 0;
  std::size_t ns_error_bridging_ns_error_out_parameter_sites_total = 0;
  std::size_t ns_error_bridging_ns_error_bridge_path_sites_total = 0;
  std::size_t ns_error_bridging_failable_call_sites_total = 0;
  std::size_t ns_error_bridging_normalized_sites_total = 0;
  std::size_t ns_error_bridging_bridge_boundary_sites_total = 0;
  std::size_t ns_error_bridging_contract_violation_sites_total = 0;
  std::size_t error_diagnostics_recovery_sites_total = 0;
  std::size_t error_diagnostics_recovery_diagnostic_emit_sites_total = 0;
  std::size_t error_diagnostics_recovery_recovery_anchor_sites_total = 0;
  std::size_t error_diagnostics_recovery_recovery_boundary_sites_total = 0;
  std::size_t error_diagnostics_recovery_fail_closed_diagnostic_sites_total = 0;
  std::size_t error_diagnostics_recovery_normalized_sites_total = 0;
  std::size_t error_diagnostics_recovery_gate_blocked_sites_total = 0;
  std::size_t error_diagnostics_recovery_contract_violation_sites_total = 0;
  std::size_t result_like_lowering_sites_total = 0;
  std::size_t result_like_lowering_result_success_sites_total = 0;
  std::size_t result_like_lowering_result_failure_sites_total = 0;
  std::size_t result_like_lowering_result_branch_sites_total = 0;
  std::size_t result_like_lowering_result_payload_sites_total = 0;
  std::size_t result_like_lowering_normalized_sites_total = 0;
  std::size_t result_like_lowering_branch_merge_sites_total = 0;
  std::size_t result_like_lowering_contract_violation_sites_total = 0;
  std::size_t unwind_cleanup_sites_total = 0;
  std::size_t unwind_cleanup_exceptional_exit_sites_total = 0;
  std::size_t unwind_cleanup_action_sites_total = 0;
  std::size_t unwind_cleanup_scope_sites_total = 0;
  std::size_t unwind_cleanup_resume_sites_total = 0;
  std::size_t unwind_cleanup_normalized_sites_total = 0;
  std::size_t unwind_cleanup_fail_closed_sites_total = 0;
  std::size_t unwind_cleanup_contract_violation_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_keyword_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_suspension_point_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_resume_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_state_machine_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_continuation_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_normalized_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_gate_blocked_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_contract_violation_sites_total = 0;
  std::size_t symbol_graph_global_symbol_nodes_total = 0;
  std::size_t symbol_graph_function_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_property_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_property_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_method_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_method_symbol_nodes_total = 0;
  std::size_t symbol_graph_top_level_scope_symbols_total = 0;
  std::size_t symbol_graph_nested_scope_symbols_total = 0;
  std::size_t symbol_graph_scope_frames_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_sites_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_hits_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_misses_total = 0;
  std::size_t symbol_graph_method_resolution_sites_total = 0;
  std::size_t symbol_graph_method_resolution_hits_total = 0;
  std::size_t symbol_graph_method_resolution_misses_total = 0;
  std::size_t method_lookup_override_conflict_lookup_sites_total = 0;
  std::size_t method_lookup_override_conflict_lookup_hits_total = 0;
  std::size_t method_lookup_override_conflict_lookup_misses_total = 0;
  std::size_t method_lookup_override_conflict_override_sites_total = 0;
  std::size_t method_lookup_override_conflict_override_hits_total = 0;
  std::size_t method_lookup_override_conflict_override_misses_total = 0;
  std::size_t method_lookup_override_conflict_override_conflicts_total = 0;
  std::size_t method_lookup_override_conflict_unresolved_base_interfaces_total = 0;
  std::size_t property_synthesis_ivar_binding_property_synthesis_sites_total = 0;
  std::size_t property_synthesis_ivar_binding_explicit_ivar_bindings_total = 0;
  std::size_t property_synthesis_ivar_binding_default_ivar_bindings_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_sites_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_resolved_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_missing_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_conflicts_total = 0;
  std::size_t id_class_sel_object_pointer_param_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_object_pointer_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_object_pointer_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_object_pointer_type_sites_total = 0;
  std::size_t block_literal_capture_semantics_sites_total = 0;
  std::size_t block_literal_capture_semantics_parameter_entries_total = 0;
  std::size_t block_literal_capture_semantics_capture_entries_total = 0;
  std::size_t block_literal_capture_semantics_body_statement_entries_total = 0;
  std::size_t block_literal_capture_semantics_empty_capture_sites_total = 0;
  std::size_t block_literal_capture_semantics_nondeterministic_capture_sites_total = 0;
  std::size_t block_literal_capture_semantics_non_normalized_sites_total = 0;
  std::size_t block_literal_capture_semantics_contract_violation_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_invoke_argument_slots_total = 0;
  std::size_t block_abi_invoke_trampoline_capture_word_count_total = 0;
  std::size_t block_abi_invoke_trampoline_parameter_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_capture_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_body_statement_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_descriptor_symbolized_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_invoke_symbolized_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_missing_invoke_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_non_normalized_layout_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_contract_violation_sites_total = 0;
  std::size_t block_storage_escape_sites_total = 0;
  std::size_t block_storage_escape_mutable_capture_count_total = 0;
  std::size_t block_storage_escape_byref_slot_count_total = 0;
  std::size_t block_storage_escape_parameter_entries_total = 0;
  std::size_t block_storage_escape_capture_entries_total = 0;
  std::size_t block_storage_escape_body_statement_entries_total = 0;
  std::size_t block_storage_escape_requires_byref_cells_sites_total = 0;
  std::size_t block_storage_escape_escape_analysis_enabled_sites_total = 0;
  std::size_t block_storage_escape_escape_to_heap_sites_total = 0;
  std::size_t block_storage_escape_escape_profile_normalized_sites_total = 0;
  std::size_t block_storage_escape_byref_layout_symbolized_sites_total = 0;
  std::size_t block_storage_escape_contract_violation_sites_total = 0;
  std::size_t block_copy_dispose_sites_total = 0;
  std::size_t block_copy_dispose_mutable_capture_count_total = 0;
  std::size_t block_copy_dispose_byref_slot_count_total = 0;
  std::size_t block_copy_dispose_parameter_entries_total = 0;
  std::size_t block_copy_dispose_capture_entries_total = 0;
  std::size_t block_copy_dispose_body_statement_entries_total = 0;
  std::size_t block_copy_dispose_copy_helper_required_sites_total = 0;
  std::size_t block_copy_dispose_dispose_helper_required_sites_total = 0;
  std::size_t block_copy_dispose_profile_normalized_sites_total = 0;
  std::size_t block_copy_dispose_copy_helper_symbolized_sites_total = 0;
  std::size_t block_copy_dispose_dispose_helper_symbolized_sites_total = 0;
  std::size_t block_copy_dispose_contract_violation_sites_total = 0;
  std::size_t block_determinism_perf_baseline_sites_total = 0;
  std::size_t block_determinism_perf_baseline_weight_total = 0;
  std::size_t block_determinism_perf_baseline_parameter_entries_total = 0;
  std::size_t block_determinism_perf_baseline_capture_entries_total = 0;
  std::size_t block_determinism_perf_baseline_body_statement_entries_total = 0;
  std::size_t block_determinism_perf_baseline_deterministic_capture_sites_total = 0;
  std::size_t block_determinism_perf_baseline_heavy_tier_sites_total = 0;
  std::size_t block_determinism_perf_baseline_normalized_profile_sites_total = 0;
  std::size_t block_determinism_perf_baseline_contract_violation_sites_total = 0;
  std::size_t message_send_selector_lowering_sites_total = 0;
  std::size_t message_send_selector_lowering_unary_form_sites_total = 0;
  std::size_t message_send_selector_lowering_keyword_form_sites_total = 0;
  std::size_t message_send_selector_lowering_symbol_sites_total = 0;
  std::size_t message_send_selector_lowering_piece_entries_total = 0;
  std::size_t message_send_selector_lowering_argument_piece_entries_total = 0;
  std::size_t message_send_selector_lowering_normalized_sites_total = 0;
  std::size_t message_send_selector_lowering_form_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_arity_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_symbol_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_missing_symbol_sites_total = 0;
  std::size_t message_send_selector_lowering_contract_violation_sites_total = 0;
  std::size_t dispatch_abi_marshalling_sites_total = 0;
  std::size_t dispatch_abi_marshalling_receiver_slots_total = 0;
  std::size_t dispatch_abi_marshalling_selector_symbol_slots_total = 0;
  std::size_t dispatch_abi_marshalling_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_keyword_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_unary_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_arity_mismatch_sites_total = 0;
  std::size_t dispatch_abi_marshalling_missing_selector_symbol_sites_total = 0;
  std::size_t dispatch_abi_marshalling_contract_violation_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_receiver_nil_literal_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_enabled_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_foldable_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_non_nil_receiver_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_contract_violation_sites_total = 0;
  std::size_t super_dispatch_method_family_sites_total = 0;
  std::size_t super_dispatch_method_family_receiver_super_identifier_sites_total = 0;
  std::size_t super_dispatch_method_family_enabled_sites_total = 0;
  std::size_t super_dispatch_method_family_requires_class_context_sites_total = 0;
  std::size_t super_dispatch_method_family_init_sites_total = 0;
  std::size_t super_dispatch_method_family_copy_sites_total = 0;
  std::size_t super_dispatch_method_family_mutable_copy_sites_total = 0;
  std::size_t super_dispatch_method_family_new_sites_total = 0;
  std::size_t super_dispatch_method_family_none_sites_total = 0;
  std::size_t super_dispatch_method_family_returns_retained_result_sites_total = 0;
  std::size_t super_dispatch_method_family_returns_related_result_sites_total = 0;
  std::size_t super_dispatch_method_family_contract_violation_sites_total = 0;
  std::size_t runtime_link_host_link_message_send_sites_total = 0;
  std::size_t runtime_link_host_link_required_sites_total = 0;
  std::size_t runtime_link_host_link_elided_sites_total = 0;
  std::size_t runtime_link_host_link_runtime_dispatch_arg_slots_total = 0;
  std::size_t runtime_link_host_link_runtime_dispatch_declaration_parameter_count_total = 0;
  std::size_t runtime_link_host_link_contract_violation_sites_total = 0;
  std::string runtime_link_host_link_runtime_dispatch_symbol;
  bool runtime_link_host_link_default_runtime_dispatch_symbol_binding = true;
  std::size_t retain_release_operation_ownership_qualified_sites_total = 0;
  std::size_t retain_release_operation_retain_insertion_sites_total = 0;
  std::size_t retain_release_operation_release_insertion_sites_total = 0;
  std::size_t retain_release_operation_autorelease_insertion_sites_total = 0;
  std::size_t retain_release_operation_contract_violation_sites_total = 0;
  std::size_t weak_unowned_semantics_ownership_candidate_sites_total = 0;
  std::size_t weak_unowned_semantics_weak_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_unowned_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_unowned_safe_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_conflict_sites_total = 0;
  std::size_t weak_unowned_semantics_contract_violation_sites_total = 0;
  std::size_t ownership_arc_diagnostic_candidate_sites_total = 0;
  std::size_t ownership_arc_fixit_available_sites_total = 0;
  std::size_t ownership_arc_profiled_sites_total = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites_total = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites_total = 0;
  std::size_t ownership_arc_contract_violation_sites_total = 0;
  std::size_t autoreleasepool_scope_sites_total = 0;
  std::size_t autoreleasepool_scope_symbolized_sites_total = 0;
  std::size_t autoreleasepool_scope_contract_violation_sites_total = 0;
  unsigned autoreleasepool_scope_max_depth_total = 0;
  bool diagnostics_accounting_consistent = false;
  bool diagnostics_bus_publish_consistent = false;
  bool diagnostics_canonicalized = false;
  bool diagnostics_hardening_satisfied = false;
  bool pass_flow_recovery_replay_contract_satisfied = false;
  std::string pass_flow_recovery_replay_key;
  bool pass_flow_recovery_replay_key_deterministic = false;
  bool pass_flow_recovery_determinism_hardening_satisfied = false;
  bool diagnostics_after_pass_monotonic = false;
  bool deterministic_parser_sema_conformance_matrix = false;
  bool deterministic_parser_sema_conformance_corpus = false;
  bool deterministic_parser_sema_conformance_evidence_record = false;
  bool deterministic_parser_sema_performance_quality_guardrails = false;
  bool deterministic_parser_sema_cross_lane_integration_sync = false;
  bool deterministic_parser_sema_docs_runbook_sync = false;
  bool deterministic_parser_sema_release_candidate_replay_dry_run = false;
  bool deterministic_parser_sema_advanced_core_shard1 = false;
  bool deterministic_parser_sema_advanced_contract_rejection_shard1 = false;
  bool deterministic_parser_sema_advanced_diagnostics_shard1 = false;
  bool deterministic_parser_sema_advanced_conformance_shard1 = false;
  bool deterministic_parser_sema_advanced_integration_shard1 = false;
  bool deterministic_parser_sema_advanced_performance_shard1 = false;
  bool deterministic_parser_sema_advanced_core_shard2 = false;
  bool deterministic_parser_sema_advanced_contract_rejection_shard2 = false;
  bool deterministic_parser_sema_advanced_diagnostics_shard2 = false;
  bool deterministic_parser_sema_integration_closeout_signoff = false;
  bool deterministic_parser_sema_handoff_publication_transfer_record = false;
  bool deterministic_parser_sema_parity_publication_readiness_record = false;
  bool deterministic_core_semantic_parity_publication_readiness_record = false;
  bool deterministic_core_semantic_summary_readiness_record = false;
  bool deterministic_selector_property_type_annotation_readiness_record = false;
  bool deterministic_type_boundary_summary_readiness_record = false;
  bool deterministic_module_type_abi_summary_readiness_record = false;
  bool deterministic_module_boundary_summary_readiness_record = false;
  bool deterministic_intermodule_flow_summary_readiness_record = false;
  bool deterministic_module_semantic_parity_publication_readiness_record = false;
  bool deterministic_intermodule_flow_parity_publication_readiness_record = false;
  bool deterministic_concurrency_parity_publication_readiness_record = false;
  bool deterministic_unsafe_error_parity_validation_readiness_record = false;
  bool deterministic_control_binding_parity_validation_readiness_record = false;
  bool deterministic_async_block_message_parity_validation_readiness_record = false;
  bool deterministic_dispatch_runtime_arc_parity_validation_readiness_record = false;
  bool deterministic_parser_sema_contract_readiness_record = false;
  bool deterministic_diagnostics_publication_record = false;
  bool deterministic_pass_flow_recovery_record = false;
  bool deterministic_pass_manager_publication_record = false;
  bool deterministic_type_metadata_publication_record = false;
  bool deterministic_type_metadata_mapping_readiness_record = false;
  bool deterministic_atomic_vector_mapping_publication_record = false;
  bool deterministic_typed_semantic_handoff_record = false;
  bool deterministic_parity_closeout_publication_readiness_record = false;
  bool deterministic_parity_validation_record = false;
  bool deterministic_closeout_surface_readiness_record = false;
  bool deterministic_closeout_signoff_record = false;
  bool deterministic_semantic_diagnostics = false;
  bool deterministic_type_metadata_handoff = false;
  bool deterministic_interface_implementation_handoff = false;
  bool deterministic_protocol_category_composition_handoff = false;
  bool deterministic_class_protocol_category_linking_handoff = false;
  bool deterministic_selector_normalization_handoff = false;
  bool deterministic_property_attribute_handoff = false;
  bool deterministic_type_annotation_surface_handoff = false;
  bool deterministic_lightweight_generic_constraint_handoff = false;
  bool deterministic_nullability_flow_warning_precision_handoff = false;
  bool deterministic_protocol_qualified_object_type_handoff = false;
  bool deterministic_variance_bridge_cast_handoff = false;
  bool deterministic_generic_metadata_abi_handoff = false;
  bool deterministic_module_import_graph_handoff = false;
  bool deterministic_namespace_collision_shadowing_handoff = false;
  bool deterministic_public_private_api_partition_handoff = false;
  bool deterministic_incremental_module_cache_invalidation_handoff = false;
  bool deterministic_cross_module_conformance_handoff = false;
  bool deterministic_throws_propagation_handoff = false;
  bool deterministic_async_continuation_handoff = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  bool deterministic_unsafe_pointer_extension_handoff = false;
  bool deterministic_inline_asm_intrinsic_governance_handoff = false;
  bool deterministic_ns_error_bridging_handoff = false;
  bool deterministic_error_diagnostics_recovery_handoff = false;
  bool deterministic_result_like_lowering_handoff = false;
  bool deterministic_unwind_cleanup_handoff = false;
  bool deterministic_await_lowering_suspension_state_lowering_handoff = false;
  bool deterministic_symbol_graph_scope_resolution_handoff = false;
  bool deterministic_method_lookup_override_conflict_handoff = false;
  bool deterministic_property_synthesis_ivar_binding_handoff = false;
  bool deterministic_id_class_sel_object_pointer_type_checking_handoff = false;
  bool deterministic_block_literal_capture_semantics_handoff = false;
  bool deterministic_block_abi_invoke_trampoline_handoff = false;
  bool deterministic_block_storage_escape_handoff = false;
  bool deterministic_block_copy_dispose_handoff = false;
  bool deterministic_block_determinism_perf_baseline_handoff = false;
  bool deterministic_message_send_selector_lowering_handoff = false;
  bool deterministic_dispatch_abi_marshalling_handoff = false;
  bool deterministic_nil_receiver_semantics_foldability_handoff = false;
  bool deterministic_super_dispatch_method_family_handoff = false;
  bool deterministic_runtime_link_host_link_handoff = false;
  bool deterministic_retain_release_operation_handoff = false;
  bool deterministic_weak_unowned_semantics_handoff = false;
  bool deterministic_arc_diagnostics_fixit_handoff = false;
  bool deterministic_autoreleasepool_scope_handoff = false;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3BootstrapLegalityFailureContractSummary
      bootstrap_legality_failure_contract_summary;
  Objc3BootstrapLegalitySemanticsSummary
      bootstrap_legality_semantics_summary;
  Objc3BootstrapFailureRestartSemanticsSummary
      bootstrap_failure_restart_semantics_summary;
  Objc3CompatibilityStrictnessClaimSemanticsSummary
      compatibility_strictness_claim_semantics_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3IncrementalModuleCacheInvalidationSummary incremental_module_cache_invalidation_summary;
  Objc3CrossModuleConformanceSummary cross_module_conformance_summary;
  Objc3ThrowsPropagationSummary throws_propagation_summary;
  Objc3AsyncContinuationSummary async_continuation_summary;
  Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary;
  Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary;
  Objc3ConcurrencyReplayRaceGuardSummary concurrency_replay_race_guard_summary;
  Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary;
  Objc3InlineAsmIntrinsicGovernanceSummary inline_asm_intrinsic_governance_summary;
  Objc3NSErrorBridgingSummary ns_error_bridging_summary;
  Objc3ErrorDiagnosticsRecoverySummary error_diagnostics_recovery_summary;
  Objc3ResultLikeLoweringSummary result_like_lowering_summary;
  Objc3UnwindCleanupSummary unwind_cleanup_summary;
  Objc3AwaitLoweringSuspensionStateSummary
      await_lowering_suspension_state_lowering_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeLinkHostLinkSummary runtime_link_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
  Objc3AtomicMemoryOrderMappingSummary atomic_memory_order_mapping;
  bool deterministic_atomic_memory_order_mapping = false;
  Objc3VectorTypeLoweringSummary vector_type_lowering;
  bool deterministic_vector_type_lowering = false;
  bool ready = false;
};

Objc3SemaCoreSemanticSummaryReadinessRecord
BuildObjc3SemaCoreSemanticSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord
BuildObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaTypeBoundarySummaryReadinessRecord
BuildObjc3SemaTypeBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaModuleTypeAbiSummaryReadinessRecord
BuildObjc3SemaModuleTypeAbiSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaModuleBoundarySummaryReadinessRecord
BuildObjc3SemaModuleBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaTypedSemanticHandoffRecord
BuildObjc3SemaTypedSemanticHandoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaAtomicVectorMappingPublicationRecord
BuildObjc3SemaAtomicVectorMappingPublicationRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3AtomicMemoryOrderMappingSummary &atomic_memory_order_mapping,
    bool deterministic_atomic_memory_order_mapping,
    const Objc3VectorTypeLoweringSummary &vector_type_lowering,
    bool deterministic_vector_type_lowering);

Objc3SemaTypeMetadataMappingReadinessRecord
BuildObjc3SemaTypeMetadataMappingReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

bool Objc3ParserSemaSyncCountsReady(
    std::size_t expected_count,
    std::size_t required_count,
    std::size_t passed_count,
    std::size_t failed_count);

std::size_t Objc3SemaEvidenceCount(bool ready);

Objc3ParserSemaParityPublicationReadinessRecord
BuildObjc3ParserSemaParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffPublicationTransferRecord &transfer_record,
    bool deterministic_transfer_record);

Objc3SemaCoreSemanticParityPublicationReadinessRecord
BuildObjc3SemaCoreSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff,
    bool deterministic_interface_implementation_handoff,
    bool deterministic_protocol_category_composition_handoff,
    bool deterministic_class_protocol_category_linking_handoff,
    bool deterministic_selector_normalization_handoff,
    bool deterministic_property_attribute_handoff,
    bool deterministic_type_annotation_surface_handoff,
    bool deterministic_lightweight_generic_constraint_handoff,
    bool deterministic_nullability_flow_warning_precision_handoff,
    bool deterministic_protocol_qualified_object_type_handoff,
    bool deterministic_variance_bridge_cast_handoff);

Objc3SemaModuleSemanticParityPublicationReadinessRecord
BuildObjc3SemaModuleSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_generic_metadata_abi_handoff,
    bool deterministic_module_import_graph_handoff,
    bool deterministic_namespace_collision_shadowing_handoff,
    bool deterministic_public_private_api_partition_handoff,
    bool deterministic_incremental_module_cache_invalidation_handoff);

Objc3SemaIntermoduleFlowSummaryReadinessRecord
BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff);

Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
BuildObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff);

Objc3SemaConcurrencyParityPublicationReadinessRecord
BuildObjc3SemaConcurrencyParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_actor_isolation_sendability_handoff,
    bool deterministic_task_runtime_cancellation_handoff,
    bool deterministic_concurrency_replay_race_guard_handoff);

Objc3SemaUnsafeErrorParityValidationReadinessRecord
BuildObjc3SemaUnsafeErrorParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unsafe_pointer_extension_handoff,
    bool deterministic_inline_asm_intrinsic_governance_handoff,
    bool deterministic_ns_error_bridging_handoff,
    bool deterministic_error_diagnostics_recovery_handoff,
    bool deterministic_result_like_lowering_handoff);

Objc3SemaControlBindingParityValidationReadinessRecord
BuildObjc3SemaControlBindingParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unwind_cleanup_handoff,
    bool deterministic_async_continuation_handoff,
    bool deterministic_symbol_graph_scope_resolution_handoff,
    bool deterministic_method_lookup_override_conflict_handoff,
    bool deterministic_property_synthesis_ivar_binding_handoff,
    bool deterministic_id_class_sel_object_pointer_type_checking_handoff);

Objc3SemaAsyncBlockMessageParityValidationReadinessRecord
BuildObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_await_lowering_suspension_state_lowering_handoff,
    bool deterministic_block_literal_capture_semantics_handoff,
    bool deterministic_block_abi_invoke_trampoline_handoff,
    bool deterministic_block_storage_escape_handoff,
    bool deterministic_block_copy_dispose_handoff,
    bool deterministic_block_determinism_perf_baseline_handoff,
    bool deterministic_message_send_selector_lowering_handoff);

Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
BuildObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_dispatch_abi_marshalling_handoff,
    bool deterministic_nil_receiver_semantics_foldability_handoff,
    bool deterministic_super_dispatch_method_family_handoff,
    bool deterministic_runtime_link_host_link_handoff,
    bool deterministic_retain_release_operation_handoff,
    bool deterministic_weak_unowned_semantics_handoff,
    bool deterministic_arc_diagnostics_fixit_handoff,
    bool deterministic_autoreleasepool_scope_handoff);

Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3ParserSemaContractReadinessRecord
BuildObjc3ParserSemaContractReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

#include "sema/objc3_sema_pass_manager_closeout_readiness.inc"

Objc3SemaParityCloseoutPublicationReadinessRecord
BuildObjc3SemaParityCloseoutPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    bool pass_manager_executed,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaCloseoutSurfaceReadinessRecord
BuildObjc3SemaCloseoutSurfaceReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaCloseoutSignoffRecord BuildObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

#include "sema/objc3_sema_pass_manager_surface_readiness.inc"

bool IsReadyObjc3SemaParityContractSurface(
    const Objc3SemaParityContractSurface &surface);
