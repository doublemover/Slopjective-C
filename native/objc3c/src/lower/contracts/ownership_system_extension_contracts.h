#pragma once

#include <cstddef>
#include <string>

// Ownership system extension lowering owns cleanup resources, borrowed
// boundaries, retainable-family callable artifacts, and the helper-backed
// runtime integration surface that completes that ABI family.
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringContractId =
    "objc3c.ownership.system.extension.lowering.contract.v1";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_ownership_system_extension_lowering_contract";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringModel =
    "cleanup-resource-borrowed-and-retainable-family-sema-packets-now-feed-one-deterministic-ownership-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringDeferredModel =
    "live-cleanup-runtime-carriers-borrowed-lifetime-enforcement-and-runnable-retainable-family-runtime-interop-remain-later-system-extension-runtime-work";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringLaneContract =
    "objc3c.ownership.system.extension.lowering.contract.v1";

inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionContractId =
    "objc3c.ownership.borrowed.retainable.family.abi.completion.v1";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_ownership_borrowed_pointer_and_retainable_family_abi_completion";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionLaneContract =
    "objc3c.ownership.borrowed.retainable.family.abi.completion.v1";

inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeContractId =
    "objc3c.ownership.system.helper.runtime.contract.v1";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeSourceModel =
    "cleanup-resource-invalidation-and-retainable-family-runtime-proof-reuses-existing-private-arc-autorelease-and-snapshot-helpers";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeAbiModel =
    "private-retain-release-autorelease-autoreleasepool-and-testing-snapshot-helper-cluster";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimePackagingModel =
    "same-packaged-runtime-archive-no-public-runtime-header-widening-and-no-new-ownership-import-surface";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeFailClosedModel =
    "borrowed-lifetime-runtime-enforcement-and-escaping-resource-ownership-transfer-remain-deferred";

inline constexpr const char
    *kObjc3OwnershipLiveCleanupRetainableIntegrationContractId =
        "objc3c.ownership.live.cleanup.retainable.runtime.integration.v1";
inline constexpr const char
    *kObjc3OwnershipLiveCleanupRetainableIntegrationSourceModel =
        "supported-ownership-cleanup-resource-and-retainable-family-sites-now-link-and-execute-through-emitted-cleanup-calls-and-the-private-helper-cluster";
inline constexpr const char
    *kObjc3OwnershipLiveCleanupRetainableIntegrationExecutionModel =
        "linked-native-probes-execute-lifo-cleanup-resource-invalidation-and-retainable-family-helper-traffic-on-the-supported-slice";
inline constexpr const char
    *kObjc3OwnershipLiveCleanupRetainableIntegrationPackagingModel =
        "linked-module-object-plus-existing-runtime-support-archive-no-new-runtime-package-surface";
inline constexpr const char
    *kObjc3OwnershipLiveCleanupRetainableIntegrationFailClosedModel =
        "borrowed-lifetime-runtime-enforcement-and-escaping-resource-ownership-transfer-remain-deferred";

struct Objc3OwnershipSystemExtensionLoweringContract {
  std::size_t cleanup_hook_sites = 0;
  std::size_t resource_local_sites = 0;
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
  std::size_t borrowed_escape_candidate_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidObjc3OwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionLoweringContract &contract);
std::string Objc3OwnershipSystemExtensionLoweringReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract);
std::string Objc3OwnershipSystemHelperRuntimeContractSummary();
std::string Objc3OwnershipLiveCleanupRetainableIntegrationSummary();
