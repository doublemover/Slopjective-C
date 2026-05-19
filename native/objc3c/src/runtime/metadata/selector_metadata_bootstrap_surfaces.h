#pragma once

#include <cstdint>
#include <string>

#include "ast/objc3_ast_contracts_cross_module_link_plan.h"
#include "ast/objc3_ast_contracts_metadata_packaging.h"
#include "ast/objc3_ast_contracts_runtime_bootstrap_support_bootstrap_api.h"
#include "ast/objc3_ast_contracts_runtime_bootstrap_support_registration_descriptor.h"
#include "ast/objc3_ast_contracts_runtime_bootstrap_support_library.h"
#include "lower/contracts/runtime_bootstrap_entry_lowering_contracts.h"
#include "lower/contracts/runtime_bootstrap_image_root_contracts.h"

struct Objc3RuntimeStartupBootstrapInvariantSummary {
  std::string contract_id = kObjc3RuntimeStartupBootstrapInvariantContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_surface_path =
      kObjc3RuntimeStartupBootstrapInvariantSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string realization_order_policy =
      kObjc3RuntimeStartupBootstrapRealizationOrderPolicy;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string image_local_initialization_scope =
      kObjc3RuntimeStartupBootstrapImageLocalInitializationScope;
  std::string constructor_root_uniqueness_policy =
      kObjc3RuntimeStartupBootstrapConstructorRootUniquenessPolicy;
  std::string constructor_root_consumption_model =
      kObjc3RuntimeStartupBootstrapConsumptionModel;
  std::string startup_execution_mode =
      kObjc3RuntimeStartupBootstrapExecutionMode;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string manifest_authority_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool duplicate_registration_semantics_frozen = false;
  bool realization_order_semantics_frozen = false;
  bool failure_mode_semantics_frozen = false;
  bool image_local_initialization_scope_frozen = false;
  bool constructor_root_uniqueness_frozen = false;
  bool startup_execution_not_yet_landed = false;
  bool live_duplicate_registration_enforcement_not_yet_landed = false;
  bool image_local_realization_not_yet_landed = false;
  bool ready_for_bootstrap_implementation = false;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.realization_order_policy.empty() &&
         !summary.failure_mode.empty() &&
         !summary.image_local_initialization_scope.empty() &&
         !summary.constructor_root_uniqueness_policy.empty() &&
         !summary.constructor_root_consumption_model.empty() &&
         !summary.startup_execution_mode.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.manifest_authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.duplicate_registration_semantics_frozen &&
         summary.realization_order_semantics_frozen &&
         summary.failure_mode_semantics_frozen &&
         summary.image_local_initialization_scope_frozen &&
         summary.constructor_root_uniqueness_frozen &&
         summary.startup_execution_not_yet_landed &&
         summary.live_duplicate_registration_enforcement_not_yet_landed &&
         summary.image_local_realization_not_yet_landed &&
         summary.ready_for_bootstrap_implementation &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapSemanticsSummary {
  std::string contract_id = kObjc3RuntimeBootstrapSemanticsContractId;
  std::string bootstrap_invariant_contract_id =
      kObjc3RuntimeStartupBootstrapInvariantContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_surface_path =
      kObjc3RuntimeBootstrapSemanticsSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string realization_order_policy =
      kObjc3RuntimeStartupBootstrapRealizationOrderPolicy;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string image_local_initialization_scope =
      kObjc3RuntimeStartupBootstrapImageLocalInitializationScope;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string manifest_authority_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string runtime_library_archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string registration_result_model =
      kObjc3RuntimeBootstrapResultModel;
  std::string registration_order_ordinal_model =
      kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  int success_status_code = kObjc3RuntimeBootstrapSuccessStatusCode;
  int invalid_descriptor_status_code =
      kObjc3RuntimeBootstrapInvalidDescriptorStatusCode;
  int duplicate_registration_status_code =
      kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode;
  int out_of_order_status_code =
      kObjc3RuntimeBootstrapOutOfOrderStatusCode;
  int invalid_registration_roots_status_code =
      kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool bootstrap_invariant_contract_ready = false;
  bool registration_manifest_contract_ready = false;
  bool live_runtime_enforcement_landed = false;
  bool registration_manifest_bootstrap_semantics_published = false;
  bool runtime_probe_required = false;
  bool no_partial_commit_on_failure = false;
  bool ready_for_constructor_root_implementation = false;
  std::string bootstrap_invariant_replay_key;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapSemanticsSummary(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_invariant_contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.realization_order_policy.empty() &&
         !summary.failure_mode.empty() &&
         !summary.image_local_initialization_scope.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.manifest_authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.runtime_library_archive_relative_path.empty() &&
         !summary.registration_result_model.empty() &&
         !summary.registration_order_ordinal_model.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed &&
         summary.bootstrap_invariant_contract_ready &&
         summary.registration_manifest_contract_ready &&
         summary.live_runtime_enforcement_landed &&
         summary.registration_manifest_bootstrap_semantics_published &&
         summary.runtime_probe_required &&
         summary.no_partial_commit_on_failure &&
         summary.ready_for_constructor_root_implementation &&
         !summary.bootstrap_invariant_replay_key.empty() &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapLoweringSummary {
  std::string contract_id = kObjc3RuntimeBootstrapLoweringContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId;
  std::string registration_descriptor_artifact =
      kObjc3RuntimeBootstrapRegistrationDescriptorArtifact;
  std::string bootstrap_surface_path =
      "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract";
  std::string lowering_boundary_model =
      kObjc3RuntimeBootstrapLoweringBoundaryModel;
  std::string registration_descriptor_handoff_model =
      kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string constructor_init_stub_symbol_prefix =
      kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix;
  std::string registration_table_symbol_prefix =
      kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix;
  std::string image_local_init_state_symbol_prefix =
      kObjc3RuntimeBootstrapImageLocalInitStateSymbolPrefix;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string global_ctor_list_model =
      kObjc3RuntimeBootstrapGlobalCtorListModel;
  std::string registration_table_layout_model =
      kObjc3RuntimeBootstrapRegistrationTableLayoutModel;
  std::string image_local_initialization_model =
      kObjc3RuntimeBootstrapImageLocalInitializationModel;
  std::uint64_t registration_table_abi_version =
      kObjc3RuntimeBootstrapRegistrationTableAbiVersion;
  std::uint64_t registration_table_pointer_field_count =
      kObjc3RuntimeBootstrapRegistrationTablePointerFieldCount;
  std::string constructor_root_emission_state =
      kObjc3RuntimeBootstrapConstructorRootEmissionState;
  std::string init_stub_emission_state =
      kObjc3RuntimeBootstrapInitStubEmissionState;
  std::string registration_table_emission_state =
      kObjc3RuntimeBootstrapRegistrationTableEmissionState;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool lowering_contract_published = false;
  bool manifest_authority_preserved = false;
  bool no_bootstrap_ir_materialization_yet = false;
  bool bootstrap_ir_materialization_landed = false;
  bool image_local_initialization_landed = false;
  bool ready_for_bootstrap_materialization = false;
  std::string registration_manifest_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLoweringSummary(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.registration_descriptor_artifact.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.lowering_boundary_model.empty() &&
         !summary.registration_descriptor_handoff_model.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.constructor_init_stub_symbol_prefix.empty() &&
         !summary.registration_table_symbol_prefix.empty() &&
         !summary.image_local_init_state_symbol_prefix.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.global_ctor_list_model.empty() &&
         !summary.registration_table_layout_model.empty() &&
         !summary.image_local_initialization_model.empty() &&
         summary.registration_table_abi_version > 0 &&
         summary.registration_table_pointer_field_count > 0 &&
         !summary.constructor_root_emission_state.empty() &&
         !summary.init_stub_emission_state.empty() &&
         !summary.registration_table_emission_state.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.lowering_contract_published &&
         summary.manifest_authority_preserved &&
         !summary.no_bootstrap_ir_materialization_yet &&
         summary.bootstrap_ir_materialization_landed &&
         summary.image_local_initialization_landed &&
         summary.ready_for_bootstrap_materialization &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
