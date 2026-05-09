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
inline constexpr const char *kObjc3SemaDiagnosticHandoffOwner =
    "native.frontend.sema.diagnostic-stage";
inline constexpr const char *kObjc3SemaPassManagerPublicationOwner =
    "native.frontend.sema.pass-manager-publication";
inline constexpr const char *kObjc3SemaTypeMetadataPublicationOwner =
    "native.frontend.sema.type-metadata-publication";
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
         record.diagnostics_publication_ready && record.deterministic;
}

inline Objc3SemaPassManagerPublicationRecord
BuildObjc3SemaPassManagerPublicationRecord(
    const Objc3ParserSemaHandoffOwnerRecord &parser_owner_record,
    const Objc3SemaPassFlowSummary &pass_flow_summary,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff) {
  Objc3SemaPassManagerPublicationRecord record;
  record.parser_sema_handoff_owner_record = parser_owner_record;
  record.pass_flow_owner = pass_flow_summary.stage_input_owner;
  record.diagnostic_handoff_owner = pass_flow_summary.diagnostic_handoff_owner;
  record.owner_model = pass_flow_summary.owner_model;
  record.strict_no_fallback = pass_flow_summary.strict_no_fallback;
  record.strict_no_compatibility = pass_flow_summary.strict_no_compatibility;
  record.parser_sema_handoff_owner_ready =
      IsReadyObjc3ParserSemaHandoffOwnerRecord(parser_owner_record);
  record.pass_flow_summary_ready =
      IsReadyObjc3SemaPassFlowSummary(pass_flow_summary);
  record.semantic_diagnostics_ready = deterministic_semantic_diagnostics;
  record.type_metadata_handoff_ready = deterministic_type_metadata_handoff;
  record.diagnostics_publication_ready =
      pass_flow_summary.diagnostics_hardening_satisfied &&
      pass_flow_summary.diagnostics_bus_publish_consistent &&
      pass_flow_summary.diagnostics_canonicalized;
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
      record.diagnostics_publication_ready;
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

struct Objc3SemaParityContractSurface {
  Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;
  Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;
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
  Objc3SemaPassFlowSummary sema_pass_flow_summary;
  Objc3SemaPassManagerPublicationRecord pass_manager_publication_record;
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
  bool deterministic_pass_manager_publication_record = false;
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

inline bool IsReadyObjc3SemaParityContractSurface(const Objc3SemaParityContractSurface &surface) {
  return surface.ready && surface.deterministic_parser_sema_conformance_matrix &&
         surface.deterministic_parser_sema_conformance_corpus &&
         surface.deterministic_parser_sema_performance_quality_guardrails &&
         surface.deterministic_parser_sema_cross_lane_integration_sync &&
         surface.deterministic_parser_sema_docs_runbook_sync &&
         surface.deterministic_parser_sema_release_candidate_replay_dry_run &&
         surface.deterministic_parser_sema_advanced_core_shard1 &&
         surface.deterministic_parser_sema_advanced_contract_rejection_shard1 &&
         surface.deterministic_parser_sema_advanced_diagnostics_shard1 &&
         surface.deterministic_parser_sema_advanced_conformance_shard1 &&
         surface.deterministic_parser_sema_advanced_integration_shard1 &&
         surface.deterministic_parser_sema_advanced_performance_shard1 &&
         surface.deterministic_parser_sema_advanced_core_shard2 &&
         surface.deterministic_parser_sema_advanced_contract_rejection_shard2 &&
         surface.deterministic_parser_sema_advanced_diagnostics_shard2 &&
         surface.deterministic_parser_sema_integration_closeout_signoff &&
         surface.deterministic_pass_manager_publication_record &&
         IsReadyObjc3SemaPassFlowSummary(surface.sema_pass_flow_summary) &&
         IsReadyObjc3SemaPassManagerPublicationRecord(
             surface.pass_manager_publication_record) &&
         surface.parser_sema_conformance_matrix.deterministic &&
         surface.parser_sema_conformance_corpus.deterministic &&
         surface.parser_sema_performance_quality_guardrails.deterministic &&
         surface.parser_sema_cross_lane_integration_sync.deterministic &&
         surface.parser_sema_docs_runbook_sync.deterministic &&
         surface.parser_sema_release_candidate_replay_dry_run.deterministic &&
         surface.parser_sema_advanced_core_shard1.deterministic &&
         surface.parser_sema_advanced_contract_rejection_shard1.deterministic &&
         surface.parser_sema_advanced_diagnostics_shard1.deterministic &&
         surface.parser_sema_advanced_conformance_shard1.deterministic &&
         surface.parser_sema_advanced_integration_shard1.deterministic &&
         surface.parser_sema_advanced_performance_shard1.deterministic &&
         surface.parser_sema_advanced_core_shard2.deterministic &&
         surface.parser_sema_advanced_contract_rejection_shard2.deterministic &&
         surface.parser_sema_advanced_diagnostics_shard2.deterministic &&
         surface.parser_sema_integration_closeout_signoff.deterministic &&
         IsReadyObjc3BootstrapLegalityFailureContractSummary(
             surface.bootstrap_legality_failure_contract_summary) &&
         IsReadyObjc3BootstrapLegalitySemanticsSummary(
             surface.bootstrap_legality_semantics_summary) &&
         IsReadyObjc3BootstrapFailureRestartSemanticsSummary(
             surface.bootstrap_failure_restart_semantics_summary) &&
         IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(
             surface.compatibility_strictness_claim_semantics_summary) &&
         surface.parser_sema_performance_quality_guardrails.required_guardrail_count == 7u &&
         surface.parser_sema_performance_quality_guardrails.passed_guardrail_count ==
             surface.parser_sema_performance_quality_guardrails.required_guardrail_count &&
         surface.parser_sema_performance_quality_guardrails.failed_guardrail_count == 0u &&
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
         surface.parser_sema_performance_quality_guardrails.matrix_subset_budget_consistent &&
         surface.parser_sema_performance_quality_guardrails.corpus_case_budget_consistent &&
         surface.parser_sema_cross_lane_integration_sync.required_sync_count == 4u &&
         surface.parser_sema_cross_lane_integration_sync.passed_sync_count ==
             surface.parser_sema_cross_lane_integration_sync.required_sync_count &&
         surface.parser_sema_cross_lane_integration_sync.failed_sync_count == 0u &&
         surface.parser_sema_cross_lane_integration_sync.matrix_consistent &&
         surface.parser_sema_cross_lane_integration_sync.corpus_consistent &&
         surface.parser_sema_cross_lane_integration_sync
             .performance_quality_guardrails_consistent &&
         surface.parser_sema_cross_lane_integration_sync
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_docs_runbook_sync.required_sync_count == 3u &&
         surface.parser_sema_docs_runbook_sync.passed_sync_count ==
             surface.parser_sema_docs_runbook_sync.required_sync_count &&
         surface.parser_sema_docs_runbook_sync.failed_sync_count == 0u &&
         surface.parser_sema_docs_runbook_sync.cross_lane_integration_sync_ready &&
         surface.parser_sema_docs_runbook_sync
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_docs_runbook_sync.parity_surface_sync &&
         surface.parser_sema_release_candidate_replay_dry_run.required_sync_count ==
             3u &&
         surface.parser_sema_release_candidate_replay_dry_run.passed_sync_count ==
             surface.parser_sema_release_candidate_replay_dry_run
                 .required_sync_count &&
         surface.parser_sema_release_candidate_replay_dry_run.failed_sync_count ==
             0u &&
         surface.parser_sema_release_candidate_replay_dry_run
             .docs_runbook_sync_ready &&
         surface.parser_sema_release_candidate_replay_dry_run
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_release_candidate_replay_dry_run.replay_surface_sync &&
         surface.parser_sema_advanced_core_shard1.required_sync_count == 3u &&
         surface.parser_sema_advanced_core_shard1.passed_sync_count ==
             surface.parser_sema_advanced_core_shard1.required_sync_count &&
         surface.parser_sema_advanced_core_shard1.failed_sync_count == 0u &&
         surface.parser_sema_advanced_core_shard1
             .release_candidate_replay_dry_run_ready &&
         surface.parser_sema_advanced_core_shard1
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_core_shard1.shard_surface_sync &&
         surface.parser_sema_advanced_contract_rejection_shard1.required_sync_count == 3u &&
         surface.parser_sema_advanced_contract_rejection_shard1
             .passed_sync_count ==
             surface.parser_sema_advanced_contract_rejection_shard1
                 .required_sync_count &&
         surface.parser_sema_advanced_contract_rejection_shard1
             .failed_sync_count == 0u &&
         surface.parser_sema_advanced_contract_rejection_shard1
             .advanced_core_shard1_ready &&
         surface.parser_sema_advanced_contract_rejection_shard1
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_contract_rejection_shard1
             .shard_surface_sync &&
         surface.parser_sema_advanced_diagnostics_shard1.required_sync_count == 3u &&
         surface.parser_sema_advanced_diagnostics_shard1.passed_sync_count ==
             surface.parser_sema_advanced_diagnostics_shard1.required_sync_count &&
         surface.parser_sema_advanced_diagnostics_shard1.failed_sync_count == 0u &&
         surface.parser_sema_advanced_diagnostics_shard1
             .advanced_contract_rejection_shard1_ready &&
         surface.parser_sema_advanced_diagnostics_shard1
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_diagnostics_shard1
             .shard_surface_sync &&
         surface.parser_sema_advanced_conformance_shard1.required_sync_count == 3u &&
         surface.parser_sema_advanced_conformance_shard1.passed_sync_count ==
             surface.parser_sema_advanced_conformance_shard1.required_sync_count &&
         surface.parser_sema_advanced_conformance_shard1.failed_sync_count == 0u &&
         surface.parser_sema_advanced_conformance_shard1
             .advanced_diagnostics_shard1_ready &&
         surface.parser_sema_advanced_conformance_shard1
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_conformance_shard1
             .shard_surface_sync &&
         surface.parser_sema_advanced_integration_shard1.required_sync_count == 3u &&
         surface.parser_sema_advanced_integration_shard1.passed_sync_count ==
             surface.parser_sema_advanced_integration_shard1.required_sync_count &&
         surface.parser_sema_advanced_integration_shard1.failed_sync_count == 0u &&
         surface.parser_sema_advanced_integration_shard1
             .advanced_conformance_shard1_ready &&
         surface.parser_sema_advanced_integration_shard1
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_integration_shard1
             .shard_surface_sync &&
         surface.parser_sema_advanced_performance_shard1.required_sync_count == 3u &&
         surface.parser_sema_advanced_performance_shard1.passed_sync_count ==
             surface.parser_sema_advanced_performance_shard1.required_sync_count &&
         surface.parser_sema_advanced_performance_shard1.failed_sync_count == 0u &&
         surface.parser_sema_advanced_performance_shard1
             .advanced_integration_shard1_ready &&
         surface.parser_sema_advanced_performance_shard1
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_performance_shard1
             .shard_surface_sync &&
         surface.parser_sema_advanced_core_shard2.required_sync_count == 3u &&
         surface.parser_sema_advanced_core_shard2.passed_sync_count ==
             surface.parser_sema_advanced_core_shard2.required_sync_count &&
         surface.parser_sema_advanced_core_shard2.failed_sync_count == 0u &&
         surface.parser_sema_advanced_core_shard2
             .advanced_performance_shard1_ready &&
         surface.parser_sema_advanced_core_shard2
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_core_shard2
             .shard_surface_sync &&
         surface.parser_sema_advanced_contract_rejection_shard2.required_sync_count == 3u &&
         surface.parser_sema_advanced_contract_rejection_shard2.passed_sync_count ==
             surface.parser_sema_advanced_contract_rejection_shard2.required_sync_count &&
         surface.parser_sema_advanced_contract_rejection_shard2.failed_sync_count == 0u &&
         surface.parser_sema_advanced_contract_rejection_shard2
             .advanced_core_shard2_ready &&
         surface.parser_sema_advanced_contract_rejection_shard2
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_contract_rejection_shard2
             .shard_surface_sync &&
         surface.parser_sema_advanced_diagnostics_shard2.required_sync_count == 3u &&
         surface.parser_sema_advanced_diagnostics_shard2.passed_sync_count ==
             surface.parser_sema_advanced_diagnostics_shard2.required_sync_count &&
         surface.parser_sema_advanced_diagnostics_shard2.failed_sync_count == 0u &&
         surface.parser_sema_advanced_diagnostics_shard2
             .advanced_contract_rejection_shard2_ready &&
         surface.parser_sema_advanced_diagnostics_shard2
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_advanced_diagnostics_shard2
             .shard_surface_sync &&
         surface.parser_sema_integration_closeout_signoff.required_sync_count == 3u &&
         surface.parser_sema_integration_closeout_signoff.passed_sync_count ==
             surface.parser_sema_integration_closeout_signoff.required_sync_count &&
         surface.parser_sema_integration_closeout_signoff.failed_sync_count == 0u &&
         surface.parser_sema_integration_closeout_signoff
             .advanced_diagnostics_shard2_ready &&
         surface.parser_sema_integration_closeout_signoff
             .pass_manager_contract_surface_sync &&
         surface.parser_sema_integration_closeout_signoff
             .gate_signoff_surface_sync &&
         surface.parser_sema_conformance_matrix.top_level_declaration_count_matches &&
         surface.parser_sema_conformance_matrix.global_decl_count_matches &&
         surface.parser_sema_conformance_matrix.protocol_decl_count_matches &&
         surface.parser_sema_conformance_matrix.interface_decl_count_matches &&
         surface.parser_sema_conformance_matrix.implementation_decl_count_matches &&
         surface.parser_sema_conformance_matrix.function_decl_count_matches &&
         surface.parser_sema_conformance_matrix.protocol_property_decl_count_matches &&
         surface.parser_sema_conformance_matrix.protocol_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix
             .protocol_class_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix
             .protocol_instance_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix.interface_property_decl_count_matches &&
         surface.parser_sema_conformance_matrix.interface_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix
             .interface_class_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix
             .interface_instance_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix.implementation_property_decl_count_matches &&
         surface.parser_sema_conformance_matrix.implementation_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix
             .implementation_class_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix
             .implementation_instance_method_decl_count_matches &&
         surface.parser_sema_conformance_matrix.interface_category_decl_count_matches &&
         surface.parser_sema_conformance_matrix.implementation_category_decl_count_matches &&
         surface.parser_sema_conformance_matrix.function_prototype_count_matches &&
         surface.parser_sema_conformance_matrix.function_pure_count_matches &&
         surface.parser_sema_conformance_matrix.ast_shape_fingerprint_matches &&
         surface.parser_sema_conformance_matrix.ast_top_level_layout_fingerprint_matches &&
         surface.parser_sema_conformance_matrix.parser_contract_snapshot_fingerprint_matches &&
         surface.parser_sema_conformance_matrix.parser_diagnostic_budget_consistent &&
         surface.parser_sema_conformance_matrix.parser_token_top_level_budget_consistent &&
         surface.parser_sema_conformance_matrix.parser_subset_count_consistent &&
         surface.parser_sema_conformance_matrix.parser_contract_snapshot_deterministic &&
         surface.parser_sema_conformance_matrix.parser_recovery_replay_ready &&
         surface.parser_sema_conformance_corpus.required_case_count == 5u &&
         surface.parser_sema_conformance_corpus.passed_case_count ==
             surface.parser_sema_conformance_corpus.required_case_count &&
         surface.parser_sema_conformance_corpus.failed_case_count == 0u &&
         surface.parser_sema_conformance_corpus.has_top_level_declaration_count_case &&
         surface.parser_sema_conformance_corpus.has_snapshot_fingerprint_case &&
         surface.parser_sema_conformance_corpus.has_diagnostic_budget_case &&
         surface.parser_sema_conformance_corpus.has_subset_count_case &&
         surface.parser_sema_conformance_corpus.has_recovery_replay_case &&
         surface.parser_sema_conformance_corpus
             .top_level_declaration_count_case_passed &&
         surface.parser_sema_conformance_corpus.snapshot_fingerprint_case_passed &&
         surface.parser_sema_conformance_corpus.diagnostic_budget_case_passed &&
         surface.parser_sema_conformance_corpus.subset_count_case_passed &&
         surface.parser_sema_conformance_corpus.recovery_replay_case_passed &&
         surface.diagnostics_accounting_consistent &&
         surface.diagnostics_bus_publish_consistent &&
         surface.diagnostics_canonicalized &&
         surface.diagnostics_hardening_satisfied &&
         surface.pass_flow_recovery_replay_contract_satisfied &&
         !surface.pass_flow_recovery_replay_key.empty() &&
         surface.pass_flow_recovery_replay_key_deterministic &&
         surface.pass_flow_recovery_determinism_hardening_satisfied &&
         surface.diagnostics_after_pass_monotonic && surface.deterministic_semantic_diagnostics &&
         surface.deterministic_type_metadata_handoff && surface.deterministic_atomic_memory_order_mapping &&
         surface.deterministic_vector_type_lowering &&
         surface.atomic_memory_order_mapping.deterministic &&
         surface.vector_type_lowering.deterministic &&
         surface.globals_total == surface.type_metadata_global_entries &&
         surface.functions_total == surface.type_metadata_function_entries &&
         surface.interfaces_total == surface.type_metadata_interface_entries &&
         surface.implementations_total == surface.type_metadata_implementation_entries &&
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
