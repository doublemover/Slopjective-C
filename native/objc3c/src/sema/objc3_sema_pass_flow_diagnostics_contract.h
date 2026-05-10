#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>

#include "sema/objc3_sema_pass_flow_core_contract.h"

struct Objc3SemaDiagnosticsBus {
  std::vector<std::string> *diagnostics = nullptr;
  std::string diagnostic_handoff_owner = kObjc3SemaDiagnosticHandoffOwner;
  std::string diagnostic_catalog_owner = std::string(::kObjc3SemaDiagnosticCatalogOwner);
  std::string diagnostic_fixit_owner = std::string(::kObjc3SemaDiagnosticFixitOwner);
  std::string diagnostic_recovery_owner = std::string(::kObjc3SemaDiagnosticRecoveryOwner);
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool recovery_counts_as_success = false;

  void Publish(const std::string &diagnostic) const {
    if (diagnostics == nullptr) {
      return;
    }
    diagnostics->push_back(diagnostic);
  }

  void PublishBatch(const std::vector<std::string> &batch) const {
    if (diagnostics == nullptr || batch.empty()) {
      return;
    }
    diagnostics->insert(diagnostics->end(), batch.begin(), batch.end());
  }

  std::size_t Count() const {
    if (diagnostics == nullptr) {
      return 0;
    }
    return diagnostics->size();
  }
};

struct Objc3SemaDiagnosticsPublicationRecord {
  std::string diagnostics_publication_owner =
      kObjc3SemaDiagnosticsPublicationOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string diagnostic_handoff_owner = kObjc3SemaDiagnosticHandoffOwner;
  std::string diagnostic_catalog_owner =
      std::string(::kObjc3SemaDiagnosticCatalogOwner);
  std::string diagnostic_fixit_owner =
      std::string(::kObjc3SemaDiagnosticFixitOwner);
  std::string diagnostic_recovery_owner =
      std::string(::kObjc3SemaDiagnosticRecoveryOwner);
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool recovery_counts_as_success = false;
  std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};
  std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};
  std::size_t diagnostics_total = 0;
  bool diagnostics_after_pass_monotonic = false;
  bool diagnostics_emission_totals_consistent = false;
  bool diagnostics_accounting_consistent = false;
  bool diagnostics_bus_publish_consistent = false;
  bool diagnostics_canonicalized = false;
  bool diagnostics_hardening_satisfied = false;
  bool semantic_diagnostics_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaDiagnosticsPublicationRecord(
    const Objc3SemaDiagnosticsPublicationRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.diagnostics_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.diagnostic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.diagnostic_catalog_owner) &&
         Objc3SemaOwnerIsExplicit(record.diagnostic_fixit_owner) &&
         Objc3SemaOwnerIsExplicit(record.diagnostic_recovery_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         !record.recovery_counts_as_success &&
         record.diagnostics_total == record.diagnostics_after_pass.back() &&
         record.diagnostics_after_pass_monotonic &&
         record.diagnostics_emission_totals_consistent &&
         record.diagnostics_accounting_consistent &&
         record.diagnostics_bus_publish_consistent &&
         record.diagnostics_canonicalized &&
         record.diagnostics_hardening_satisfied &&
         record.semantic_diagnostics_ready && record.deterministic;
}

inline Objc3SemaDiagnosticsPublicationRecord
BuildObjc3SemaDiagnosticsPublicationRecord(
    const Objc3SemaPassFlowSummary &pass_flow_summary,
    bool deterministic_semantic_diagnostics) {
  Objc3SemaDiagnosticsPublicationRecord record;
  record.stage_input_owner = pass_flow_summary.stage_input_owner;
  record.diagnostic_handoff_owner = pass_flow_summary.diagnostic_handoff_owner;
  record.diagnostic_catalog_owner = pass_flow_summary.diagnostic_catalog_owner;
  record.diagnostic_fixit_owner = pass_flow_summary.diagnostic_fixit_owner;
  record.diagnostic_recovery_owner = pass_flow_summary.diagnostic_recovery_owner;
  record.owner_model = pass_flow_summary.owner_model;
  record.strict_no_retired_route = pass_flow_summary.strict_no_retired_route;
  record.strict_no_compatibility = pass_flow_summary.strict_no_compatibility;
  record.recovery_counts_as_success =
      pass_flow_summary.recovery_counts_as_success;
  record.diagnostics_after_pass = pass_flow_summary.diagnostics_after_pass;
  record.diagnostics_emitted_by_pass =
      pass_flow_summary.diagnostics_emitted_by_pass;
  record.diagnostics_total = pass_flow_summary.diagnostics_total;
  record.diagnostics_after_pass_monotonic =
      pass_flow_summary.diagnostics_after_pass_monotonic;
  record.diagnostics_emission_totals_consistent =
      pass_flow_summary.diagnostics_emission_totals_consistent;
  record.diagnostics_accounting_consistent =
      pass_flow_summary.diagnostics_accounting_consistent;
  record.diagnostics_bus_publish_consistent =
      pass_flow_summary.diagnostics_bus_publish_consistent;
  record.diagnostics_canonicalized = pass_flow_summary.diagnostics_canonicalized;
  record.diagnostics_hardening_satisfied =
      pass_flow_summary.diagnostics_hardening_satisfied;
  record.semantic_diagnostics_ready = deterministic_semantic_diagnostics;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.diagnostics_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.diagnostic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.diagnostic_catalog_owner) &&
      Objc3SemaOwnerIsExplicit(record.diagnostic_fixit_owner) &&
      Objc3SemaOwnerIsExplicit(record.diagnostic_recovery_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      !record.recovery_counts_as_success &&
      record.diagnostics_total == record.diagnostics_after_pass.back() &&
      record.diagnostics_after_pass_monotonic &&
      record.diagnostics_emission_totals_consistent &&
      record.diagnostics_accounting_consistent &&
      record.diagnostics_bus_publish_consistent &&
      record.diagnostics_canonicalized &&
      record.diagnostics_hardening_satisfied &&
      record.semantic_diagnostics_ready;
  return record;
}

struct Objc3SemaPassFlowRecoveryRecord {
  std::string pass_flow_recovery_owner = kObjc3SemaPassFlowRecoveryOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string diagnostic_handoff_owner = kObjc3SemaDiagnosticHandoffOwner;
  std::string diagnostics_publication_owner =
      kObjc3SemaDiagnosticsPublicationOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool recovery_counts_as_success = false;
  bool parser_recovery_replay_ready = false;
  bool parser_recovery_replay_case_present = false;
  bool parser_recovery_replay_case_passed = false;
  bool recovery_replay_contract_satisfied = false;
  std::string recovery_replay_key;
  bool recovery_replay_key_deterministic = false;
  bool recovery_determinism_hardening_satisfied = false;
  bool diagnostics_publication_ready = false;
  bool robustness_guardrails_satisfied = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaPassFlowRecoveryRecord(
    const Objc3SemaPassFlowRecoveryRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.pass_flow_recovery_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.diagnostic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.diagnostics_publication_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         !record.recovery_counts_as_success &&
         record.parser_recovery_replay_ready &&
         record.parser_recovery_replay_case_present &&
         record.parser_recovery_replay_case_passed &&
         record.recovery_replay_contract_satisfied &&
         record.recovery_replay_key.rfind("sema-pass-recovery:v1:", 0) == 0 &&
         record.recovery_replay_key_deterministic &&
         record.recovery_determinism_hardening_satisfied &&
         record.diagnostics_publication_ready &&
         record.robustness_guardrails_satisfied && record.deterministic;
}

inline Objc3SemaPassFlowRecoveryRecord
BuildObjc3SemaPassFlowRecoveryRecord(
    const Objc3SemaPassFlowSummary &pass_flow_summary,
    const Objc3SemaDiagnosticsPublicationRecord &diagnostics_record) {
  Objc3SemaPassFlowRecoveryRecord record;
  record.stage_input_owner = pass_flow_summary.stage_input_owner;
  record.diagnostic_handoff_owner = pass_flow_summary.diagnostic_handoff_owner;
  record.owner_model = pass_flow_summary.owner_model;
  record.strict_no_retired_route = pass_flow_summary.strict_no_retired_route;
  record.strict_no_compatibility = pass_flow_summary.strict_no_compatibility;
  record.recovery_counts_as_success =
      pass_flow_summary.recovery_counts_as_success;
  record.parser_recovery_replay_ready =
      pass_flow_summary.parser_recovery_replay_ready;
  record.parser_recovery_replay_case_present =
      pass_flow_summary.parser_recovery_replay_case_present;
  record.parser_recovery_replay_case_passed =
      pass_flow_summary.parser_recovery_replay_case_passed;
  record.recovery_replay_contract_satisfied =
      pass_flow_summary.recovery_replay_contract_satisfied;
  record.recovery_replay_key = pass_flow_summary.recovery_replay_key;
  record.recovery_replay_key_deterministic =
      pass_flow_summary.recovery_replay_key_deterministic;
  record.recovery_determinism_hardening_satisfied =
      pass_flow_summary.recovery_determinism_hardening_satisfied;
  record.diagnostics_publication_ready =
      IsReadyObjc3SemaDiagnosticsPublicationRecord(diagnostics_record);
  record.robustness_guardrails_satisfied =
      pass_flow_summary.robustness_guardrails_satisfied;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.pass_flow_recovery_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.diagnostic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.diagnostics_publication_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      !record.recovery_counts_as_success &&
      record.parser_recovery_replay_ready &&
      record.parser_recovery_replay_case_present &&
      record.parser_recovery_replay_case_passed &&
      record.recovery_replay_contract_satisfied &&
      record.recovery_replay_key.rfind("sema-pass-recovery:v1:", 0) == 0 &&
      record.recovery_replay_key_deterministic &&
      record.recovery_determinism_hardening_satisfied &&
      record.diagnostics_publication_ready &&
      record.robustness_guardrails_satisfied;
  return record;
}
