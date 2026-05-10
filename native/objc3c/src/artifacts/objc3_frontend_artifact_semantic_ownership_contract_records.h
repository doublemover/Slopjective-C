#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3OwnershipQualifierLoweringLaneContract =
    "objc3c.ownership.qualifier.lowering.v1";
inline constexpr const char *kObjc3RetainReleaseOperationLoweringLaneContract =
    "objc3c.retain.release.operation.lowering.v1";
inline constexpr const char *kObjc3AutoreleasePoolScopeLoweringLaneContract =
    "objc3c.autoreleasepool.scope.lowering.v1";
inline constexpr const char *kObjc3WeakUnownedSemanticsLoweringLaneContract =
    "objc3c.weak.unowned.semantics.lowering.v1";
inline constexpr const char *kObjc3ArcDiagnosticsFixitLoweringLaneContract =
    "objc3c.arc.diagnostics.fixit.lowering.v1";

struct Objc3OwnershipQualifierLoweringContract {
  std::size_t ownership_qualifier_sites = 0;
  std::size_t invalid_ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_annotation_sites = 0;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationLoweringContract {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AutoreleasePoolScopeLoweringContract {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  unsigned max_scope_depth = 0;
  std::size_t scope_entry_transition_sites = 0;
  std::size_t scope_exit_transition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsLoweringContract {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitLoweringContract {
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

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

bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract);
std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract);
bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract);
std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract);
bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
bool IsValidObjc3OwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionLoweringContract &contract);
std::string Objc3OwnershipSystemExtensionLoweringReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract);
