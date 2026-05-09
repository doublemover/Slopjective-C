#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "sema/objc3_sema_canonical_literal_contract.h"
#include "sema/objc3_sema_contract.h"

inline constexpr std::uint32_t kObjc3SemaPassManagerContractVersionMajor = 1;
inline constexpr std::uint32_t kObjc3SemaPassManagerContractVersionMinor = 0;
inline constexpr std::uint32_t kObjc3SemaPassManagerContractVersionPatch = 0;
inline constexpr const char *kObjc3SemaOwnerSplitContractId =
    "objc3c.sema.pass-manager.owner-split.hard-cutover.v1";
inline constexpr const char *kObjc3SemaStageInputOwner =
    "native.frontend.sema.stage-input";
inline constexpr const char *kObjc3SemaTypedSemanticHandoffOwner =
    "native.frontend.sema.typed-handoff";
inline constexpr const char *kObjc3ParserSemaContractHandoffOwner =
    "native.frontend.parser-sema.contract-handoff";
inline constexpr const char *kObjc3ParserSemaSnapshotNormalizationOwner =
    "native.frontend.parser-sema.snapshot-normalization";
inline constexpr const char *kObjc3ParserSemaCanonicalRejectionOwner =
    "native.frontend.parser-sema.canonical-rejection";
inline constexpr const char *kObjc3ParserSemaConformanceEvidenceOwner =
    "native.frontend.parser-sema.conformance-evidence";
inline constexpr const char *kObjc3ParserSemaHandoffPublicationEvidenceOwner =
    "native.frontend.parser-sema.handoff-publication-evidence";
inline constexpr const char *kObjc3ParserSemaHandoffPublicationTransferOwner =
    "native.frontend.parser-sema.handoff-publication-transfer";
inline constexpr const char *kObjc3ParserSemaParityPublicationReadinessOwner =
    "native.frontend.parser-sema.parity-publication-readiness";
inline constexpr const char *kObjc3SemaCoreSemanticParityPublicationReadinessOwner =
    "native.frontend.sema.core-semantic-parity-publication-readiness";
inline constexpr const char *kObjc3SemaModuleSemanticParityPublicationReadinessOwner =
    "native.frontend.sema.module-semantic-parity-publication-readiness";
inline constexpr const char *kObjc3SemaIntermoduleFlowParityPublicationReadinessOwner =
    "native.frontend.sema.intermodule-flow-parity-publication-readiness";
inline constexpr const char *kObjc3SemaConcurrencyParityPublicationReadinessOwner =
    "native.frontend.sema.concurrency-parity-publication-readiness";
inline constexpr const char *kObjc3SemaUnsafeErrorParityValidationReadinessOwner =
    "native.frontend.sema.unsafe-error-parity-validation-readiness";
inline constexpr const char *kObjc3ParserSemaHandoffScaffoldReadinessOwner =
    "native.frontend.parser-sema.handoff-scaffold-readiness";
inline constexpr const char *kObjc3ParserSemaContractReadinessOwner =
    "native.frontend.parser-sema.contract-readiness";
inline constexpr const char *kObjc3SemaDiagnosticHandoffOwner =
    "native.frontend.sema.diagnostic-stage";
inline constexpr const char *kObjc3SemaDiagnosticsPublicationOwner =
    "native.frontend.sema.diagnostics-publication";
inline constexpr const char *kObjc3SemaPassFlowRecoveryOwner =
    "native.frontend.sema.pass-flow-recovery";
inline constexpr const char *kObjc3SemaPassManagerPublicationOwner =
    "native.frontend.sema.pass-manager-publication";
inline constexpr const char *kObjc3SemaTypeMetadataPublicationOwner =
    "native.frontend.sema.type-metadata-publication";
inline constexpr const char *kObjc3SemaTypeMetadataMappingReadinessOwner =
    "native.frontend.sema.type-metadata-mapping-readiness";
inline constexpr const char *kObjc3SemaTypedSemanticHandoffPublicationOwner =
    "native.frontend.sema.typed-semantic-handoff-publication";
inline constexpr const char *kObjc3SemaParityValidationOwner =
    "native.frontend.sema.parity-validation";
inline constexpr const char *kObjc3SemaCloseoutSignoffOwner =
    "native.frontend.sema.closeout-signoff";
inline constexpr const char *kObjc3SemaNoFallbackOwnerModel =
    "strict-hard-cutover-no-fallback-no-compatibility-shim";

enum class Objc3SemaPassId {
  BuildIntegrationSurface = 0,
  ValidateBodies = 1,
  ValidatePureContract = 2,
};

enum class Objc3SemaLanguageProfile : std::uint8_t {
  Canonical = 0,
};

inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder = {
    Objc3SemaPassId::BuildIntegrationSurface,
    Objc3SemaPassId::ValidateBodies,
    Objc3SemaPassId::ValidatePureContract,
};

inline bool IsMonotonicObjc3SemaDiagnosticsAfterPass(const std::array<std::size_t, 3> &diagnostics_after_pass) {
  for (std::size_t i = 1; i < diagnostics_after_pass.size(); ++i) {
    if (diagnostics_after_pass[i] < diagnostics_after_pass[i - 1]) {
      return false;
    }
  }
  return true;
}

struct Objc3SemaPassFlowSummary {
  std::array<Objc3SemaPassId, 3> configured_pass_order = kObjc3SemaPassOrder;
  std::array<bool, 3> pass_executed = {false, false, false};
  std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};
  std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};
  Objc3SemaLanguageProfile language_profile = Objc3SemaLanguageProfile::Canonical;
  std::size_t canonical_literal_rejection_total_sites = 0;
  std::size_t configured_pass_count = kObjc3SemaPassOrder.size();
  std::size_t executed_pass_count = 0;
  std::size_t duplicate_pass_execution_count = 0;
  std::size_t missing_pass_execution_count = 0;
  std::size_t diagnostics_total = 0;
  std::size_t transition_edge_count = 0;
  std::size_t symbol_globals_count = 0;
  std::size_t symbol_functions_count = 0;
  std::size_t symbol_interfaces_count = 0;
  std::size_t symbol_implementations_count = 0;
  std::size_t type_metadata_global_entries = 0;
  std::size_t type_metadata_function_entries = 0;
  std::size_t type_metadata_interface_entries = 0;
  std::size_t type_metadata_implementation_entries = 0;
  bool pass_order_matches_contract = false;
  bool diagnostics_after_pass_monotonic = false;
  bool diagnostics_emission_totals_consistent = false;
  bool diagnostics_accounting_consistent = false;
  bool diagnostics_bus_publish_consistent = false;
  bool diagnostics_canonicalized = false;
  bool diagnostics_hardening_satisfied = false;
  bool parser_recovery_replay_ready = false;
  bool parser_recovery_replay_case_present = false;
  bool parser_recovery_replay_case_passed = false;
  bool recovery_replay_contract_satisfied = false;
  std::string recovery_replay_key;
  bool recovery_replay_key_deterministic = false;
  bool recovery_determinism_hardening_satisfied = false;
  bool compatibility_handoff_consistent = false;
  bool robustness_guardrails_satisfied = false;
  bool symbol_flow_counts_consistent = false;
  std::uint64_t pass_execution_fingerprint = 1469598103934665603ull;
  std::string deterministic_handoff_key;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner = kObjc3SemaTypedSemanticHandoffOwner;
  std::string diagnostic_handoff_owner = kObjc3SemaDiagnosticHandoffOwner;
  std::string diagnostic_catalog_owner = std::string(::kObjc3SemaDiagnosticCatalogOwner);
  std::string diagnostic_fixit_owner = std::string(::kObjc3SemaDiagnosticFixitOwner);
  std::string diagnostic_recovery_owner = std::string(::kObjc3SemaDiagnosticRecoveryOwner);
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool owner_split_explicit = false;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool recovery_counts_as_success = false;
  bool replay_key_deterministic = false;
  bool deterministic = false;
};

inline bool Objc3SemaOwnerIsExplicit(const std::string &owner) {
  return !owner.empty() && owner.rfind("native.", 0) == 0;
}

inline bool Objc3SemaOwnerSplitIsReady(
    const std::string &stage_input_owner,
    const std::string &typed_semantic_handoff_owner,
    const std::string &diagnostic_handoff_owner,
    const std::string &owner_model,
    bool strict_no_fallback,
    bool strict_no_compatibility) {
  return Objc3SemaOwnerIsExplicit(stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(diagnostic_handoff_owner) &&
         owner_model == kObjc3SemaNoFallbackOwnerModel &&
         strict_no_fallback && strict_no_compatibility &&
         Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kSemantic);
}

inline bool IsReadyObjc3SemaPassFlowSummary(const Objc3SemaPassFlowSummary &summary) {
  return summary.pass_order_matches_contract &&
         summary.configured_pass_count == kObjc3SemaPassOrder.size() &&
         summary.executed_pass_count == summary.configured_pass_count &&
         summary.diagnostics_total == summary.diagnostics_after_pass.back() &&
         summary.transition_edge_count + 1u == summary.executed_pass_count &&
         summary.pass_executed[static_cast<std::size_t>(Objc3SemaPassId::BuildIntegrationSurface)] &&
         summary.pass_executed[static_cast<std::size_t>(Objc3SemaPassId::ValidateBodies)] &&
         summary.pass_executed[static_cast<std::size_t>(Objc3SemaPassId::ValidatePureContract)] &&
         summary.diagnostics_after_pass_monotonic &&
         summary.diagnostics_emission_totals_consistent &&
         summary.diagnostics_accounting_consistent &&
         summary.diagnostics_bus_publish_consistent &&
         summary.diagnostics_canonicalized &&
         summary.diagnostics_hardening_satisfied &&
         summary.parser_recovery_replay_ready &&
         summary.parser_recovery_replay_case_present &&
         summary.parser_recovery_replay_case_passed &&
         summary.recovery_replay_contract_satisfied &&
         !summary.recovery_replay_key.empty() &&
         summary.recovery_replay_key_deterministic &&
         summary.recovery_determinism_hardening_satisfied &&
         summary.compatibility_handoff_consistent &&
         summary.robustness_guardrails_satisfied &&
         summary.symbol_flow_counts_consistent &&
         summary.pass_execution_fingerprint != 1469598103934665603ull &&
         !summary.deterministic_handoff_key.empty() &&
         summary.owner_split_explicit &&
         Objc3SemaOwnerIsExplicit(summary.diagnostic_catalog_owner) &&
         Objc3SemaOwnerIsExplicit(summary.diagnostic_fixit_owner) &&
         Objc3SemaOwnerIsExplicit(summary.diagnostic_recovery_owner) &&
         Objc3SemaOwnerSplitIsReady(
             summary.stage_input_owner,
             summary.typed_semantic_handoff_owner,
             summary.diagnostic_handoff_owner,
             summary.owner_model,
             summary.strict_no_fallback,
             summary.strict_no_compatibility) &&
         !summary.recovery_counts_as_success &&
         summary.replay_key_deterministic &&
         summary.symbol_globals_count == summary.type_metadata_global_entries &&
         summary.symbol_functions_count == summary.type_metadata_function_entries &&
         summary.symbol_interfaces_count == summary.type_metadata_interface_entries &&
         summary.symbol_implementations_count == summary.type_metadata_implementation_entries &&
         summary.deterministic;
}

struct Objc3SemaDiagnosticsBus {
  std::vector<std::string> *diagnostics = nullptr;
  std::string diagnostic_handoff_owner = kObjc3SemaDiagnosticHandoffOwner;
  std::string diagnostic_catalog_owner = std::string(::kObjc3SemaDiagnosticCatalogOwner);
  std::string diagnostic_fixit_owner = std::string(::kObjc3SemaDiagnosticFixitOwner);
  std::string diagnostic_recovery_owner = std::string(::kObjc3SemaDiagnosticRecoveryOwner);
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
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
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
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
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
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
  record.strict_no_fallback = pass_flow_summary.strict_no_fallback;
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
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
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
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
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
  record.strict_no_fallback = pass_flow_summary.strict_no_fallback;
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
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
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

struct Objc3SemaPassManagerInput {
  const Objc3ParsedProgram *program = nullptr;
  const Objc3ParserContractSnapshot *parser_contract_snapshot = nullptr;
  Objc3SemanticValidationOptions validation_options;
  Objc3SemaLanguageProfile language_profile = Objc3SemaLanguageProfile::Canonical;
  Objc3SemaCanonicalLiteralRejectionCounts canonical_literal_rejection_counts;
  Objc3SemaDiagnosticsBus diagnostics_bus;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner = kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool recovery_counts_as_success = false;
};

struct Objc3ParserSemaHandoffOwnerRecord {
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner = kObjc3SemaTypedSemanticHandoffOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string snapshot_normalization_owner =
      kObjc3ParserSemaSnapshotNormalizationOwner;
  std::string canonical_rejection_owner =
      kObjc3ParserSemaCanonicalRejectionOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool strict_snapshot_normalization_rejection = true;
  bool strict_canonical_literal_rejection = true;
  bool parser_contract_snapshot_supplied = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaHandoffOwnerRecord(
    const Objc3ParserSemaHandoffOwnerRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.snapshot_normalization_owner) &&
         Objc3SemaOwnerIsExplicit(record.canonical_rejection_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.strict_snapshot_normalization_rejection &&
         record.strict_canonical_literal_rejection && record.deterministic;
}

inline Objc3ParserSemaHandoffOwnerRecord
BuildObjc3ParserSemaHandoffOwnerRecord(const Objc3SemaPassManagerInput &input) {
  Objc3ParserSemaHandoffOwnerRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.parser_contract_snapshot_supplied =
      input.parser_contract_snapshot != nullptr;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.snapshot_normalization_owner) &&
      Objc3SemaOwnerIsExplicit(record.canonical_rejection_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.strict_snapshot_normalization_rejection &&
      record.strict_canonical_literal_rejection;
  return record;
}

struct Objc3SemaPassManagerPublicationRecord {
  Objc3ParserSemaHandoffOwnerRecord parser_sema_handoff_owner_record;
  Objc3SemaDiagnosticsPublicationRecord diagnostics_publication_record;
  std::string pass_manager_publication_owner =
      kObjc3SemaPassManagerPublicationOwner;
  std::string pass_flow_owner = kObjc3SemaStageInputOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string diagnostic_handoff_owner = kObjc3SemaDiagnosticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool parser_sema_handoff_owner_ready = false;
  bool pass_flow_summary_ready = false;
  bool semantic_diagnostics_ready = false;
  bool type_metadata_handoff_ready = false;
  bool diagnostics_publication_ready = false;
  bool diagnostics_publication_record_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaPassManagerPublicationRecord(
    const Objc3SemaPassManagerPublicationRecord &record) {
  return IsReadyObjc3ParserSemaHandoffOwnerRecord(
             record.parser_sema_handoff_owner_record) &&
         Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.pass_flow_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.diagnostic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.parser_sema_handoff_owner_ready &&
         record.pass_flow_summary_ready && record.semantic_diagnostics_ready &&
         record.type_metadata_handoff_ready &&
         record.diagnostics_publication_ready &&
         record.diagnostics_publication_record_ready && record.deterministic;
}

inline Objc3SemaPassManagerPublicationRecord
BuildObjc3SemaPassManagerPublicationRecord(
    const Objc3ParserSemaHandoffOwnerRecord &parser_owner_record,
    const Objc3SemaPassFlowSummary &pass_flow_summary,
    const Objc3SemaDiagnosticsPublicationRecord &diagnostics_record,
    bool deterministic_type_metadata_handoff) {
  Objc3SemaPassManagerPublicationRecord record;
  record.parser_sema_handoff_owner_record = parser_owner_record;
  record.diagnostics_publication_record = diagnostics_record;
  record.pass_flow_owner = pass_flow_summary.stage_input_owner;
  record.diagnostic_handoff_owner = pass_flow_summary.diagnostic_handoff_owner;
  record.owner_model = pass_flow_summary.owner_model;
  record.strict_no_fallback = pass_flow_summary.strict_no_fallback;
  record.strict_no_compatibility = pass_flow_summary.strict_no_compatibility;
  record.parser_sema_handoff_owner_ready =
      IsReadyObjc3ParserSemaHandoffOwnerRecord(parser_owner_record);
  record.pass_flow_summary_ready =
      IsReadyObjc3SemaPassFlowSummary(pass_flow_summary);
  record.semantic_diagnostics_ready =
      diagnostics_record.semantic_diagnostics_ready;
  record.type_metadata_handoff_ready = deterministic_type_metadata_handoff;
  record.diagnostics_publication_ready =
      pass_flow_summary.diagnostics_hardening_satisfied &&
      pass_flow_summary.diagnostics_bus_publish_consistent &&
      pass_flow_summary.diagnostics_canonicalized;
  record.diagnostics_publication_record_ready =
      IsReadyObjc3SemaDiagnosticsPublicationRecord(diagnostics_record);
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_flow_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.diagnostic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.parser_sema_handoff_owner_ready &&
      record.pass_flow_summary_ready && record.semantic_diagnostics_ready &&
      record.type_metadata_handoff_ready &&
      record.diagnostics_publication_ready &&
      record.diagnostics_publication_record_ready;
  return record;
}

struct Objc3SemaTypeMetadataPublicationRecord {
  std::string integration_surface_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t integration_surface_global_count = 0;
  std::size_t integration_surface_function_count = 0;
  std::size_t integration_surface_interface_count = 0;
  std::size_t integration_surface_implementation_count = 0;
  std::size_t type_metadata_global_entries = 0;
  std::size_t type_metadata_function_entries = 0;
  std::size_t type_metadata_interface_entries = 0;
  std::size_t type_metadata_implementation_entries = 0;
  bool cardinality_consistent = false;
  bool interface_implementation_handoff_ready = false;
  bool protocol_category_composition_handoff_ready = false;
  bool class_protocol_category_linking_handoff_ready = false;
  bool selector_normalization_handoff_ready = false;
  bool deterministic_type_metadata_handoff = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaTypeMetadataPublicationRecord(
    const Objc3SemaTypeMetadataPublicationRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.cardinality_consistent &&
         record.interface_implementation_handoff_ready &&
         record.protocol_category_composition_handoff_ready &&
         record.class_protocol_category_linking_handoff_ready &&
         record.selector_normalization_handoff_ready &&
         record.deterministic_type_metadata_handoff && record.deterministic;
}

inline Objc3SemaTypeMetadataPublicationRecord
BuildObjc3SemaTypeMetadataPublicationRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    bool deterministic_type_metadata_handoff,
    bool deterministic_interface_implementation_handoff,
    bool deterministic_protocol_category_composition_handoff,
    bool deterministic_class_protocol_category_linking_handoff,
    bool deterministic_selector_normalization_handoff) {
  Objc3SemaTypeMetadataPublicationRecord record;
  record.integration_surface_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.integration_surface_global_count = integration_surface.globals.size();
  record.integration_surface_function_count =
      integration_surface.functions.size();
  record.integration_surface_interface_count =
      integration_surface.interfaces.size();
  record.integration_surface_implementation_count =
      integration_surface.implementations.size();
  record.type_metadata_global_entries =
      type_metadata_handoff.global_names_lexicographic.size();
  record.type_metadata_function_entries =
      type_metadata_handoff.functions_lexicographic.size();
  record.type_metadata_interface_entries =
      type_metadata_handoff.interfaces_lexicographic.size();
  record.type_metadata_implementation_entries =
      type_metadata_handoff.implementations_lexicographic.size();
  record.cardinality_consistent =
      record.integration_surface_global_count ==
          record.type_metadata_global_entries &&
      record.integration_surface_function_count ==
          record.type_metadata_function_entries &&
      record.integration_surface_interface_count ==
          record.type_metadata_interface_entries &&
      record.integration_surface_implementation_count ==
          record.type_metadata_implementation_entries;
  record.interface_implementation_handoff_ready =
      deterministic_interface_implementation_handoff &&
      type_metadata_handoff.interface_implementation_summary.deterministic;
  record.protocol_category_composition_handoff_ready =
      deterministic_protocol_category_composition_handoff &&
      type_metadata_handoff.protocol_category_composition_summary.deterministic;
  record.class_protocol_category_linking_handoff_ready =
      deterministic_class_protocol_category_linking_handoff &&
      type_metadata_handoff.class_protocol_category_linking_summary
          .deterministic;
  record.selector_normalization_handoff_ready =
      deterministic_selector_normalization_handoff &&
      type_metadata_handoff.selector_normalization_summary.deterministic;
  record.deterministic_type_metadata_handoff =
      deterministic_type_metadata_handoff;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.cardinality_consistent &&
      record.interface_implementation_handoff_ready &&
      record.protocol_category_composition_handoff_ready &&
      record.class_protocol_category_linking_handoff_ready &&
      record.selector_normalization_handoff_ready &&
      record.deterministic_type_metadata_handoff;
  return record;
}

struct Objc3SemaTypedSemanticHandoffRecord {
  std::string typed_semantic_handoff_publication_owner =
      kObjc3SemaTypedSemanticHandoffPublicationOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool type_metadata_identity_handoffs_ready = false;
  bool type_annotation_handoffs_ready = false;
  bool module_boundary_handoffs_ready = false;
  bool concurrency_recovery_handoffs_ready = false;
  bool symbol_dispatch_handoffs_ready = false;
  bool block_dispatch_handoffs_ready = false;
  bool ownership_runtime_handoffs_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaTypedSemanticHandoffRecord(
    const Objc3SemaTypedSemanticHandoffRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.typed_semantic_handoff_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.type_metadata_identity_handoffs_ready &&
         record.type_annotation_handoffs_ready &&
         record.module_boundary_handoffs_ready &&
         record.concurrency_recovery_handoffs_ready &&
         record.symbol_dispatch_handoffs_ready &&
         record.block_dispatch_handoffs_ready &&
         record.ownership_runtime_handoffs_ready && record.deterministic;
}

struct Objc3SemaTypeMetadataMappingReadinessRecord {
  std::string type_metadata_mapping_readiness_owner =
      kObjc3SemaTypeMetadataMappingReadinessOwner;
  std::string integration_surface_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t globals_total = 0;
  std::size_t functions_total = 0;
  std::size_t interfaces_total = 0;
  std::size_t implementations_total = 0;
  std::size_t type_metadata_global_entries = 0;
  std::size_t type_metadata_function_entries = 0;
  std::size_t type_metadata_interface_entries = 0;
  std::size_t type_metadata_implementation_entries = 0;
  bool type_metadata_publication_ready = false;
  bool type_metadata_handoff_ready = false;
  bool cardinality_consistent = false;
  bool atomic_memory_order_mapping_ready = false;
  bool vector_type_lowering_ready = false;
  bool mapping_summaries_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
    const Objc3SemaTypeMetadataMappingReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.type_metadata_mapping_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.type_metadata_publication_ready &&
         record.type_metadata_handoff_ready &&
         record.cardinality_consistent &&
         record.atomic_memory_order_mapping_ready &&
         record.vector_type_lowering_ready && record.mapping_summaries_ready &&
         record.deterministic;
}

struct Objc3SemaParityValidationRecord {
  std::string parity_validation_owner = kObjc3SemaParityValidationOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string pass_manager_publication_owner =
      kObjc3SemaPassManagerPublicationOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool pass_manager_executed = false;
  bool parser_sema_contract_ready = false;
  bool pass_flow_summary_ready = false;
  bool publication_records_ready = false;
  bool diagnostics_publication_ready = false;
  bool pass_flow_recovery_ready = false;
  bool type_metadata_cardinality_ready = false;
  bool typed_semantic_handoffs_ready = false;
  bool mapping_summaries_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaParityValidationRecord(
    const Objc3SemaParityValidationRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.pass_manager_executed && record.parser_sema_contract_ready &&
         record.pass_flow_summary_ready && record.publication_records_ready &&
         record.diagnostics_publication_ready &&
         record.pass_flow_recovery_ready &&
         record.type_metadata_cardinality_ready &&
         record.typed_semantic_handoffs_ready &&
         record.mapping_summaries_ready && record.deterministic;
}

inline Objc3SemaParityValidationRecord BuildObjc3SemaParityValidationRecord(
    const Objc3SemaPassManagerInput &input,
    bool pass_manager_executed,
    bool parser_sema_contract_ready,
    bool pass_flow_summary_ready,
    bool publication_records_ready,
    bool diagnostics_publication_ready,
    bool pass_flow_recovery_ready,
    bool type_metadata_cardinality_ready,
    bool typed_semantic_handoffs_ready,
    bool mapping_summaries_ready) {
  Objc3SemaParityValidationRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.pass_manager_executed = pass_manager_executed;
  record.parser_sema_contract_ready = parser_sema_contract_ready;
  record.pass_flow_summary_ready = pass_flow_summary_ready;
  record.publication_records_ready = publication_records_ready;
  record.diagnostics_publication_ready = diagnostics_publication_ready;
  record.pass_flow_recovery_ready = pass_flow_recovery_ready;
  record.type_metadata_cardinality_ready = type_metadata_cardinality_ready;
  record.typed_semantic_handoffs_ready = typed_semantic_handoffs_ready;
  record.mapping_summaries_ready = mapping_summaries_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.pass_manager_executed && record.parser_sema_contract_ready &&
      record.pass_flow_summary_ready && record.publication_records_ready &&
      record.diagnostics_publication_ready &&
      record.pass_flow_recovery_ready &&
      record.type_metadata_cardinality_ready &&
      record.typed_semantic_handoffs_ready && record.mapping_summaries_ready;
  return record;
}

struct Objc3SemaCloseoutSignoffRecord {
  std::string closeout_signoff_owner = kObjc3SemaCloseoutSignoffOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parity_validation_owner = kObjc3SemaParityValidationOwner;
  std::string pass_manager_publication_owner =
      kObjc3SemaPassManagerPublicationOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool parity_validation_ready = false;
  bool parser_sema_closeout_ready = false;
  bool pass_manager_publication_ready = false;
  bool type_metadata_publication_ready = false;
  bool diagnostics_publication_ready = false;
  bool pass_flow_recovery_ready = false;
  bool type_metadata_handoff_ready = false;
  bool typed_semantic_handoffs_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaCloseoutSignoffRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.closeout_signoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
         Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.parity_validation_ready && record.parser_sema_closeout_ready &&
         record.pass_manager_publication_ready &&
         record.type_metadata_publication_ready &&
         record.diagnostics_publication_ready &&
         record.pass_flow_recovery_ready && record.type_metadata_handoff_ready &&
         record.typed_semantic_handoffs_ready && record.deterministic;
}

inline Objc3SemaCloseoutSignoffRecord BuildObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaPassManagerInput &input,
    bool parity_validation_ready,
    bool parser_sema_closeout_ready,
    bool pass_manager_publication_ready,
    bool type_metadata_publication_ready,
    bool diagnostics_publication_ready,
    bool pass_flow_recovery_ready,
    bool type_metadata_handoff_ready,
    bool typed_semantic_handoffs_ready) {
  Objc3SemaCloseoutSignoffRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.parity_validation_ready = parity_validation_ready;
  record.parser_sema_closeout_ready = parser_sema_closeout_ready;
  record.pass_manager_publication_ready = pass_manager_publication_ready;
  record.type_metadata_publication_ready = type_metadata_publication_ready;
  record.diagnostics_publication_ready = diagnostics_publication_ready;
  record.pass_flow_recovery_ready = pass_flow_recovery_ready;
  record.type_metadata_handoff_ready = type_metadata_handoff_ready;
  record.typed_semantic_handoffs_ready = typed_semantic_handoffs_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.closeout_signoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.parity_validation_ready && record.parser_sema_closeout_ready &&
      record.pass_manager_publication_ready &&
      record.type_metadata_publication_ready &&
      record.diagnostics_publication_ready &&
      record.pass_flow_recovery_ready && record.type_metadata_handoff_ready &&
      record.typed_semantic_handoffs_ready;
  return record;
}

struct Objc3ParserSemaConformanceMatrix {
  std::size_t parser_top_level_declaration_count = 0;
  std::size_t ast_top_level_declaration_count = 0;
  std::size_t parser_global_decl_count = 0;
  std::size_t ast_global_decl_count = 0;
  std::size_t parser_protocol_decl_count = 0;
  std::size_t ast_protocol_decl_count = 0;
  std::size_t parser_interface_decl_count = 0;
  std::size_t ast_interface_decl_count = 0;
  std::size_t parser_implementation_decl_count = 0;
  std::size_t ast_implementation_decl_count = 0;
  std::size_t parser_function_decl_count = 0;
  std::size_t ast_function_decl_count = 0;
  std::size_t parser_protocol_property_decl_count = 0;
  std::size_t ast_protocol_property_decl_count = 0;
  std::size_t parser_protocol_method_decl_count = 0;
  std::size_t ast_protocol_method_decl_count = 0;
  std::size_t parser_protocol_class_method_decl_count = 0;
  std::size_t ast_protocol_class_method_decl_count = 0;
  std::size_t parser_protocol_instance_method_decl_count = 0;
  std::size_t ast_protocol_instance_method_decl_count = 0;
  std::size_t parser_interface_property_decl_count = 0;
  std::size_t ast_interface_property_decl_count = 0;
  std::size_t parser_interface_method_decl_count = 0;
  std::size_t ast_interface_method_decl_count = 0;
  std::size_t parser_interface_class_method_decl_count = 0;
  std::size_t ast_interface_class_method_decl_count = 0;
  std::size_t parser_interface_instance_method_decl_count = 0;
  std::size_t ast_interface_instance_method_decl_count = 0;
  std::size_t parser_implementation_property_decl_count = 0;
  std::size_t ast_implementation_property_decl_count = 0;
  std::size_t parser_implementation_method_decl_count = 0;
  std::size_t ast_implementation_method_decl_count = 0;
  std::size_t parser_implementation_class_method_decl_count = 0;
  std::size_t ast_implementation_class_method_decl_count = 0;
  std::size_t parser_implementation_instance_method_decl_count = 0;
  std::size_t ast_implementation_instance_method_decl_count = 0;
  std::size_t parser_interface_category_decl_count = 0;
  std::size_t ast_interface_category_decl_count = 0;
  std::size_t parser_implementation_category_decl_count = 0;
  std::size_t ast_implementation_category_decl_count = 0;
  std::size_t parser_function_prototype_count = 0;
  std::size_t ast_function_prototype_count = 0;
  std::size_t parser_function_pure_count = 0;
  std::size_t ast_function_pure_count = 0;
  std::uint64_t parser_ast_shape_fingerprint = 0;
  std::uint64_t ast_shape_fingerprint = 0;
  std::uint64_t parser_ast_top_level_layout_fingerprint = 0;
  std::uint64_t ast_top_level_layout_fingerprint = 0;
  std::uint64_t parser_contract_snapshot_fingerprint = 0;
  std::uint64_t expected_parser_contract_snapshot_fingerprint = 0;
  bool top_level_declaration_count_matches = false;
  bool global_decl_count_matches = false;
  bool protocol_decl_count_matches = false;
  bool interface_decl_count_matches = false;
  bool implementation_decl_count_matches = false;
  bool function_decl_count_matches = false;
  bool protocol_property_decl_count_matches = false;
  bool protocol_method_decl_count_matches = false;
  bool protocol_class_method_decl_count_matches = false;
  bool protocol_instance_method_decl_count_matches = false;
  bool interface_property_decl_count_matches = false;
  bool interface_method_decl_count_matches = false;
  bool interface_class_method_decl_count_matches = false;
  bool interface_instance_method_decl_count_matches = false;
  bool implementation_property_decl_count_matches = false;
  bool implementation_method_decl_count_matches = false;
  bool implementation_class_method_decl_count_matches = false;
  bool implementation_instance_method_decl_count_matches = false;
  bool interface_category_decl_count_matches = false;
  bool implementation_category_decl_count_matches = false;
  bool function_prototype_count_matches = false;
  bool function_pure_count_matches = false;
  bool ast_shape_fingerprint_matches = false;
  bool ast_top_level_layout_fingerprint_matches = false;
  bool parser_contract_snapshot_fingerprint_matches = false;
  bool parser_diagnostic_budget_consistent = false;
  bool parser_token_top_level_budget_consistent = false;
  bool parser_subset_count_consistent = false;
  bool parser_contract_snapshot_deterministic = false;
  bool parser_recovery_replay_ready = false;
  bool deterministic = false;
};

struct Objc3ParserSemaConformanceCorpus {
  std::size_t required_case_count = 0;
  std::size_t passed_case_count = 0;
  std::size_t failed_case_count = 0;
  bool has_top_level_declaration_count_case = false;
  bool has_snapshot_fingerprint_case = false;
  bool has_diagnostic_budget_case = false;
  bool has_subset_count_case = false;
  bool has_recovery_replay_case = false;
  bool top_level_declaration_count_case_passed = false;
  bool snapshot_fingerprint_case_passed = false;
  bool diagnostic_budget_case_passed = false;
  bool subset_count_case_passed = false;
  bool recovery_replay_case_passed = false;
  bool deterministic = false;
};

struct Objc3ParserSemaConformanceEvidenceRecord {
  std::string parser_sema_conformance_evidence_owner =
      kObjc3ParserSemaConformanceEvidenceOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_matrix_evidence_count = 30u;
  std::size_t passed_matrix_evidence_count = 0;
  std::size_t required_corpus_case_count = 5u;
  std::size_t passed_corpus_case_count = 0;
  std::size_t failed_corpus_case_count = 0;
  bool conformance_matrix_deterministic = false;
  bool conformance_corpus_deterministic = false;
  bool declaration_count_evidence_ready = false;
  bool member_count_evidence_ready = false;
  bool category_function_evidence_ready = false;
  bool fingerprint_evidence_ready = false;
  bool parser_budget_replay_evidence_ready = false;
  bool corpus_inventory_ready = false;
  bool corpus_cases_passed = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3ParserSemaConformanceEvidenceRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.parser_sema_conformance_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_matrix_evidence_count == 30u &&
         record.passed_matrix_evidence_count ==
             record.required_matrix_evidence_count &&
         record.required_corpus_case_count == 5u &&
         record.passed_corpus_case_count == record.required_corpus_case_count &&
         record.failed_corpus_case_count == 0u &&
         record.conformance_matrix_deterministic &&
         record.conformance_corpus_deterministic &&
         record.declaration_count_evidence_ready &&
         record.member_count_evidence_ready &&
         record.category_function_evidence_ready &&
         record.fingerprint_evidence_ready &&
         record.parser_budget_replay_evidence_ready &&
         record.corpus_inventory_ready && record.corpus_cases_passed &&
         record.deterministic;
}

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

struct Objc3SemaUnsafeErrorParityValidationReadinessRecord {
  std::string unsafe_error_parity_validation_readiness_owner =
      kObjc3SemaUnsafeErrorParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 5u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool unsafe_pointer_extension_ready = false;
  bool inline_asm_intrinsic_governance_ready = false;
  bool ns_error_bridging_ready = false;
  bool error_diagnostics_recovery_ready = false;
  bool result_like_lowering_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaUnsafeErrorParityValidationReadinessRecord(
    const Objc3SemaUnsafeErrorParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.unsafe_error_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_validation_count == 5u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.unsafe_pointer_extension_ready &&
         record.inline_asm_intrinsic_governance_ready &&
         record.ns_error_bridging_ready &&
         record.error_diagnostics_recovery_ready &&
         record.result_like_lowering_ready && record.deterministic;
}

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
  Objc3SemaModuleSemanticParityPublicationReadinessRecord
      module_semantic_parity_publication_readiness_record;
  Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
      intermodule_flow_parity_publication_readiness_record;
  Objc3SemaConcurrencyParityPublicationReadinessRecord
      concurrency_parity_publication_readiness_record;
  Objc3SemaUnsafeErrorParityValidationReadinessRecord
      unsafe_error_parity_validation_readiness_record;
  Objc3ParserSemaContractReadinessRecord
      parser_sema_contract_readiness_record;
  Objc3SemaPassFlowSummary sema_pass_flow_summary;
  Objc3SemaDiagnosticsPublicationRecord diagnostics_publication_record;
  Objc3SemaPassFlowRecoveryRecord pass_flow_recovery_record;
  Objc3SemaPassManagerPublicationRecord pass_manager_publication_record;
  Objc3SemaTypeMetadataPublicationRecord type_metadata_publication_record;
  Objc3SemaTypeMetadataMappingReadinessRecord
      type_metadata_mapping_readiness_record;
  Objc3SemaTypedSemanticHandoffRecord typed_semantic_handoff_record;
  Objc3SemaParityValidationRecord parity_validation_record;
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
  bool deterministic_module_semantic_parity_publication_readiness_record = false;
  bool deterministic_intermodule_flow_parity_publication_readiness_record = false;
  bool deterministic_concurrency_parity_publication_readiness_record = false;
  bool deterministic_unsafe_error_parity_validation_readiness_record = false;
  bool deterministic_parser_sema_contract_readiness_record = false;
  bool deterministic_diagnostics_publication_record = false;
  bool deterministic_pass_flow_recovery_record = false;
  bool deterministic_pass_manager_publication_record = false;
  bool deterministic_type_metadata_publication_record = false;
  bool deterministic_type_metadata_mapping_readiness_record = false;
  bool deterministic_typed_semantic_handoff_record = false;
  bool deterministic_parity_validation_record = false;
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

inline Objc3SemaTypedSemanticHandoffRecord
BuildObjc3SemaTypedSemanticHandoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaTypedSemanticHandoffRecord record;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.type_metadata_identity_handoffs_ready =
      surface.deterministic_interface_implementation_handoff &&
      surface.deterministic_protocol_category_composition_handoff &&
      surface.deterministic_class_protocol_category_linking_handoff &&
      surface.deterministic_selector_normalization_handoff;
  record.type_annotation_handoffs_ready =
      surface.deterministic_property_attribute_handoff &&
      surface.deterministic_type_annotation_surface_handoff &&
      surface.deterministic_lightweight_generic_constraint_handoff &&
      surface.deterministic_nullability_flow_warning_precision_handoff &&
      surface.deterministic_protocol_qualified_object_type_handoff &&
      surface.deterministic_variance_bridge_cast_handoff &&
      surface.deterministic_generic_metadata_abi_handoff;
  record.module_boundary_handoffs_ready =
      surface.deterministic_module_import_graph_handoff &&
      surface.deterministic_namespace_collision_shadowing_handoff &&
      surface.deterministic_public_private_api_partition_handoff &&
      surface.deterministic_incremental_module_cache_invalidation_handoff &&
      surface.deterministic_cross_module_conformance_handoff;
  record.concurrency_recovery_handoffs_ready =
      surface.deterministic_throws_propagation_handoff &&
      surface.deterministic_unwind_cleanup_handoff &&
      surface.deterministic_async_continuation_handoff &&
      surface.deterministic_actor_isolation_sendability_handoff &&
      surface.deterministic_task_runtime_cancellation_handoff &&
      surface.deterministic_concurrency_replay_race_guard_handoff &&
      surface.deterministic_unsafe_pointer_extension_handoff &&
      surface.deterministic_inline_asm_intrinsic_governance_handoff &&
      surface.deterministic_ns_error_bridging_handoff &&
      surface.deterministic_error_diagnostics_recovery_handoff &&
      surface.deterministic_result_like_lowering_handoff &&
      surface.deterministic_await_lowering_suspension_state_lowering_handoff;
  record.symbol_dispatch_handoffs_ready =
      surface.deterministic_symbol_graph_scope_resolution_handoff &&
      surface.deterministic_method_lookup_override_conflict_handoff &&
      surface.deterministic_property_synthesis_ivar_binding_handoff &&
      surface.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      surface.deterministic_message_send_selector_lowering_handoff &&
      surface.deterministic_dispatch_abi_marshalling_handoff &&
      surface.deterministic_nil_receiver_semantics_foldability_handoff &&
      surface.deterministic_super_dispatch_method_family_handoff;
  record.block_dispatch_handoffs_ready =
      surface.deterministic_block_literal_capture_semantics_handoff &&
      surface.deterministic_block_abi_invoke_trampoline_handoff &&
      surface.deterministic_block_storage_escape_handoff &&
      surface.deterministic_block_copy_dispose_handoff &&
      surface.deterministic_block_determinism_perf_baseline_handoff;
  record.ownership_runtime_handoffs_ready =
      surface.deterministic_runtime_link_host_link_handoff &&
      surface.deterministic_retain_release_operation_handoff &&
      surface.deterministic_weak_unowned_semantics_handoff &&
      surface.deterministic_arc_diagnostics_fixit_handoff &&
      surface.deterministic_autoreleasepool_scope_handoff;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.typed_semantic_handoff_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.type_metadata_identity_handoffs_ready &&
      record.type_annotation_handoffs_ready &&
      record.module_boundary_handoffs_ready &&
      record.concurrency_recovery_handoffs_ready &&
      record.symbol_dispatch_handoffs_ready &&
      record.block_dispatch_handoffs_ready &&
      record.ownership_runtime_handoffs_ready;
  return record;
}

inline Objc3SemaTypeMetadataMappingReadinessRecord
BuildObjc3SemaTypeMetadataMappingReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaTypeMetadataMappingReadinessRecord record;
  record.integration_surface_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.globals_total = surface.globals_total;
  record.functions_total = surface.functions_total;
  record.interfaces_total = surface.interfaces_total;
  record.implementations_total = surface.implementations_total;
  record.type_metadata_global_entries = surface.type_metadata_global_entries;
  record.type_metadata_function_entries =
      surface.type_metadata_function_entries;
  record.type_metadata_interface_entries =
      surface.type_metadata_interface_entries;
  record.type_metadata_implementation_entries =
      surface.type_metadata_implementation_entries;
  record.type_metadata_publication_ready =
      surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          surface.type_metadata_publication_record);
  record.type_metadata_handoff_ready =
      surface.deterministic_type_metadata_handoff;
  record.cardinality_consistent =
      record.globals_total == record.type_metadata_global_entries &&
      record.functions_total == record.type_metadata_function_entries &&
      record.interfaces_total == record.type_metadata_interface_entries &&
      record.implementations_total ==
          record.type_metadata_implementation_entries;
  record.atomic_memory_order_mapping_ready =
      surface.deterministic_atomic_memory_order_mapping &&
      surface.atomic_memory_order_mapping.deterministic;
  record.vector_type_lowering_ready =
      surface.deterministic_vector_type_lowering &&
      surface.vector_type_lowering.deterministic;
  record.mapping_summaries_ready =
      record.atomic_memory_order_mapping_ready &&
      record.vector_type_lowering_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.type_metadata_mapping_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.type_metadata_publication_ready &&
      record.type_metadata_handoff_ready && record.cardinality_consistent &&
      record.atomic_memory_order_mapping_ready &&
      record.vector_type_lowering_ready && record.mapping_summaries_ready;
  return record;
}

inline bool Objc3ParserSemaSyncCountsReady(
    std::size_t expected_count,
    std::size_t required_count,
    std::size_t passed_count,
    std::size_t failed_count) {
  return required_count == expected_count && passed_count == required_count &&
         failed_count == 0u;
}

inline std::size_t Objc3SemaEvidenceCount(bool ready) {
  return ready ? 1u : 0u;
}

inline Objc3ParserSemaParityPublicationReadinessRecord
BuildObjc3ParserSemaParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffPublicationTransferRecord &transfer_record,
    bool deterministic_transfer_record) {
  Objc3ParserSemaParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
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
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 17u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

inline Objc3SemaCoreSemanticParityPublicationReadinessRecord
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
    bool deterministic_variance_bridge_cast_handoff) {
  Objc3SemaCoreSemanticParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.semantic_diagnostics_ready = deterministic_semantic_diagnostics;
  record.type_metadata_handoff_ready = deterministic_type_metadata_handoff;
  record.interface_implementation_ready =
      deterministic_interface_implementation_handoff &&
      surface.interfaces_total == surface.type_metadata_interface_entries &&
      surface.implementations_total ==
          surface.type_metadata_implementation_entries &&
      surface.interface_implementation_summary.resolved_interfaces ==
          surface.type_metadata_interface_entries &&
      surface.interface_implementation_summary.resolved_implementations ==
          surface.type_metadata_implementation_entries;
  record.protocol_category_composition_ready =
      deterministic_protocol_category_composition_handoff &&
      surface.protocol_category_composition_summary.protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.protocol_category_composition_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.protocol_category_composition_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites <=
          surface.protocol_category_composition_summary
              .total_composition_sites();
  record.class_protocol_category_linking_ready =
      deterministic_class_protocol_category_linking_handoff &&
      surface.class_protocol_category_linking_summary.declared_interfaces ==
          surface.interface_implementation_summary.declared_interfaces &&
      surface.class_protocol_category_linking_summary.resolved_interfaces ==
          surface.interface_implementation_summary.resolved_interfaces &&
      surface.class_protocol_category_linking_summary.declared_implementations ==
          surface.interface_implementation_summary.declared_implementations &&
      surface.class_protocol_category_linking_summary.resolved_implementations ==
          surface.interface_implementation_summary.resolved_implementations &&
      surface.class_protocol_category_linking_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .linked_implementation_symbols ==
          surface.linked_implementation_symbols_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites <=
          surface.class_protocol_category_linking_summary
              .total_composition_sites() &&
      surface.class_protocol_category_linking_summary.deterministic;
  record.selector_normalization_ready =
      deterministic_selector_normalization_handoff &&
      surface.selector_normalization_summary.methods_total ==
          surface.selector_normalization_methods_total &&
      surface.selector_normalization_summary.normalized_methods ==
          surface.selector_normalization_normalized_methods_total &&
      surface.selector_normalization_summary.selector_piece_entries ==
          surface.selector_normalization_piece_entries_total &&
      surface.selector_normalization_summary
              .selector_parameter_piece_entries ==
          surface.selector_normalization_parameter_piece_entries_total &&
      surface.selector_normalization_summary.selector_pieceless_methods ==
          surface.selector_normalization_pieceless_methods_total &&
      surface.selector_normalization_summary.selector_spelling_mismatches ==
          surface.selector_normalization_spelling_mismatches_total &&
      surface.selector_normalization_summary.selector_arity_mismatches ==
          surface.selector_normalization_arity_mismatches_total &&
      surface.selector_normalization_summary
              .selector_parameter_linkage_mismatches ==
          surface.selector_normalization_parameter_linkage_mismatches_total &&
      surface.selector_normalization_summary
              .selector_normalization_flag_mismatches ==
          surface.selector_normalization_flag_mismatches_total &&
      surface.selector_normalization_summary.selector_missing_keyword_pieces ==
          surface.selector_normalization_missing_keyword_pieces_total &&
      surface.selector_normalization_summary.normalized_methods <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary
              .selector_parameter_piece_entries <=
          surface.selector_normalization_summary.selector_piece_entries &&
      surface.selector_normalization_summary.contract_violations() <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary.deterministic;
  record.property_attribute_ready =
      deterministic_property_attribute_handoff &&
      surface.property_attribute_summary.properties_total ==
          surface.property_attribute_properties_total &&
      surface.property_attribute_summary.attribute_entries ==
          surface.property_attribute_entries_total &&
      surface.property_attribute_summary.readonly_modifiers ==
          surface.property_attribute_readonly_modifiers_total &&
      surface.property_attribute_summary.readwrite_modifiers ==
          surface.property_attribute_readwrite_modifiers_total &&
      surface.property_attribute_summary.atomic_modifiers ==
          surface.property_attribute_atomic_modifiers_total &&
      surface.property_attribute_summary.nonatomic_modifiers ==
          surface.property_attribute_nonatomic_modifiers_total &&
      surface.property_attribute_summary.copy_modifiers ==
          surface.property_attribute_copy_modifiers_total &&
      surface.property_attribute_summary.strong_modifiers ==
          surface.property_attribute_strong_modifiers_total &&
      surface.property_attribute_summary.weak_modifiers ==
          surface.property_attribute_weak_modifiers_total &&
      surface.property_attribute_summary.assign_modifiers ==
          surface.property_attribute_assign_modifiers_total &&
      surface.property_attribute_summary.getter_modifiers ==
          surface.property_attribute_getter_modifiers_total &&
      surface.property_attribute_summary.setter_modifiers ==
          surface.property_attribute_setter_modifiers_total &&
      surface.property_attribute_summary.invalid_attribute_entries ==
          surface.property_attribute_invalid_attribute_entries_total &&
      surface.property_attribute_summary.property_contract_violations ==
          surface.property_attribute_contract_violations_total &&
      surface.property_attribute_summary.getter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.setter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.deterministic;
  record.type_annotation_surface_ready =
      deterministic_type_annotation_surface_handoff &&
      surface.type_annotation_surface_summary.generic_suffix_sites ==
          surface.type_annotation_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary.pointer_declarator_sites ==
          surface.type_annotation_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary.nullability_suffix_sites ==
          surface.type_annotation_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary.ownership_qualifier_sites ==
          surface.type_annotation_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.object_pointer_type_sites ==
          surface.type_annotation_object_pointer_type_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
          surface.type_annotation_invalid_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_pointer_declarator_sites ==
          surface.type_annotation_invalid_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_nullability_suffix_sites ==
          surface.type_annotation_invalid_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites ==
          surface.type_annotation_invalid_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          surface.type_annotation_surface_summary.generic_suffix_sites &&
      surface.type_annotation_surface_summary
              .invalid_pointer_declarator_sites <=
          surface.type_annotation_surface_summary.pointer_declarator_sites &&
      surface.type_annotation_surface_summary
              .invalid_nullability_suffix_sites <=
          surface.type_annotation_surface_summary.nullability_suffix_sites &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites <=
          surface.type_annotation_surface_summary.ownership_qualifier_sites &&
      surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          surface.type_annotation_surface_summary.total_type_annotation_sites() &&
      surface.type_annotation_surface_summary.deterministic;
  record.lightweight_generic_constraint_ready =
      deterministic_lightweight_generic_constraint_handoff &&
      surface.lightweight_generic_constraint_summary.generic_constraint_sites ==
          surface.lightweight_generic_constraint_sites_total &&
      surface.lightweight_generic_constraint_summary.generic_suffix_sites ==
          surface.lightweight_generic_constraint_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.object_pointer_type_sites ==
          surface
              .lightweight_generic_constraint_object_pointer_type_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites ==
          surface
              .lightweight_generic_constraint_terminated_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.pointer_declarator_sites ==
          surface.lightweight_generic_constraint_pointer_declarator_sites_total &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites ==
          surface.lightweight_generic_constraint_normalized_sites_total &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites ==
          surface
              .lightweight_generic_constraint_contract_violation_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_suffix_sites &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.deterministic;
  record.nullability_flow_warning_precision_ready =
      deterministic_nullability_flow_warning_precision_handoff &&
      surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==
          surface.nullability_flow_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .object_pointer_type_sites ==
          surface.nullability_flow_object_pointer_type_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_nullability_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites ==
          surface.nullability_flow_nullable_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
          surface.nullability_flow_nonnull_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites ==
          surface.nullability_flow_normalized_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .contract_violation_sites ==
          surface.nullability_flow_contract_violation_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary.contract_violation_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites +
              surface.nullability_flow_warning_precision_summary
                  .nonnull_suffix_sites &&
      surface.nullability_flow_warning_precision_summary.deterministic;
  record.protocol_qualified_object_type_ready =
      deterministic_protocol_qualified_object_type_handoff &&
      surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites ==
          surface.protocol_qualified_object_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.object_pointer_type_sites ==
          surface
              .protocol_qualified_object_type_object_pointer_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_terminated_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.pointer_declarator_sites ==
          surface
              .protocol_qualified_object_type_pointer_declarator_sites_total &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_normalized_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites ==
          surface
              .protocol_qualified_object_type_contract_violation_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_composition_sites &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.deterministic;
  record.variance_bridge_cast_ready =
      deterministic_variance_bridge_cast_handoff &&
      surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==
          surface.variance_bridge_cast_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites ==
          surface.variance_bridge_cast_protocol_composition_sites_total &&
      surface.variance_bridge_cast_summary.ownership_qualifier_sites ==
          surface.variance_bridge_cast_ownership_qualifier_sites_total &&
      surface.variance_bridge_cast_summary.object_pointer_type_sites ==
          surface.variance_bridge_cast_object_pointer_type_sites_total &&
      surface.variance_bridge_cast_summary.pointer_declarator_sites ==
          surface.variance_bridge_cast_pointer_declarator_sites_total &&
      surface.variance_bridge_cast_summary.normalized_sites ==
          surface.variance_bridge_cast_normalized_sites_total &&
      surface.variance_bridge_cast_summary.contract_violation_sites ==
          surface.variance_bridge_cast_contract_violation_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.normalized_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.contract_violation_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.semantic_diagnostics_ready) +
      Objc3SemaEvidenceCount(record.type_metadata_handoff_ready) +
      Objc3SemaEvidenceCount(record.interface_implementation_ready) +
      Objc3SemaEvidenceCount(record.protocol_category_composition_ready) +
      Objc3SemaEvidenceCount(record.class_protocol_category_linking_ready) +
      Objc3SemaEvidenceCount(record.selector_normalization_ready) +
      Objc3SemaEvidenceCount(record.property_attribute_ready) +
      Objc3SemaEvidenceCount(record.type_annotation_surface_ready) +
      Objc3SemaEvidenceCount(record.lightweight_generic_constraint_ready) +
      Objc3SemaEvidenceCount(
          record.nullability_flow_warning_precision_ready) +
      Objc3SemaEvidenceCount(record.protocol_qualified_object_type_ready) +
      Objc3SemaEvidenceCount(record.variance_bridge_cast_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.core_semantic_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 12u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

inline Objc3SemaModuleSemanticParityPublicationReadinessRecord
BuildObjc3SemaModuleSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_generic_metadata_abi_handoff,
    bool deterministic_module_import_graph_handoff,
    bool deterministic_namespace_collision_shadowing_handoff,
    bool deterministic_public_private_api_partition_handoff,
    bool deterministic_incremental_module_cache_invalidation_handoff) {
  Objc3SemaModuleSemanticParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.generic_metadata_abi_ready =
      deterministic_generic_metadata_abi_handoff &&
      surface.generic_metadata_abi_summary.generic_metadata_abi_sites ==
          surface.generic_metadata_abi_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites ==
          surface.generic_metadata_abi_generic_suffix_sites_total &&
      surface.generic_metadata_abi_summary.protocol_composition_sites ==
          surface.generic_metadata_abi_protocol_composition_sites_total &&
      surface.generic_metadata_abi_summary.ownership_qualifier_sites ==
          surface.generic_metadata_abi_ownership_qualifier_sites_total &&
      surface.generic_metadata_abi_summary.object_pointer_type_sites ==
          surface.generic_metadata_abi_object_pointer_type_sites_total &&
      surface.generic_metadata_abi_summary.pointer_declarator_sites ==
          surface.generic_metadata_abi_pointer_declarator_sites_total &&
      surface.generic_metadata_abi_summary.normalized_sites ==
          surface.generic_metadata_abi_normalized_sites_total &&
      surface.generic_metadata_abi_summary.contract_violation_sites ==
          surface.generic_metadata_abi_contract_violation_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.protocol_composition_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.normalized_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.contract_violation_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.deterministic;
  record.module_import_graph_ready =
      deterministic_module_import_graph_handoff &&
      surface.module_import_graph_summary.module_import_graph_sites ==
          surface.module_import_graph_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites ==
          surface.module_import_graph_import_edge_candidate_sites_total &&
      surface.module_import_graph_summary.namespace_segment_sites ==
          surface.module_import_graph_namespace_segment_sites_total &&
      surface.module_import_graph_summary.object_pointer_type_sites ==
          surface.module_import_graph_object_pointer_type_sites_total &&
      surface.module_import_graph_summary.pointer_declarator_sites ==
          surface.module_import_graph_pointer_declarator_sites_total &&
      surface.module_import_graph_summary.normalized_sites ==
          surface.module_import_graph_normalized_sites_total &&
      surface.module_import_graph_summary.contract_violation_sites ==
          surface.module_import_graph_contract_violation_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.namespace_segment_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.normalized_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.contract_violation_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.deterministic;
  record.namespace_collision_shadowing_ready =
      deterministic_namespace_collision_shadowing_handoff &&
      surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites ==
          surface.namespace_collision_shadowing_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites ==
          surface
              .namespace_collision_shadowing_namespace_segment_sites_total &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites ==
          surface
              .namespace_collision_shadowing_import_edge_candidate_sites_total &&
      surface.namespace_collision_shadowing_summary.object_pointer_type_sites ==
          surface
              .namespace_collision_shadowing_object_pointer_type_sites_total &&
      surface.namespace_collision_shadowing_summary.pointer_declarator_sites ==
          surface
              .namespace_collision_shadowing_pointer_declarator_sites_total &&
      surface.namespace_collision_shadowing_summary.normalized_sites ==
          surface.namespace_collision_shadowing_normalized_sites_total &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites ==
          surface
              .namespace_collision_shadowing_contract_violation_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.normalized_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.deterministic;
  record.public_private_api_partition_ready =
      deterministic_public_private_api_partition_handoff &&
      surface.public_private_api_partition_summary
              .public_private_api_partition_sites ==
          surface.public_private_api_partition_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites ==
          surface.public_private_api_partition_namespace_segment_sites_total &&
      surface.public_private_api_partition_summary
              .import_edge_candidate_sites ==
          surface.public_private_api_partition_import_edge_candidate_sites_total &&
      surface.public_private_api_partition_summary.object_pointer_type_sites ==
          surface.public_private_api_partition_object_pointer_type_sites_total &&
      surface.public_private_api_partition_summary.pointer_declarator_sites ==
          surface.public_private_api_partition_pointer_declarator_sites_total &&
      surface.public_private_api_partition_summary.normalized_sites ==
          surface.public_private_api_partition_normalized_sites_total &&
      surface.public_private_api_partition_summary.contract_violation_sites ==
          surface.public_private_api_partition_contract_violation_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.import_edge_candidate_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.normalized_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.contract_violation_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.deterministic;
  record.incremental_module_cache_invalidation_ready =
      deterministic_incremental_module_cache_invalidation_handoff &&
      surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites ==
          surface.incremental_module_cache_invalidation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites ==
          surface
              .incremental_module_cache_invalidation_namespace_segment_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_import_edge_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .object_pointer_type_sites ==
          surface
              .incremental_module_cache_invalidation_object_pointer_type_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .pointer_declarator_sites ==
          surface
              .incremental_module_cache_invalidation_pointer_declarator_sites_total &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites ==
          surface.incremental_module_cache_invalidation_normalized_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites ==
          surface
              .incremental_module_cache_invalidation_contract_violation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.generic_metadata_abi_ready) +
      Objc3SemaEvidenceCount(record.module_import_graph_ready) +
      Objc3SemaEvidenceCount(record.namespace_collision_shadowing_ready) +
      Objc3SemaEvidenceCount(record.public_private_api_partition_ready) +
      Objc3SemaEvidenceCount(
          record.incremental_module_cache_invalidation_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_semantic_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 5u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

inline Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
BuildObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff) {
  Objc3SemaIntermoduleFlowParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.cross_module_conformance_ready =
      deterministic_cross_module_conformance_handoff &&
      surface.cross_module_conformance_summary.cross_module_conformance_sites ==
          surface.cross_module_conformance_sites_total &&
      surface.cross_module_conformance_summary.namespace_segment_sites ==
          surface.cross_module_conformance_namespace_segment_sites_total &&
      surface.cross_module_conformance_summary.import_edge_candidate_sites ==
          surface
              .cross_module_conformance_import_edge_candidate_sites_total &&
      surface.cross_module_conformance_summary.object_pointer_type_sites ==
          surface.cross_module_conformance_object_pointer_type_sites_total &&
      surface.cross_module_conformance_summary.pointer_declarator_sites ==
          surface.cross_module_conformance_pointer_declarator_sites_total &&
      surface.cross_module_conformance_summary.normalized_sites ==
          surface.cross_module_conformance_normalized_sites_total &&
      surface.cross_module_conformance_summary
              .cache_invalidation_candidate_sites ==
          surface
              .cross_module_conformance_cache_invalidation_candidate_sites_total &&
      surface.cross_module_conformance_summary.contract_violation_sites ==
          surface.cross_module_conformance_contract_violation_sites_total &&
      surface.cross_module_conformance_summary.namespace_segment_sites <=
          surface.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      surface.cross_module_conformance_summary.import_edge_candidate_sites <=
          surface.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      surface.cross_module_conformance_summary.normalized_sites <=
          surface.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      surface.cross_module_conformance_summary
              .cache_invalidation_candidate_sites <=
          surface.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      surface.cross_module_conformance_summary.normalized_sites +
              surface.cross_module_conformance_summary
                  .cache_invalidation_candidate_sites ==
          surface.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      surface.cross_module_conformance_summary.contract_violation_sites <=
          surface.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      surface.cross_module_conformance_summary.deterministic;
  record.throws_propagation_ready =
      deterministic_throws_propagation_handoff &&
      surface.throws_propagation_summary.throws_propagation_sites ==
          surface.throws_propagation_sites_total &&
      surface.throws_propagation_summary.namespace_segment_sites ==
          surface.throws_propagation_namespace_segment_sites_total &&
      surface.throws_propagation_summary.import_edge_candidate_sites ==
          surface.throws_propagation_import_edge_candidate_sites_total &&
      surface.throws_propagation_summary.object_pointer_type_sites ==
          surface.throws_propagation_object_pointer_type_sites_total &&
      surface.throws_propagation_summary.pointer_declarator_sites ==
          surface.throws_propagation_pointer_declarator_sites_total &&
      surface.throws_propagation_summary.normalized_sites ==
          surface.throws_propagation_normalized_sites_total &&
      surface.throws_propagation_summary.cache_invalidation_candidate_sites ==
          surface
              .throws_propagation_cache_invalidation_candidate_sites_total &&
      surface.throws_propagation_summary.contract_violation_sites ==
          surface.throws_propagation_contract_violation_sites_total &&
      surface.throws_propagation_summary.namespace_segment_sites <=
          surface.throws_propagation_summary.throws_propagation_sites &&
      surface.throws_propagation_summary.import_edge_candidate_sites <=
          surface.throws_propagation_summary.throws_propagation_sites &&
      surface.throws_propagation_summary.normalized_sites <=
          surface.throws_propagation_summary.throws_propagation_sites &&
      surface.throws_propagation_summary.cache_invalidation_candidate_sites <=
          surface.throws_propagation_summary.throws_propagation_sites &&
      surface.throws_propagation_summary.normalized_sites +
              surface.throws_propagation_summary
                  .cache_invalidation_candidate_sites ==
          surface.throws_propagation_summary.throws_propagation_sites &&
      surface.throws_propagation_summary.contract_violation_sites <=
          surface.throws_propagation_summary.throws_propagation_sites &&
      surface.throws_propagation_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.cross_module_conformance_ready) +
      Objc3SemaEvidenceCount(record.throws_propagation_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.intermodule_flow_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 2u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

inline Objc3SemaConcurrencyParityPublicationReadinessRecord
BuildObjc3SemaConcurrencyParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_actor_isolation_sendability_handoff,
    bool deterministic_task_runtime_cancellation_handoff,
    bool deterministic_concurrency_replay_race_guard_handoff) {
  Objc3SemaConcurrencyParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.actor_isolation_sendability_ready =
      deterministic_actor_isolation_sendability_handoff &&
      surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites ==
          surface.actor_isolation_sendability_sites_total &&
      surface.actor_isolation_sendability_summary.actor_isolation_decl_sites ==
          surface.actor_isolation_decl_sites_total &&
      surface.actor_isolation_sendability_summary.actor_hop_sites ==
          surface.actor_hop_sites_total &&
      surface.actor_isolation_sendability_summary.sendable_annotation_sites ==
          surface.sendable_annotation_sites_total &&
      surface.actor_isolation_sendability_summary.non_sendable_crossing_sites ==
          surface.non_sendable_crossing_sites_total &&
      surface.actor_isolation_sendability_summary.isolation_boundary_sites ==
          surface.actor_isolation_sendability_isolation_boundary_sites_total &&
      surface.actor_isolation_sendability_summary.normalized_sites ==
          surface.actor_isolation_sendability_normalized_sites_total &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites ==
          surface.actor_isolation_sendability_gate_blocked_sites_total &&
      surface.actor_isolation_sendability_summary.contract_violation_sites ==
          surface.actor_isolation_sendability_contract_violation_sites_total &&
      surface.actor_isolation_sendability_summary.actor_isolation_decl_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.actor_hop_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.sendable_annotation_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.non_sendable_crossing_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.isolation_boundary_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.normalized_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites <=
          surface.actor_isolation_sendability_summary
              .non_sendable_crossing_sites &&
      surface.actor_isolation_sendability_summary.contract_violation_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.normalized_sites +
              surface.actor_isolation_sendability_summary.gate_blocked_sites ==
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.deterministic;
  record.task_runtime_cancellation_ready =
      deterministic_task_runtime_cancellation_handoff &&
      surface.task_runtime_cancellation_summary.task_runtime_interop_sites ==
          surface.task_runtime_cancellation_sites_total &&
      surface.task_runtime_cancellation_summary.runtime_hook_sites ==
          surface.task_runtime_cancellation_runtime_hook_sites_total &&
      surface.task_runtime_cancellation_summary.cancellation_check_sites ==
          surface.task_runtime_cancellation_cancellation_check_sites_total &&
      surface.task_runtime_cancellation_summary.cancellation_handler_sites ==
          surface
              .task_runtime_cancellation_cancellation_handler_sites_total &&
      surface.task_runtime_cancellation_summary.suspension_point_sites ==
          surface.task_runtime_cancellation_suspension_point_sites_total &&
      surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites ==
          surface
              .task_runtime_cancellation_cancellation_propagation_sites_total &&
      surface.task_runtime_cancellation_summary.normalized_sites ==
          surface.task_runtime_cancellation_normalized_sites_total &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites ==
          surface.task_runtime_cancellation_gate_blocked_sites_total &&
      surface.task_runtime_cancellation_summary.contract_violation_sites ==
          surface.task_runtime_cancellation_contract_violation_sites_total &&
      surface.task_runtime_cancellation_summary.runtime_hook_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_check_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_handler_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.suspension_point_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_propagation_sites <=
          surface.task_runtime_cancellation_summary.cancellation_check_sites &&
      surface.task_runtime_cancellation_summary.cancellation_propagation_sites <=
          surface.task_runtime_cancellation_summary.cancellation_handler_sites &&
      surface.task_runtime_cancellation_summary.normalized_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites <=
          surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites &&
      surface.task_runtime_cancellation_summary.contract_violation_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.normalized_sites +
              surface.task_runtime_cancellation_summary.gate_blocked_sites ==
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.deterministic;
  record.concurrency_replay_race_guard_ready =
      deterministic_concurrency_replay_race_guard_handoff &&
      surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites ==
          surface.concurrency_replay_race_guard_sites_total &&
      surface.concurrency_replay_race_guard_summary.concurrency_replay_sites ==
          surface
              .concurrency_replay_race_guard_concurrency_replay_sites_total &&
      surface.concurrency_replay_race_guard_summary.replay_proof_sites ==
          surface.concurrency_replay_race_guard_replay_proof_sites_total &&
      surface.concurrency_replay_race_guard_summary.race_guard_sites ==
          surface.concurrency_replay_race_guard_race_guard_sites_total &&
      surface.concurrency_replay_race_guard_summary.task_handoff_sites ==
          surface.concurrency_replay_race_guard_task_handoff_sites_total &&
      surface.concurrency_replay_race_guard_summary.actor_isolation_sites ==
          surface.concurrency_replay_race_guard_actor_isolation_sites_total &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites ==
          surface
              .concurrency_replay_race_guard_deterministic_schedule_sites_total &&
      surface.concurrency_replay_race_guard_summary.guard_blocked_sites ==
          surface.concurrency_replay_race_guard_guard_blocked_sites_total &&
      surface.concurrency_replay_race_guard_summary.contract_violation_sites ==
          surface.concurrency_replay_race_guard_contract_violation_sites_total &&
      surface.concurrency_replay_race_guard_summary.concurrency_replay_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.replay_proof_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.race_guard_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.task_handoff_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.actor_isolation_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.guard_blocked_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.contract_violation_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites +
              surface.concurrency_replay_race_guard_summary
                  .guard_blocked_sites ==
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.actor_isolation_sendability_ready) +
      Objc3SemaEvidenceCount(record.task_runtime_cancellation_ready) +
      Objc3SemaEvidenceCount(record.concurrency_replay_race_guard_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.concurrency_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 3u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

inline Objc3SemaUnsafeErrorParityValidationReadinessRecord
BuildObjc3SemaUnsafeErrorParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unsafe_pointer_extension_handoff,
    bool deterministic_inline_asm_intrinsic_governance_handoff,
    bool deterministic_ns_error_bridging_handoff,
    bool deterministic_error_diagnostics_recovery_handoff,
    bool deterministic_result_like_lowering_handoff) {
  Objc3SemaUnsafeErrorParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.unsafe_pointer_extension_ready =
      deterministic_unsafe_pointer_extension_handoff &&
      surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites ==
          surface.unsafe_pointer_extension_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_keyword_sites ==
          surface.unsafe_pointer_extension_unsafe_keyword_sites_total &&
      surface.unsafe_pointer_extension_summary.pointer_arithmetic_sites ==
          surface.unsafe_pointer_extension_pointer_arithmetic_sites_total &&
      surface.unsafe_pointer_extension_summary.raw_pointer_type_sites ==
          surface.unsafe_pointer_extension_raw_pointer_type_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_operation_sites ==
          surface.unsafe_pointer_extension_unsafe_operation_sites_total &&
      surface.unsafe_pointer_extension_summary.normalized_sites ==
          surface.unsafe_pointer_extension_normalized_sites_total &&
      surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
          surface.unsafe_pointer_extension_gate_blocked_sites_total &&
      surface.unsafe_pointer_extension_summary.contract_violation_sites ==
          surface.unsafe_pointer_extension_contract_violation_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_keyword_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.pointer_arithmetic_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.raw_pointer_type_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.unsafe_operation_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.normalized_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.gate_blocked_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.contract_violation_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.normalized_sites +
              surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.deterministic;
  record.inline_asm_intrinsic_governance_ready =
      deterministic_inline_asm_intrinsic_governance_handoff &&
      surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites ==
          surface.inline_asm_intrinsic_governance_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
          surface.inline_asm_intrinsic_governance_inline_asm_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
          surface.inline_asm_intrinsic_governance_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites ==
          surface
              .inline_asm_intrinsic_governance_governed_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites ==
          surface
              .inline_asm_intrinsic_governance_privileged_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites ==
          surface.inline_asm_intrinsic_governance_normalized_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_gate_blocked_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites ==
          surface
              .inline_asm_intrinsic_governance_contract_violation_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary.intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
          surface.throws_propagation_summary.cache_invalidation_candidate_sites &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
          surface.unsafe_pointer_extension_summary.unsafe_operation_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          surface.throws_propagation_summary.normalized_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          surface.unsafe_pointer_extension_summary.normalized_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites >=
          surface.inline_asm_intrinsic_governance_summary.inline_asm_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites -
              surface.inline_asm_intrinsic_governance_summary
                  .inline_asm_sites ==
          surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_summary.intrinsic_sites -
              surface.inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites +
              surface.inline_asm_intrinsic_governance_summary
                  .gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.deterministic;
  record.ns_error_bridging_ready =
      deterministic_ns_error_bridging_handoff &&
      surface.ns_error_bridging_summary.ns_error_bridging_sites ==
          surface.ns_error_bridging_sites_total &&
      surface.ns_error_bridging_summary.ns_error_parameter_sites ==
          surface.ns_error_bridging_ns_error_parameter_sites_total &&
      surface.ns_error_bridging_summary.ns_error_out_parameter_sites ==
          surface.ns_error_bridging_ns_error_out_parameter_sites_total &&
      surface.ns_error_bridging_summary.ns_error_bridge_path_sites ==
          surface.ns_error_bridging_ns_error_bridge_path_sites_total &&
      surface.ns_error_bridging_summary.failable_call_sites ==
          surface.ns_error_bridging_failable_call_sites_total &&
      surface.ns_error_bridging_summary.normalized_sites ==
          surface.ns_error_bridging_normalized_sites_total &&
      surface.ns_error_bridging_summary.bridge_boundary_sites ==
          surface.ns_error_bridging_bridge_boundary_sites_total &&
      surface.ns_error_bridging_summary.contract_violation_sites ==
          surface.ns_error_bridging_contract_violation_sites_total &&
      surface.ns_error_bridging_summary.ns_error_parameter_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.ns_error_out_parameter_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.ns_error_bridge_path_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.failable_call_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.normalized_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.bridge_boundary_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.normalized_sites +
              surface.ns_error_bridging_summary.bridge_boundary_sites ==
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.contract_violation_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.deterministic;
  record.error_diagnostics_recovery_ready =
      deterministic_error_diagnostics_recovery_handoff &&
      surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites ==
          surface.error_diagnostics_recovery_sites_total &&
      surface.error_diagnostics_recovery_summary.diagnostic_emit_sites ==
          surface.error_diagnostics_recovery_diagnostic_emit_sites_total &&
      surface.error_diagnostics_recovery_summary.recovery_anchor_sites ==
          surface.error_diagnostics_recovery_recovery_anchor_sites_total &&
      surface.error_diagnostics_recovery_summary.recovery_boundary_sites ==
          surface.error_diagnostics_recovery_recovery_boundary_sites_total &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites ==
          surface
              .error_diagnostics_recovery_fail_closed_diagnostic_sites_total &&
      surface.error_diagnostics_recovery_summary.normalized_sites ==
          surface.error_diagnostics_recovery_normalized_sites_total &&
      surface.error_diagnostics_recovery_summary.gate_blocked_sites ==
          surface.error_diagnostics_recovery_gate_blocked_sites_total &&
      surface.error_diagnostics_recovery_summary.contract_violation_sites ==
          surface.error_diagnostics_recovery_contract_violation_sites_total &&
      surface.error_diagnostics_recovery_summary.diagnostic_emit_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.recovery_anchor_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.recovery_boundary_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites <=
          surface.error_diagnostics_recovery_summary.diagnostic_emit_sites &&
      surface.error_diagnostics_recovery_summary.normalized_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.gate_blocked_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.contract_violation_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.normalized_sites +
              surface.error_diagnostics_recovery_summary.gate_blocked_sites ==
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.deterministic;
  record.result_like_lowering_ready =
      deterministic_result_like_lowering_handoff &&
      surface.result_like_lowering_summary.result_like_sites ==
          surface.result_like_lowering_sites_total &&
      surface.result_like_lowering_summary.result_success_sites ==
          surface.result_like_lowering_result_success_sites_total &&
      surface.result_like_lowering_summary.result_failure_sites ==
          surface.result_like_lowering_result_failure_sites_total &&
      surface.result_like_lowering_summary.result_branch_sites ==
          surface.result_like_lowering_result_branch_sites_total &&
      surface.result_like_lowering_summary.result_payload_sites ==
          surface.result_like_lowering_result_payload_sites_total &&
      surface.result_like_lowering_summary.normalized_sites ==
          surface.result_like_lowering_normalized_sites_total &&
      surface.result_like_lowering_summary.branch_merge_sites ==
          surface.result_like_lowering_branch_merge_sites_total &&
      surface.result_like_lowering_summary.contract_violation_sites ==
          surface.result_like_lowering_contract_violation_sites_total &&
      surface.result_like_lowering_summary.result_success_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_failure_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_branch_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_payload_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.normalized_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.branch_merge_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.contract_violation_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_success_sites +
              surface.result_like_lowering_summary.result_failure_sites ==
          surface.result_like_lowering_summary.normalized_sites &&
      surface.result_like_lowering_summary.normalized_sites +
              surface.result_like_lowering_summary.branch_merge_sites ==
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.unsafe_pointer_extension_ready) +
      Objc3SemaEvidenceCount(record.inline_asm_intrinsic_governance_ready) +
      Objc3SemaEvidenceCount(record.ns_error_bridging_ready) +
      Objc3SemaEvidenceCount(record.error_diagnostics_recovery_ready) +
      Objc3SemaEvidenceCount(record.result_like_lowering_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.unsafe_error_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 5u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

inline Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaConformanceEvidenceRecord record;
  const Objc3ParserSemaConformanceMatrix &matrix =
      surface.parser_sema_conformance_matrix;
  const Objc3ParserSemaConformanceCorpus &corpus =
      surface.parser_sema_conformance_corpus;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_matrix_deterministic =
      surface.deterministic_parser_sema_conformance_matrix &&
      matrix.deterministic;
  record.conformance_corpus_deterministic =
      surface.deterministic_parser_sema_conformance_corpus &&
      corpus.deterministic;
  record.declaration_count_evidence_ready =
      matrix.top_level_declaration_count_matches &&
      matrix.global_decl_count_matches && matrix.protocol_decl_count_matches &&
      matrix.interface_decl_count_matches &&
      matrix.implementation_decl_count_matches &&
      matrix.function_decl_count_matches;
  record.member_count_evidence_ready =
      matrix.protocol_property_decl_count_matches &&
      matrix.protocol_method_decl_count_matches &&
      matrix.protocol_class_method_decl_count_matches &&
      matrix.protocol_instance_method_decl_count_matches &&
      matrix.interface_property_decl_count_matches &&
      matrix.interface_method_decl_count_matches &&
      matrix.interface_class_method_decl_count_matches &&
      matrix.interface_instance_method_decl_count_matches &&
      matrix.implementation_property_decl_count_matches &&
      matrix.implementation_method_decl_count_matches &&
      matrix.implementation_class_method_decl_count_matches &&
      matrix.implementation_instance_method_decl_count_matches;
  record.category_function_evidence_ready =
      matrix.interface_category_decl_count_matches &&
      matrix.implementation_category_decl_count_matches &&
      matrix.function_prototype_count_matches &&
      matrix.function_pure_count_matches;
  record.fingerprint_evidence_ready =
      matrix.ast_shape_fingerprint_matches &&
      matrix.ast_top_level_layout_fingerprint_matches &&
      matrix.parser_contract_snapshot_fingerprint_matches;
  record.parser_budget_replay_evidence_ready =
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent &&
      matrix.parser_subset_count_consistent &&
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  record.passed_matrix_evidence_count =
      Objc3SemaEvidenceCount(matrix.top_level_declaration_count_matches) +
      Objc3SemaEvidenceCount(matrix.global_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.protocol_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.interface_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_category_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_category_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_prototype_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_pure_count_matches) +
      Objc3SemaEvidenceCount(matrix.ast_shape_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.ast_top_level_layout_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.parser_contract_snapshot_fingerprint_matches) +
      Objc3SemaEvidenceCount(matrix.parser_diagnostic_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_token_top_level_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_subset_count_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_contract_snapshot_deterministic) +
      Objc3SemaEvidenceCount(matrix.parser_recovery_replay_ready);
  record.required_corpus_case_count = corpus.required_case_count;
  record.passed_corpus_case_count = corpus.passed_case_count;
  record.failed_corpus_case_count = corpus.failed_case_count;
  record.corpus_inventory_ready =
      corpus.required_case_count == 5u &&
      corpus.has_top_level_declaration_count_case &&
      corpus.has_snapshot_fingerprint_case &&
      corpus.has_diagnostic_budget_case && corpus.has_subset_count_case &&
      corpus.has_recovery_replay_case;
  record.corpus_cases_passed =
      corpus.passed_case_count == corpus.required_case_count &&
      corpus.failed_case_count == 0u &&
      corpus.top_level_declaration_count_case_passed &&
      corpus.snapshot_fingerprint_case_passed &&
      corpus.diagnostic_budget_case_passed &&
      corpus.subset_count_case_passed && corpus.recovery_replay_case_passed;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_matrix_evidence_count == 30u &&
      record.passed_matrix_evidence_count ==
          record.required_matrix_evidence_count &&
      record.required_corpus_case_count == 5u &&
      record.passed_corpus_case_count == record.required_corpus_case_count &&
      record.failed_corpus_case_count == 0u &&
      record.conformance_matrix_deterministic &&
      record.conformance_corpus_deterministic &&
      record.declaration_count_evidence_ready &&
      record.member_count_evidence_ready &&
      record.category_function_evidence_ready &&
      record.fingerprint_evidence_ready &&
      record.parser_budget_replay_evidence_ready &&
      record.corpus_inventory_ready && record.corpus_cases_passed;
  return record;
}

inline Objc3ParserSemaContractReadinessRecord
BuildObjc3ParserSemaContractReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaContractReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_evidence_ready =
      surface.deterministic_parser_sema_conformance_evidence_record &&
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          surface.parser_sema_conformance_evidence_record);
  record.conformance_matrix_ready =
      record.conformance_evidence_ready &&
      surface.parser_sema_conformance_evidence_record
          .conformance_matrix_deterministic;
  record.conformance_corpus_ready =
      record.conformance_evidence_ready &&
      surface.parser_sema_conformance_evidence_record
          .conformance_corpus_deterministic;
  record.performance_quality_guardrails_ready =
      surface.deterministic_parser_sema_performance_quality_guardrails &&
      surface.parser_sema_performance_quality_guardrails.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          7u,
          surface.parser_sema_performance_quality_guardrails
              .required_guardrail_count,
          surface.parser_sema_performance_quality_guardrails
              .passed_guardrail_count,
          surface.parser_sema_performance_quality_guardrails
              .failed_guardrail_count) &&
      surface.parser_sema_performance_quality_guardrails
          .conformance_matrix_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .conformance_corpus_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .handoff_scaffold_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_diagnostic_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_token_top_level_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_subset_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .corpus_case_budget_consistent;
  record.cross_lane_integration_sync_ready =
      surface.deterministic_parser_sema_cross_lane_integration_sync &&
      surface.parser_sema_cross_lane_integration_sync.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          4u,
          surface.parser_sema_cross_lane_integration_sync.required_sync_count,
          surface.parser_sema_cross_lane_integration_sync.passed_sync_count,
          surface.parser_sema_cross_lane_integration_sync.failed_sync_count) &&
      surface.parser_sema_cross_lane_integration_sync.matrix_consistent &&
      surface.parser_sema_cross_lane_integration_sync.corpus_consistent &&
      surface.parser_sema_cross_lane_integration_sync
          .performance_quality_guardrails_consistent &&
      surface.parser_sema_cross_lane_integration_sync
          .pass_manager_contract_surface_sync;
  record.docs_runbook_sync_ready =
      surface.deterministic_parser_sema_docs_runbook_sync &&
      surface.parser_sema_docs_runbook_sync.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_docs_runbook_sync.required_sync_count,
          surface.parser_sema_docs_runbook_sync.passed_sync_count,
          surface.parser_sema_docs_runbook_sync.failed_sync_count) &&
      surface.parser_sema_docs_runbook_sync.cross_lane_integration_sync_ready &&
      surface.parser_sema_docs_runbook_sync.pass_manager_contract_surface_sync &&
      surface.parser_sema_docs_runbook_sync.parity_surface_sync;
  record.release_candidate_replay_dry_run_ready =
      surface.deterministic_parser_sema_release_candidate_replay_dry_run &&
      surface.parser_sema_release_candidate_replay_dry_run.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_release_candidate_replay_dry_run
              .required_sync_count,
          surface.parser_sema_release_candidate_replay_dry_run
              .passed_sync_count,
          surface.parser_sema_release_candidate_replay_dry_run
              .failed_sync_count) &&
      surface.parser_sema_release_candidate_replay_dry_run
          .docs_runbook_sync_ready &&
      surface.parser_sema_release_candidate_replay_dry_run
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_release_candidate_replay_dry_run.replay_surface_sync;
  record.advanced_core_shard1_ready =
      surface.deterministic_parser_sema_advanced_core_shard1 &&
      surface.parser_sema_advanced_core_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_core_shard1.required_sync_count,
          surface.parser_sema_advanced_core_shard1.passed_sync_count,
          surface.parser_sema_advanced_core_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_core_shard1
          .release_candidate_replay_dry_run_ready &&
      surface.parser_sema_advanced_core_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_core_shard1.shard_surface_sync;
  record.advanced_contract_rejection_shard1_ready =
      surface.deterministic_parser_sema_advanced_contract_rejection_shard1 &&
      surface.parser_sema_advanced_contract_rejection_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_contract_rejection_shard1
              .required_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard1
              .passed_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard1
              .failed_sync_count) &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .advanced_core_shard1_ready &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .shard_surface_sync;
  record.advanced_diagnostics_shard1_ready =
      surface.deterministic_parser_sema_advanced_diagnostics_shard1 &&
      surface.parser_sema_advanced_diagnostics_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_diagnostics_shard1.required_sync_count,
          surface.parser_sema_advanced_diagnostics_shard1.passed_sync_count,
          surface.parser_sema_advanced_diagnostics_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_diagnostics_shard1
          .advanced_contract_rejection_shard1_ready &&
      surface.parser_sema_advanced_diagnostics_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_diagnostics_shard1.shard_surface_sync;
  record.advanced_conformance_shard1_ready =
      surface.deterministic_parser_sema_advanced_conformance_shard1 &&
      surface.parser_sema_advanced_conformance_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_conformance_shard1.required_sync_count,
          surface.parser_sema_advanced_conformance_shard1.passed_sync_count,
          surface.parser_sema_advanced_conformance_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_conformance_shard1
          .advanced_diagnostics_shard1_ready &&
      surface.parser_sema_advanced_conformance_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_conformance_shard1.shard_surface_sync;
  record.advanced_integration_shard1_ready =
      surface.deterministic_parser_sema_advanced_integration_shard1 &&
      surface.parser_sema_advanced_integration_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_integration_shard1.required_sync_count,
          surface.parser_sema_advanced_integration_shard1.passed_sync_count,
          surface.parser_sema_advanced_integration_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_integration_shard1
          .advanced_conformance_shard1_ready &&
      surface.parser_sema_advanced_integration_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_integration_shard1.shard_surface_sync;
  record.advanced_performance_shard1_ready =
      surface.deterministic_parser_sema_advanced_performance_shard1 &&
      surface.parser_sema_advanced_performance_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_performance_shard1.required_sync_count,
          surface.parser_sema_advanced_performance_shard1.passed_sync_count,
          surface.parser_sema_advanced_performance_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_performance_shard1
          .advanced_integration_shard1_ready &&
      surface.parser_sema_advanced_performance_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_performance_shard1.shard_surface_sync;
  record.advanced_core_shard2_ready =
      surface.deterministic_parser_sema_advanced_core_shard2 &&
      surface.parser_sema_advanced_core_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_core_shard2.required_sync_count,
          surface.parser_sema_advanced_core_shard2.passed_sync_count,
          surface.parser_sema_advanced_core_shard2.failed_sync_count) &&
      surface.parser_sema_advanced_core_shard2
          .advanced_performance_shard1_ready &&
      surface.parser_sema_advanced_core_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_core_shard2.shard_surface_sync;
  record.advanced_contract_rejection_shard2_ready =
      surface.deterministic_parser_sema_advanced_contract_rejection_shard2 &&
      surface.parser_sema_advanced_contract_rejection_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_contract_rejection_shard2
              .required_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard2
              .passed_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard2
              .failed_sync_count) &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .advanced_core_shard2_ready &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .shard_surface_sync;
  record.advanced_diagnostics_shard2_ready =
      surface.deterministic_parser_sema_advanced_diagnostics_shard2 &&
      surface.parser_sema_advanced_diagnostics_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_diagnostics_shard2.required_sync_count,
          surface.parser_sema_advanced_diagnostics_shard2.passed_sync_count,
          surface.parser_sema_advanced_diagnostics_shard2.failed_sync_count) &&
      surface.parser_sema_advanced_diagnostics_shard2
          .advanced_contract_rejection_shard2_ready &&
      surface.parser_sema_advanced_diagnostics_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_diagnostics_shard2.shard_surface_sync;
  record.integration_closeout_ready =
      surface.deterministic_parser_sema_integration_closeout_signoff &&
      surface.parser_sema_integration_closeout_signoff.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_integration_closeout_signoff.required_sync_count,
          surface.parser_sema_integration_closeout_signoff.passed_sync_count,
          surface.parser_sema_integration_closeout_signoff.failed_sync_count) &&
      surface.parser_sema_integration_closeout_signoff
          .advanced_diagnostics_shard2_ready &&
      surface.parser_sema_integration_closeout_signoff
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_integration_closeout_signoff.gate_signoff_surface_sync;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
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
      record.integration_closeout_ready;
  return record;
}

inline bool IsReadyObjc3SemaParityContractSurface(const Objc3SemaParityContractSurface &surface) {
  return surface.ready &&
         surface.deterministic_parser_sema_contract_readiness_record &&
         IsReadyObjc3ParserSemaContractReadinessRecord(
             surface.parser_sema_contract_readiness_record) &&
         surface.deterministic_diagnostics_publication_record &&
         surface.deterministic_pass_manager_publication_record &&
         IsReadyObjc3SemaPassFlowSummary(surface.sema_pass_flow_summary) &&
         IsReadyObjc3SemaDiagnosticsPublicationRecord(
             surface.diagnostics_publication_record) &&
         surface.deterministic_pass_flow_recovery_record &&
         IsReadyObjc3SemaPassFlowRecoveryRecord(
             surface.pass_flow_recovery_record) &&
         IsReadyObjc3SemaPassManagerPublicationRecord(
             surface.pass_manager_publication_record) &&
         surface.deterministic_type_metadata_publication_record &&
         IsReadyObjc3SemaTypeMetadataPublicationRecord(
             surface.type_metadata_publication_record) &&
         surface.deterministic_type_metadata_mapping_readiness_record &&
         IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
             surface.type_metadata_mapping_readiness_record) &&
         surface.deterministic_typed_semantic_handoff_record &&
         IsReadyObjc3SemaTypedSemanticHandoffRecord(
             surface.typed_semantic_handoff_record) &&
         surface.deterministic_parity_validation_record &&
         IsReadyObjc3SemaParityValidationRecord(
             surface.parity_validation_record) &&
         surface.deterministic_closeout_signoff_record &&
         IsReadyObjc3SemaCloseoutSignoffRecord(
             surface.closeout_signoff_record) &&
         surface.deterministic_parser_sema_contract_readiness_record &&
         IsReadyObjc3ParserSemaContractReadinessRecord(
             surface.parser_sema_contract_readiness_record) &&
         IsReadyObjc3BootstrapLegalityFailureContractSummary(
             surface.bootstrap_legality_failure_contract_summary) &&
         IsReadyObjc3BootstrapLegalitySemanticsSummary(
             surface.bootstrap_legality_semantics_summary) &&
         IsReadyObjc3BootstrapFailureRestartSemanticsSummary(
             surface.bootstrap_failure_restart_semantics_summary) &&
         IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(
             surface.compatibility_strictness_claim_semantics_summary) &&
         surface.deterministic_parser_sema_contract_readiness_record &&
         IsReadyObjc3ParserSemaContractReadinessRecord(
             surface.parser_sema_contract_readiness_record) &&
         surface.deterministic_parser_sema_conformance_evidence_record &&
         IsReadyObjc3ParserSemaConformanceEvidenceRecord(
             surface.parser_sema_conformance_evidence_record) &&
         surface.diagnostics_accounting_consistent &&
         surface.diagnostics_bus_publish_consistent &&
         surface.diagnostics_canonicalized &&
         surface.diagnostics_hardening_satisfied &&
         surface.deterministic_pass_flow_recovery_record &&
         IsReadyObjc3SemaPassFlowRecoveryRecord(
             surface.pass_flow_recovery_record) &&
         surface.diagnostics_after_pass_monotonic && surface.deterministic_semantic_diagnostics &&
         surface.deterministic_type_metadata_mapping_readiness_record &&
         IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
             surface.type_metadata_mapping_readiness_record) &&
         surface.interface_implementation_summary.interface_method_symbols == surface.interface_method_symbols_total &&
         surface.interface_implementation_summary.implementation_method_symbols ==
             surface.implementation_method_symbols_total &&
         surface.interface_implementation_summary.linked_implementation_symbols ==
             surface.linked_implementation_symbols_total &&
         surface.interface_implementation_summary.deterministic &&
         surface.deterministic_interface_implementation_handoff &&
         surface.protocol_category_composition_summary.protocol_composition_sites ==
             surface.protocol_composition_sites_total &&
         surface.protocol_category_composition_summary.protocol_composition_symbols ==
             surface.protocol_composition_symbols_total &&
         surface.protocol_category_composition_summary.category_composition_sites ==
             surface.category_composition_sites_total &&
         surface.protocol_category_composition_summary.category_composition_symbols ==
             surface.category_composition_symbols_total &&
         surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
             surface.invalid_protocol_composition_sites_total &&
         surface.protocol_category_composition_summary.invalid_protocol_composition_sites <=
             surface.protocol_category_composition_summary.total_composition_sites() &&
         surface.protocol_category_composition_summary.deterministic &&
         surface.deterministic_protocol_category_composition_handoff &&
         surface.class_protocol_category_linking_summary.declared_interfaces ==
             surface.interface_implementation_summary.declared_interfaces &&
         surface.class_protocol_category_linking_summary.resolved_interfaces ==
             surface.interface_implementation_summary.resolved_interfaces &&
         surface.class_protocol_category_linking_summary.declared_implementations ==
             surface.interface_implementation_summary.declared_implementations &&
         surface.class_protocol_category_linking_summary.resolved_implementations ==
             surface.interface_implementation_summary.resolved_implementations &&
         surface.class_protocol_category_linking_summary.interface_method_symbols ==
             surface.interface_method_symbols_total &&
         surface.class_protocol_category_linking_summary.implementation_method_symbols ==
             surface.implementation_method_symbols_total &&
         surface.class_protocol_category_linking_summary.linked_implementation_symbols ==
             surface.linked_implementation_symbols_total &&
         surface.class_protocol_category_linking_summary.protocol_composition_sites ==
             surface.protocol_composition_sites_total &&
         surface.class_protocol_category_linking_summary.protocol_composition_symbols ==
             surface.protocol_composition_symbols_total &&
         surface.class_protocol_category_linking_summary.category_composition_sites ==
             surface.category_composition_sites_total &&
         surface.class_protocol_category_linking_summary.category_composition_symbols ==
             surface.category_composition_symbols_total &&
         surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
             surface.invalid_protocol_composition_sites_total &&
         surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
             surface.class_protocol_category_linking_summary.total_composition_sites() &&
         surface.class_protocol_category_linking_summary.deterministic &&
         surface.deterministic_class_protocol_category_linking_handoff &&
         surface.selector_normalization_summary.methods_total == surface.selector_normalization_methods_total &&
         surface.selector_normalization_summary.normalized_methods ==
             surface.selector_normalization_normalized_methods_total &&
         surface.selector_normalization_summary.selector_piece_entries ==
             surface.selector_normalization_piece_entries_total &&
         surface.selector_normalization_summary.selector_parameter_piece_entries ==
             surface.selector_normalization_parameter_piece_entries_total &&
         surface.selector_normalization_summary.selector_pieceless_methods ==
             surface.selector_normalization_pieceless_methods_total &&
         surface.selector_normalization_summary.selector_spelling_mismatches ==
             surface.selector_normalization_spelling_mismatches_total &&
         surface.selector_normalization_summary.selector_arity_mismatches ==
             surface.selector_normalization_arity_mismatches_total &&
         surface.selector_normalization_summary.selector_parameter_linkage_mismatches ==
             surface.selector_normalization_parameter_linkage_mismatches_total &&
         surface.selector_normalization_summary.selector_normalization_flag_mismatches ==
             surface.selector_normalization_flag_mismatches_total &&
         surface.selector_normalization_summary.selector_missing_keyword_pieces ==
             surface.selector_normalization_missing_keyword_pieces_total &&
         surface.selector_normalization_summary.selector_parameter_piece_entries <=
             surface.selector_normalization_summary.selector_piece_entries &&
         surface.selector_normalization_summary.normalized_methods <= surface.selector_normalization_summary.methods_total &&
         surface.selector_normalization_summary.contract_violations() <=
             surface.selector_normalization_summary.methods_total &&
         surface.selector_normalization_summary.deterministic &&
         surface.deterministic_selector_normalization_handoff &&
         surface.property_attribute_summary.properties_total == surface.property_attribute_properties_total &&
         surface.property_attribute_summary.attribute_entries == surface.property_attribute_entries_total &&
         surface.property_attribute_summary.readonly_modifiers == surface.property_attribute_readonly_modifiers_total &&
         surface.property_attribute_summary.readwrite_modifiers == surface.property_attribute_readwrite_modifiers_total &&
         surface.property_attribute_summary.atomic_modifiers == surface.property_attribute_atomic_modifiers_total &&
         surface.property_attribute_summary.nonatomic_modifiers == surface.property_attribute_nonatomic_modifiers_total &&
         surface.property_attribute_summary.copy_modifiers == surface.property_attribute_copy_modifiers_total &&
         surface.property_attribute_summary.strong_modifiers == surface.property_attribute_strong_modifiers_total &&
         surface.property_attribute_summary.weak_modifiers == surface.property_attribute_weak_modifiers_total &&
         surface.property_attribute_summary.assign_modifiers == surface.property_attribute_assign_modifiers_total &&
         surface.property_attribute_summary.getter_modifiers == surface.property_attribute_getter_modifiers_total &&
         surface.property_attribute_summary.setter_modifiers == surface.property_attribute_setter_modifiers_total &&
         surface.property_attribute_summary.invalid_attribute_entries ==
             surface.property_attribute_invalid_attribute_entries_total &&
         surface.property_attribute_summary.property_contract_violations ==
             surface.property_attribute_contract_violations_total &&
         surface.property_attribute_summary.getter_modifiers <= surface.property_attribute_summary.properties_total &&
         surface.property_attribute_summary.setter_modifiers <= surface.property_attribute_summary.properties_total &&
         surface.property_attribute_summary.deterministic &&
         surface.deterministic_property_attribute_handoff &&
         surface.type_annotation_surface_summary.generic_suffix_sites == surface.type_annotation_generic_suffix_sites_total &&
         surface.type_annotation_surface_summary.pointer_declarator_sites ==
             surface.type_annotation_pointer_declarator_sites_total &&
         surface.type_annotation_surface_summary.nullability_suffix_sites ==
             surface.type_annotation_nullability_suffix_sites_total &&
         surface.type_annotation_surface_summary.ownership_qualifier_sites ==
             surface.type_annotation_ownership_qualifier_sites_total &&
         surface.type_annotation_surface_summary.object_pointer_type_sites ==
             surface.type_annotation_object_pointer_type_sites_total &&
         surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
             surface.type_annotation_invalid_generic_suffix_sites_total &&
         surface.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
             surface.type_annotation_invalid_pointer_declarator_sites_total &&
         surface.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
             surface.type_annotation_invalid_nullability_suffix_sites_total &&
         surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==
             surface.type_annotation_invalid_ownership_qualifier_sites_total &&
         surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
             surface.type_annotation_surface_summary.generic_suffix_sites &&
         surface.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
             surface.type_annotation_surface_summary.pointer_declarator_sites &&
         surface.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
             surface.type_annotation_surface_summary.nullability_suffix_sites &&
         surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
             surface.type_annotation_surface_summary.ownership_qualifier_sites &&
         surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
             surface.type_annotation_surface_summary.total_type_annotation_sites() &&
         surface.type_annotation_surface_summary.deterministic &&
         surface.deterministic_type_annotation_surface_handoff &&
         surface.lightweight_generic_constraint_summary.generic_constraint_sites ==
             surface.lightweight_generic_constraint_sites_total &&
         surface.lightweight_generic_constraint_summary.generic_suffix_sites ==
             surface.lightweight_generic_constraint_generic_suffix_sites_total &&
         surface.lightweight_generic_constraint_summary.object_pointer_type_sites ==
             surface.lightweight_generic_constraint_object_pointer_type_sites_total &&
         surface.lightweight_generic_constraint_summary.terminated_generic_suffix_sites ==
             surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total &&
         surface.lightweight_generic_constraint_summary.pointer_declarator_sites ==
             surface.lightweight_generic_constraint_pointer_declarator_sites_total &&
         surface.lightweight_generic_constraint_summary.normalized_constraint_sites ==
             surface.lightweight_generic_constraint_normalized_sites_total &&
         surface.lightweight_generic_constraint_summary.contract_violation_sites ==
             surface.lightweight_generic_constraint_contract_violation_sites_total &&
         surface.lightweight_generic_constraint_summary.terminated_generic_suffix_sites <=
             surface.lightweight_generic_constraint_summary.generic_suffix_sites &&
         surface.lightweight_generic_constraint_summary.normalized_constraint_sites <=
             surface.lightweight_generic_constraint_summary.generic_constraint_sites &&
         surface.lightweight_generic_constraint_summary.contract_violation_sites <=
             surface.lightweight_generic_constraint_summary.generic_constraint_sites &&
         surface.lightweight_generic_constraint_summary.deterministic &&
         surface.deterministic_lightweight_generic_constraint_handoff &&
         surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==
             surface.nullability_flow_sites_total &&
         surface.nullability_flow_warning_precision_summary.object_pointer_type_sites ==
             surface.nullability_flow_object_pointer_type_sites_total &&
         surface.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
             surface.nullability_flow_nullability_suffix_sites_total &&
         surface.nullability_flow_warning_precision_summary.nullable_suffix_sites ==
             surface.nullability_flow_nullable_suffix_sites_total &&
         surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
             surface.nullability_flow_nonnull_suffix_sites_total &&
         surface.nullability_flow_warning_precision_summary.normalized_sites ==
             surface.nullability_flow_normalized_sites_total &&
         surface.nullability_flow_warning_precision_summary.contract_violation_sites ==
             surface.nullability_flow_contract_violation_sites_total &&
         surface.nullability_flow_warning_precision_summary.normalized_sites <=
             surface.nullability_flow_warning_precision_summary.nullability_flow_sites &&
         surface.nullability_flow_warning_precision_summary.contract_violation_sites <=
             surface.nullability_flow_warning_precision_summary.nullability_flow_sites &&
         surface.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
             surface.nullability_flow_warning_precision_summary.nullable_suffix_sites +
                 surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites &&
         surface.nullability_flow_warning_precision_summary.deterministic &&
         surface.deterministic_nullability_flow_warning_precision_handoff &&
         surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites ==
             surface.protocol_qualified_object_type_sites_total &&
         surface.protocol_qualified_object_type_summary.protocol_composition_sites ==
             surface.protocol_qualified_object_type_protocol_composition_sites_total &&
         surface.protocol_qualified_object_type_summary.object_pointer_type_sites ==
             surface.protocol_qualified_object_type_object_pointer_type_sites_total &&
         surface.protocol_qualified_object_type_summary.terminated_protocol_composition_sites ==
             surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total &&
         surface.protocol_qualified_object_type_summary.pointer_declarator_sites ==
