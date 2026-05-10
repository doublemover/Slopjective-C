#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/model/semantic_symbol_advanced_source_contracts.h"
#include "sema/model/semantic_symbol_core_source_closures.h"

struct Objc3FrontendOwnershipSystemExtensionSourceClosureSummary {
  std::string contract_id = kObjc3OwnershipSystemExtensionSourceClosureContractId;
  std::string frontend_surface_path = kObjc3OwnershipSystemExtensionSourceClosureSurfacePath;
  std::string source_model = kObjc3OwnershipSystemExtensionSourceClosureSourceModel;
  std::string failure_model = kObjc3OwnershipSystemExtensionSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimResourceHandleAnnotations,
      kObjc3SourceOnlyFeatureClaimBorrowedPointerAnnotations,
      kObjc3SourceOnlyFeatureClaimBorrowedReturnRelations,
      kObjc3SourceOnlyFeatureClaimExplicitBlockCaptureLists,
  };
  std::size_t resource_attribute_sites = 0;
  std::size_t resource_close_clause_sites = 0;
  std::size_t resource_invalid_clause_sites = 0;
  std::size_t borrowed_pointer_sites = 0;
  std::size_t returns_borrowed_attribute_sites = 0;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_weak_sites = 0;
  std::size_t explicit_capture_unowned_sites = 0;
  std::size_t explicit_capture_move_sites = 0;
  std::size_t explicit_capture_plain_sites = 0;
  bool resource_attribute_source_supported = false;
  bool borrowed_pointer_source_supported = false;
  bool returns_borrowed_source_supported = false;
  bool explicit_capture_list_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary {
  std::string contract_id =
      kObjc3OwnershipCleanupResourceCaptureSurfaceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3OwnershipCleanupResourceCaptureSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3OwnershipCleanupResourceCaptureSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3OwnershipCleanupResourceCaptureSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimCleanupHookAnnotations,
      kObjc3SourceOnlyFeatureClaimResourceHandleAnnotations,
      kObjc3SourceOnlyFeatureClaimCleanupResourceSugar,
      kObjc3SourceOnlyFeatureClaimExplicitBlockCaptureLists,
  };
  std::size_t cleanup_attribute_sites = 0;
  std::size_t cleanup_sugar_sites = 0;
  std::size_t resource_attribute_sites = 0;
  std::size_t resource_sugar_sites = 0;
  std::size_t resource_close_clause_sites = 0;
  std::size_t resource_invalid_clause_sites = 0;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_weak_sites = 0;
  std::size_t explicit_capture_unowned_sites = 0;
  std::size_t explicit_capture_move_sites = 0;
  std::size_t explicit_capture_plain_sites = 0;
  bool cleanup_attribute_source_supported = false;
  bool resource_sugar_source_supported = false;
  bool explicit_capture_list_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary {
  std::string contract_id =
      kObjc3OwnershipRetainableCFamilySourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3OwnershipRetainableCFamilySourceCompletionSurfacePath;
  std::string source_model =
      kObjc3OwnershipRetainableCFamilySourceCompletionSourceModel;
  std::string failure_model =
      kObjc3OwnershipRetainableCFamilySourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimRetainableCFamilyCallableAnnotations,
      kObjc3SourceOnlyFeatureClaimRetainableCFamilyCompatibilityAliases,
  };
  std::size_t family_retain_sites = 0;
  std::size_t family_release_sites = 0;
  std::size_t family_autorelease_sites = 0;
  std::size_t compatibility_returns_retained_sites = 0;
  std::size_t compatibility_returns_not_retained_sites = 0;
  std::size_t compatibility_consumed_sites = 0;
  bool callable_annotation_source_supported = false;
  bool compatibility_alias_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendDispatchDispatchIntentSourceClosureSummary {
  std::string contract_id = kObjc3DispatchDispatchIntentSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3DispatchDispatchIntentSourceClosureSurfacePath;
  std::string source_model = kObjc3DispatchDispatchIntentSourceClosureSourceModel;
  std::string failure_model = kObjc3DispatchDispatchIntentSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimDirectMethodAnnotations,
      kObjc3SourceOnlyFeatureClaimDirectMembersClassAnnotations,
      kObjc3SourceOnlyFeatureClaimFinalAnnotations,
      kObjc3SourceOnlyFeatureClaimSealedAnnotations,
      kObjc3SourceOnlyFeatureClaimDynamicMethodAnnotations,
  };
  std::size_t direct_callable_sites = 0;
  std::size_t final_callable_sites = 0;
  std::size_t dynamic_callable_sites = 0;
  std::size_t direct_members_container_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t actor_container_sites = 0;
  bool callable_annotation_source_supported = false;
  bool container_annotation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendDispatchDispatchIntentSourceCompletionSummary {
  std::string contract_id = kObjc3DispatchDispatchIntentSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3DispatchDispatchIntentSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3DispatchDispatchIntentSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3DispatchDispatchIntentSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimPrefixedDispatchIntentAttributes,
      kObjc3SourceOnlyFeatureClaimDirectMembersDefaultingSurfaces,
      kObjc3SourceOnlyFeatureClaimDynamicOptOutDefaultingSurfaces,
      kObjc3SourceOnlyFeatureClaimFinalAnnotations,
      kObjc3SourceOnlyFeatureClaimSealedAnnotations,
  };
  std::size_t prefixed_container_attribute_sites = 0;
  std::size_t direct_members_container_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t effective_direct_member_sites = 0;
  std::size_t direct_members_defaulted_method_sites = 0;
  std::size_t direct_members_dynamic_opt_out_sites = 0;
  bool prefixed_attribute_source_supported = false;
  bool defaulting_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};
