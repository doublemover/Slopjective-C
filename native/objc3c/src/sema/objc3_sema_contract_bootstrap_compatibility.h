#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_contracts.h"
#include "sema/objc3_sema_contract_type_canonical.h"

inline constexpr const char *kObjc3BootstrapLegalityFailureContractId =
    "objc3c.runtime.bootstrap.legality.duplicate.order.failure.contract.v1";
inline constexpr const char *kObjc3BootstrapLegalityFailureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_failure_contract";
inline constexpr const char *kObjc3BootstrapLegalityImageOrderInvariantModel =
    kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
inline constexpr const char *kObjc3BootstrapLegalitySemanticsContractId =
    "objc3c.runtime.bootstrap.legality.duplicate.order.semantics.v1";
inline constexpr const char *kObjc3BootstrapLegalitySemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_semantics";
inline constexpr const char *kObjc3BootstrapLegalityCrossImageLegalityModel =
    "translation-unit-identity-key-and-registration-order-ordinal-govern-bootstrap-legality";
inline constexpr const char *kObjc3BootstrapLegalitySemanticDiagnosticModel =
    "fail-closed-bootstrap-legality-before-runtime-handoff";
inline constexpr const char *kObjc3BootstrapFailureRestartSemanticsContractId =
    "objc3c.runtime.bootstrap.failure.restart.semantics.v1";
inline constexpr const char *kObjc3BootstrapFailureRestartSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_failure_restart_semantics";
inline constexpr const char *kObjc3BootstrapFailureRestartUnsupportedTopologyModel =
    "replay-requires-empty-live-runtime-state-and-retained-bootstrap-catalog";

struct Objc3BootstrapLegalityFailureContractSummary {
  std::string contract_id = kObjc3BootstrapLegalityFailureContractId;
  std::string surface_path = kObjc3BootstrapLegalityFailureSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string restart_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  std::string replay_order_model = kObjc3RuntimeBootstrapReplayOrderModel;
  std::string image_local_init_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  std::string catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  bool fail_closed = false;
  bool duplicate_registration_policy_frozen = false;
  bool image_order_invariant_frozen = false;
  bool bootstrap_rejection_frozen = false;
  bool restart_boundary_frozen = false;
  bool semantic_diagnostics_required = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3BootstrapLegalityFailureContractSummary(
    const Objc3BootstrapLegalityFailureContractSummary &summary) {
  return !summary.contract_id.empty() && !summary.surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         summary.fail_closed &&
         summary.duplicate_registration_policy_frozen &&
         summary.image_order_invariant_frozen &&
         summary.bootstrap_rejection_frozen &&
         summary.restart_boundary_frozen &&
         summary.semantic_diagnostics_required &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDependencyContractId =
    "objc3c.concurrency.await.suspension.resume.semantics.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryContractId =
    "objc3c.concurrency.async.diagnostics.compatibility.completion.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_async_diagnostics_and_compatibility_completion";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryRule =
    "async-topology-diagnostics-now-fail-closed-for-non-async-executor-affinity-async-function-prototypes-and-async-throws-while-runnable-frame-and-runtime-integration-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDeferredRule =
    "async-prototype-import-surfaces-async-error-propagation-abi-and-runnable-executor-runtime-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary {
  std::string contract_id =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t illegal_non_async_executor_sites = 0;
  std::size_t illegal_async_function_prototype_sites = 0;
  std::size_t illegal_async_throws_sites = 0;
  std::size_t compatibility_diagnostic_sites = 0;
  std::size_t supported_async_callable_sites = 0;
  bool dependency_required = false;
  bool executor_affinity_requires_async_enforced = false;
  bool async_function_prototypes_fail_closed = false;
  bool async_throws_fail_closed = false;
  bool unsupported_topology_fail_closed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummary(
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.executor_affinity_requires_async_enforced &&
         summary.async_function_prototypes_fail_closed &&
         summary.async_throws_fail_closed &&
         summary.unsupported_topology_fail_closed && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3BootstrapLegalitySemanticsSummary {
  std::string contract_id = kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_legality_failure_contract_id =
      kObjc3BootstrapLegalityFailureContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string surface_path = kObjc3BootstrapLegalitySemanticsSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string cross_image_legality_model =
      kObjc3BootstrapLegalityCrossImageLegalityModel;
  std::string semantic_diagnostic_model =
      kObjc3BootstrapLegalitySemanticDiagnosticModel;
  bool fail_closed = false;
  bool bootstrap_legality_failure_contract_ready = false;
  bool duplicate_registration_semantics_landed = false;
  bool image_order_semantics_landed = false;
  bool cross_image_legality_semantics_landed = false;
  bool semantic_diagnostics_landed = false;
  bool ready_for_lowering_and_runtime = false;
  std::string bootstrap_legality_failure_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3BootstrapLegalitySemanticsSummary(
    const Objc3BootstrapLegalitySemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_failure_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.cross_image_legality_model.empty() &&
         !summary.semantic_diagnostic_model.empty() && summary.fail_closed &&
         summary.bootstrap_legality_failure_contract_ready &&
         summary.duplicate_registration_semantics_landed &&
         summary.image_order_semantics_landed &&
         summary.cross_image_legality_semantics_landed &&
         summary.semantic_diagnostics_landed &&
         summary.ready_for_lowering_and_runtime &&
         !summary.bootstrap_legality_failure_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3BootstrapFailureRestartSemanticsSummary {
  std::string contract_id = kObjc3BootstrapFailureRestartSemanticsContractId;
  std::string bootstrap_legality_semantics_contract_id =
      kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_reset_contract_id =
      kObjc3RuntimeBootstrapResetContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string surface_path = kObjc3BootstrapFailureRestartSemanticsSurfacePath;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string restart_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  std::string replay_order_model = kObjc3RuntimeBootstrapReplayOrderModel;
  std::string image_local_init_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  std::string catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  std::string unsupported_topology_model =
      kObjc3BootstrapFailureRestartUnsupportedTopologyModel;
  bool fail_closed = false;
  bool bootstrap_legality_semantics_ready = false;
  bool failure_mode_semantics_landed = false;
  bool restart_semantics_landed = false;
  bool replay_semantics_landed = false;
  bool unsupported_topology_semantics_landed = false;
  bool deterministic_recovery_semantics_landed = false;
  bool ready_for_lowering_and_runtime = false;
  std::string bootstrap_legality_semantics_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3BootstrapFailureRestartSemanticsSummary(
    const Objc3BootstrapFailureRestartSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_semantics_contract_id.empty() &&
         !summary.bootstrap_reset_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.unsupported_topology_model.empty() &&
         summary.fail_closed &&
         summary.bootstrap_legality_semantics_ready &&
         summary.failure_mode_semantics_landed &&
         summary.restart_semantics_landed &&
         summary.replay_semantics_landed &&
         summary.unsupported_topology_semantics_landed &&
         summary.deterministic_recovery_semantics_landed &&
         summary.ready_for_lowering_and_runtime &&
         !summary.bootstrap_legality_semantics_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3CompatibilityStrictnessClaimSemanticsSummary {
  std::string contract_id =
      kObjc3CompatibilityStrictnessClaimSemanticsContractId;
  std::string runnable_feature_claim_inventory_contract_id =
      kObjc3RunnableFeatureClaimInventoryContractId;
  std::string feature_claim_truth_surface_contract_id =
      kObjc3FeatureClaimStrictnessTruthSurfaceContractId;
  std::string surface_path =
      kObjc3CompatibilityStrictnessClaimSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3CompatibilityStrictnessClaimSemanticModel;
  std::string downgrade_model =
      kObjc3CompatibilityStrictnessClaimDowngradeModel;
  std::string rejection_model =
      kObjc3CompatibilityStrictnessClaimRejectionModel;
  std::string canonical_interface_truth_model =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfaceTruthModel;
  std::string separate_compilation_macro_truth_model =
      kObjc3CompatibilityStrictnessClaimSeparateCompilationMacroTruthModel;
  std::string canonical_interface_payload_mode =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode;
  std::string effective_language_profile = "canonical";
  std::vector<std::string> suppressed_macro_claim_ids;
  std::size_t valid_language_profile_count = 0;
  std::size_t live_selection_surface_count = 0;
  std::size_t valid_selection_combination_count = 0;
  std::size_t runnable_feature_claim_count = 0;
  std::size_t downgraded_source_only_claim_count = 0;
  std::size_t rejected_unsupported_feature_claim_count = 0;
  std::size_t rejected_selection_surface_count = 0;
  std::size_t suppressed_macro_claim_count = 0;
  // unsupported-feature enforcement anchor: accepted
  // advanced source surfaces must either keep these counters at zero on the
  // runnable path or fail closed before lowering/runtime handoff.
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::size_t throws_source_rejection_site_count = 0;
  std::size_t blocks_source_rejection_site_count = 0;
  std::size_t arc_source_rejection_site_count = 0;
  bool fail_closed = false;
  bool language_profile_semantics_landed = false;
  bool canonical_literal_rejection_semantics_landed = false;
  bool source_only_claim_downgrade_semantics_landed = false;
  bool unsupported_feature_claim_rejection_semantics_landed = false;
  bool live_unsupported_feature_source_rejection_landed = false;
  bool strictness_selection_rejection_semantics_landed = false;
  bool feature_macro_claim_suppression_semantics_landed = false;
  bool canonical_interface_truth_semantics_landed = false;
  bool separate_compilation_macro_truth_semantics_landed = false;
  bool selected_configuration_valid = false;
  bool selected_configuration_downgraded = false;
  bool selected_configuration_rejected = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(
    const Objc3CompatibilityStrictnessClaimSemanticsSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical";
  return !summary.contract_id.empty() &&
         !summary.runnable_feature_claim_inventory_contract_id.empty() &&
         !summary.feature_claim_truth_surface_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.downgrade_model.empty() && !summary.rejection_model.empty() &&
         !summary.canonical_interface_truth_model.empty() &&
         !summary.separate_compilation_macro_truth_model.empty() &&
         summary.canonical_interface_payload_mode ==
             kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode &&
         language_profile_valid && summary.fail_closed &&
         summary.suppressed_macro_claim_ids.size() ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.suppressed_macro_claim_ids[0] ==
             kObjc3SuppressedMacroClaimStrictnessLevel &&
         summary.suppressed_macro_claim_ids[1] ==
             kObjc3SuppressedMacroClaimConcurrencyMode &&
         summary.suppressed_macro_claim_ids[2] ==
             kObjc3SuppressedMacroClaimConcurrencyStrict &&
         summary.valid_language_profile_count ==
             kObjc3CompatibilityStrictnessClaimValidLanguageProfileCount &&
         summary.live_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount &&
         summary.valid_selection_combination_count ==
             kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount &&
         summary.runnable_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRunnableFeatureCount &&
         summary.downgraded_source_only_claim_count ==
             kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount &&
         summary.rejected_unsupported_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRejectedFeatureCount &&
         summary.live_unsupported_feature_family_count <=
             summary.rejected_unsupported_feature_claim_count &&
         summary.throws_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.blocks_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.arc_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.live_unsupported_feature_diagnostic_count ==
             summary.live_unsupported_feature_site_count &&
         summary.throws_source_rejection_site_count +
                 summary.blocks_source_rejection_site_count +
                 summary.arc_source_rejection_site_count ==
             summary.live_unsupported_feature_site_count &&
         summary.rejected_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount &&
         summary.suppressed_macro_claim_count ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.language_profile_semantics_landed &&
         summary.canonical_literal_rejection_semantics_landed &&
         summary.source_only_claim_downgrade_semantics_landed &&
         summary.unsupported_feature_claim_rejection_semantics_landed &&
         summary.live_unsupported_feature_source_rejection_landed &&
         summary.strictness_selection_rejection_semantics_landed &&
         summary.feature_macro_claim_suppression_semantics_landed &&
         summary.canonical_interface_truth_semantics_landed &&
         summary.separate_compilation_macro_truth_semantics_landed &&
         summary.selected_configuration_valid &&
         !summary.selected_configuration_downgraded &&
         !summary.selected_configuration_rejected &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
