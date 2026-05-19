#pragma once

#include <string>

#include "ast/objc3_ast_contracts.h"

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
