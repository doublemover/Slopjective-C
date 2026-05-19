#pragma once

#include <string>

#include "ast/objc3_ast_contracts_runtime_bootstrap_support_bootstrap_api.h"
#include "ast/objc3_ast_contracts_runtime_bootstrap_support_library.h"
#include "ast/objc3_ast_contracts_source_property_metadata.h"
#include "runtime/metadata/runtime_ownership_contracts.h"

struct Objc3RuntimeBootstrapApiSummary {
  std::string contract_id = kObjc3RuntimeBootstrapApiContractId;
  std::string owner_split_contract_id =
      objc3c::runtime::kObjc3RuntimeOwnerSplitContractId;
  std::string support_library_core_feature_contract_id =
      kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  std::string support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string bootstrap_surface_path = kObjc3RuntimeBootstrapApiSurfacePath;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string registration_status_enum_type =
      kObjc3RuntimeBootstrapApiStatusEnumType;
  std::string image_descriptor_type =
      kObjc3RuntimeBootstrapApiImageDescriptorType;
  std::string selector_handle_type =
      kObjc3RuntimeBootstrapApiSelectorHandleType;
  std::string registration_snapshot_type =
      kObjc3RuntimeBootstrapApiRegistrationSnapshotType;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string selector_lookup_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_entrypoint_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string state_snapshot_symbol = kObjc3RuntimeBootstrapStateSnapshotSymbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string registration_result_model = kObjc3RuntimeBootstrapResultModel;
  std::string registration_order_ordinal_model =
      kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
  std::string runtime_state_locking_model =
      kObjc3RuntimeBootstrapApiStateLockingModel;
  std::string startup_invocation_model =
      kObjc3RuntimeBootstrapApiStartupInvocationModel;
  std::string image_walk_lifecycle_model =
      kObjc3RuntimeBootstrapApiImageWalkLifecycleModel;
  std::string deterministic_reset_lifecycle_model =
      kObjc3RuntimeBootstrapApiDeterministicResetLifecycleModel;
  std::string public_registration_api_owner =
      objc3c::runtime::kObjc3RuntimePublicRegistrationApiOwner;
  std::string public_dispatch_diagnostics_owner =
      objc3c::runtime::kObjc3RuntimePublicDispatchDiagnosticsOwner;
  std::string fail_closed_ownership_model =
      objc3c::runtime::kObjc3RuntimeFailClosedOwnershipModel;
  bool fail_closed = false;
  bool support_library_core_feature_contract_ready = false;
  bool support_library_link_wiring_contract_ready = false;
  bool api_surface_frozen = false;
  bool registration_entrypoint_frozen = false;
  bool selector_lookup_and_dispatch_frozen = false;
  bool reset_and_snapshot_hooks_frozen = false;
  bool runtime_probe_required = false;
  bool image_walk_not_yet_landed = false;
  bool deterministic_reset_expansion_not_yet_landed = false;
  bool ready_for_registrar_implementation = false;
  bool public_metadata_ownership_explicit = true;
  bool retired_route_path_allowed = false;
  std::string support_library_core_feature_replay_key;
  std::string support_library_link_wiring_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapApiSummary(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.owner_split_contract_id.empty() &&
         !summary.support_library_core_feature_contract_id.empty() &&
         !summary.support_library_link_wiring_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.public_header_path.empty() &&
         !summary.archive_relative_path.empty() &&
         !summary.registration_status_enum_type.empty() &&
         !summary.image_descriptor_type.empty() &&
         !summary.selector_handle_type.empty() &&
         !summary.registration_snapshot_type.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.selector_lookup_symbol.empty() &&
         !summary.dispatch_entrypoint_symbol.empty() &&
         !summary.state_snapshot_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.registration_result_model.empty() &&
         !summary.registration_order_ordinal_model.empty() &&
         !summary.runtime_state_locking_model.empty() &&
         !summary.startup_invocation_model.empty() &&
         !summary.image_walk_lifecycle_model.empty() &&
         !summary.deterministic_reset_lifecycle_model.empty() &&
         !summary.public_registration_api_owner.empty() &&
         !summary.public_dispatch_diagnostics_owner.empty() &&
         !summary.fail_closed_ownership_model.empty() &&
         summary.fail_closed &&
         summary.support_library_core_feature_contract_ready &&
         summary.support_library_link_wiring_contract_ready &&
         summary.api_surface_frozen &&
         summary.registration_entrypoint_frozen &&
         summary.selector_lookup_and_dispatch_frozen &&
         summary.reset_and_snapshot_hooks_frozen &&
         summary.runtime_probe_required &&
         summary.image_walk_not_yet_landed &&
         summary.deterministic_reset_expansion_not_yet_landed &&
         summary.ready_for_registrar_implementation &&
         summary.public_metadata_ownership_explicit &&
         !summary.retired_route_path_allowed &&
         objc3c::runtime::RuntimeOwnerSplitContractIsReady() &&
         !summary.support_library_core_feature_replay_key.empty() &&
         !summary.support_library_link_wiring_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
