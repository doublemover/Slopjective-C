#pragma once

#include <cstddef>
#include <string>

inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDependencyContractId =
        "objc3c.ownership.borrowed.pointer.escape.analysis.v1";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionContractId =
        "objc3c.ownership.capture.list.retainable.family.legality.v1";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSurfacePath =
        "frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionRule =
        "explicit-capture-lists-now-fail-closed-on-duplicate-unused-and-non-object-ownership-modes-while-retainable-family-callables-fail-closed-on-conflicting-and-shape-invalid-annotations-before-lowering-and-runtime-integration";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDeferredRule =
        "lowering-runtime-interop-and-runnable-retainable-family-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary {
  std::string contract_id =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionContractId;
  std::string dependency_contract_id =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDependencyContractId;
  std::string surface_path =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSurfacePath;
  std::string semantic_model =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionRule;
  std::string deferred_model =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDeferredRule;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_ownership_mode_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
  std::size_t illegal_duplicate_explicit_capture_sites = 0;
  std::size_t illegal_non_object_capture_mode_sites = 0;
  std::size_t illegal_unused_explicit_capture_sites = 0;
  std::size_t illegal_conflicting_retainable_family_sites = 0;
  std::size_t illegal_invalid_family_operation_shape_sites = 0;
  std::size_t illegal_invalid_family_alias_shape_sites = 0;
  bool dependency_required = false;
  bool explicit_capture_duplicate_fail_closed = false;
  bool explicit_capture_ownership_mode_enforced = false;
  bool explicit_capture_inventory_enforced = false;
  bool retainable_family_conflict_enforced = false;
  bool retainable_family_operation_shape_enforced = false;
  bool retainable_family_alias_shape_enforced = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool
IsReadyObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.explicit_capture_duplicate_fail_closed &&
         summary.explicit_capture_ownership_mode_enforced &&
         summary.explicit_capture_inventory_enforced &&
         summary.retainable_family_conflict_enforced &&
         summary.retainable_family_operation_shape_enforced &&
         summary.retainable_family_alias_shape_enforced &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}
