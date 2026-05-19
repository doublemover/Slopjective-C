#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "contracts/objc3_diagnostic_owner_contract.h"

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
inline constexpr const char *kObjc3SemaCoreSemanticSummaryReadinessOwner =
    "native.frontend.sema.core-semantic-summary-readiness";
inline constexpr const char *kObjc3SemaSelectorPropertyTypeAnnotationReadinessOwner =
    "native.frontend.sema.selector-property-type-annotation-readiness";
inline constexpr const char *kObjc3SemaTypeBoundarySummaryReadinessOwner =
    "native.frontend.sema.type-boundary-summary-readiness";
inline constexpr const char *kObjc3SemaModuleTypeAbiSummaryReadinessOwner =
    "native.frontend.sema.module-type-abi-summary-readiness";
inline constexpr const char *kObjc3SemaModuleBoundarySummaryReadinessOwner =
    "native.frontend.sema.module-boundary-summary-readiness";
inline constexpr const char *kObjc3SemaIntermoduleFlowSummaryReadinessOwner =
    "native.frontend.sema.intermodule-flow-summary-readiness";
inline constexpr const char *kObjc3SemaModuleSemanticParityPublicationReadinessOwner =
    "native.frontend.sema.module-semantic-parity-publication-readiness";
inline constexpr const char *kObjc3SemaIntermoduleFlowParityPublicationReadinessOwner =
    "native.frontend.sema.intermodule-flow-parity-publication-readiness";
inline constexpr const char *kObjc3SemaConcurrencyParityPublicationReadinessOwner =
    "native.frontend.sema.concurrency-parity-publication-readiness";
inline constexpr const char *kObjc3SemaUnsafeErrorParityValidationReadinessOwner =
    "native.frontend.sema.unsafe-error-parity-validation-readiness";
inline constexpr const char *kObjc3SemaControlBindingParityValidationReadinessOwner =
    "native.frontend.sema.control-binding-parity-validation-readiness";
inline constexpr const char *kObjc3SemaAsyncBlockMessageParityValidationReadinessOwner =
    "native.frontend.sema.async-block-message-parity-validation-readiness";
inline constexpr const char *kObjc3SemaDispatchRuntimeArcParityValidationReadinessOwner =
    "native.frontend.sema.dispatch-runtime-arc-parity-validation-readiness";
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
inline constexpr const char *kObjc3SemaAtomicVectorMappingPublicationOwner =
    "native.frontend.sema.atomic-vector-mapping-publication";
inline constexpr const char *kObjc3SemaTypedSemanticHandoffPublicationOwner =
    "native.frontend.sema.typed-semantic-handoff-publication";
inline constexpr const char *kObjc3SemaParityValidationOwner =
    "native.frontend.sema.parity-validation";
inline constexpr const char *kObjc3SemaParityCloseoutPublicationReadinessOwner =
    "native.frontend.sema.parity-closeout-publication-readiness";
inline constexpr const char *kObjc3SemaCloseoutSurfaceReadinessOwner =
    "native.frontend.sema.closeout-surface-readiness";
inline constexpr const char *kObjc3SemaCloseoutSignoffOwner =
    "native.frontend.sema.closeout-signoff";
inline constexpr const char *kObjc3SemaNoRetiredRouteOwnerModel =
    "strict-hard-cutover-no-retired-route-no-compatibility-gate";

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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool owner_split_explicit = false;
  bool strict_no_retired_route = true;
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
    bool strict_no_retired_route,
    bool strict_no_compatibility) {
  return Objc3SemaOwnerIsExplicit(stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(diagnostic_handoff_owner) &&
         owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         strict_no_retired_route && strict_no_compatibility &&
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
             summary.strict_no_retired_route,
             summary.strict_no_compatibility) &&
         !summary.recovery_counts_as_success &&
         summary.replay_key_deterministic &&
         summary.symbol_globals_count == summary.type_metadata_global_entries &&
         summary.symbol_functions_count == summary.type_metadata_function_entries &&
         summary.symbol_interfaces_count == summary.type_metadata_interface_entries &&
         summary.symbol_implementations_count == summary.type_metadata_implementation_entries &&
         summary.deterministic;
}
