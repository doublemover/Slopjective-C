#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

struct Objc3ParserSemaHandoffPublicationEvidenceRecord {
  std::string handoff_publication_evidence_owner =
      kObjc3ParserSemaHandoffPublicationEvidenceOwner;
  std::string parser_sema_conformance_evidence_owner =
      kObjc3ParserSemaConformanceEvidenceOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
