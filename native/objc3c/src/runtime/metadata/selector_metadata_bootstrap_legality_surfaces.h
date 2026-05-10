#pragma once

#include <cstdint>
#include <string>

struct Objc3RuntimeBootstrapLegalityFailureContractSummary {
  std::string contract_id = kObjc3BootstrapLegalityFailureContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapLegalityFailureSurfacePath;
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
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool semantic_boundary_ready = false;
  bool duplicate_registration_policy_frozen = false;
  bool image_order_invariant_frozen = false;
  bool bootstrap_rejection_frozen = false;
  bool restart_boundary_frozen = false;
  bool semantic_diagnostics_required = false;
  bool ready_for_lowering_and_runtime = false;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string semantic_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.semantic_boundary_ready &&
         summary.duplicate_registration_policy_frozen &&
         summary.image_order_invariant_frozen &&
         summary.bootstrap_rejection_frozen &&
         summary.restart_boundary_frozen &&
         summary.semantic_diagnostics_required &&
         summary.ready_for_lowering_and_runtime &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapLegalitySemanticsSummary {
  std::string contract_id = kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_legality_failure_contract_id =
      kObjc3BootstrapLegalityFailureContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapLegalitySemanticsSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string cross_image_legality_model =
      kObjc3BootstrapLegalityCrossImageLegalityModel;
  std::string semantic_diagnostic_model =
      kObjc3BootstrapLegalitySemanticDiagnosticModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string translation_unit_identity_key;
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool bootstrap_legality_failure_contract_ready = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool duplicate_registration_semantics_landed = false;
  bool image_order_semantics_landed = false;
  bool cross_image_legality_semantics_landed = false;
  bool semantic_diagnostics_landed = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string bootstrap_legality_failure_contract_replay_key;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(
    const Objc3RuntimeBootstrapLegalitySemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_failure_contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.cross_image_legality_model.empty() &&
         !summary.semantic_diagnostic_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.translation_unit_identity_key.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.semantic_boundary_ready &&
         summary.bootstrap_legality_failure_contract_ready &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.duplicate_registration_semantics_landed &&
         summary.image_order_semantics_landed &&
         summary.cross_image_legality_semantics_landed &&
         summary.semantic_diagnostics_landed &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.bootstrap_legality_failure_contract_replay_key.empty() &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapFailureRestartSemanticsSummary {
  std::string contract_id = kObjc3BootstrapFailureRestartSemanticsContractId;
  std::string bootstrap_legality_semantics_contract_id =
      kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_reset_contract_id =
      kObjc3RuntimeBootstrapResetContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapFailureRestartSemanticsSurfacePath;
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
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string translation_unit_identity_key;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  std::string replay_registered_images_symbol =
      kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
  std::string reset_replay_state_snapshot_symbol =
      kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
  int invalid_descriptor_status_code =
      kObjc3RuntimeBootstrapInvalidDescriptorStatusCode;
  int invalid_registration_roots_status_code =
      kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool bootstrap_legality_semantics_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool bootstrap_reset_contract_ready = false;
  bool failure_mode_semantics_landed = false;
  bool restart_semantics_landed = false;
  bool replay_semantics_landed = false;
  bool unsupported_topology_semantics_landed = false;
  bool deterministic_recovery_semantics_landed = false;
  bool runtime_restart_probe_required = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string bootstrap_legality_semantics_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_semantics_contract_id.empty() &&
         !summary.bootstrap_reset_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.unsupported_topology_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.translation_unit_identity_key.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         !summary.replay_registered_images_symbol.empty() &&
         !summary.reset_replay_state_snapshot_symbol.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.semantic_boundary_ready &&
         summary.bootstrap_legality_semantics_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.bootstrap_reset_contract_ready &&
         summary.failure_mode_semantics_landed &&
         summary.restart_semantics_landed &&
         summary.replay_semantics_landed &&
         summary.unsupported_topology_semantics_landed &&
         summary.deterministic_recovery_semantics_landed &&
         summary.runtime_restart_probe_required &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.bootstrap_legality_semantics_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
