#pragma once

#include <string>

#include "lower/contracts/ownership_system_extension_contracts.h"

// Runtime-backed object ownership is the lowering-owned boundary where
// property/member ownership facts become emitted runtime metadata instead of
// manifest-only evidence.
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId =
        "objc3c.runtime.backed.object.ownership.attribute.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeSourceModel =
        "runtime-backed-property-source-surface-publishes-attribute-lifetime-hook-and-accessor-ownership-profiles";
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeDescriptorModel =
        "emitted-property-descriptor-records-carry-attribute-lifetime-hook-and-accessor-ownership-strings";
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeRuntimeModel =
        "runtime-backed-property-metadata-consumes-emitted-ownership-strings-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeFailClosedModel =
        "no-manifest-only-ownership-proof-no-source-recovery-no-live-arc-hook-emission-yet";

inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesFreezeContractId =
        "objc3c.retainable.object.semantic.rules.freeze.v1";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesSemanticModel =
        "runtime-backed-object-semantic-rules-freeze-property-member-ownership-metadata-while-retain-release-remains-summary-driven-and-runtime-backed-storage-legality-is-live-sema-enforced";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesDestructionModel =
        "destruction-order-autoreleasepool-and-live-arc-execution-stay-fail-closed-outside-runtime-backed-storage-legality";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesFailClosedModel =
        "fail-closed-on-retainable-object-semantic-drift-or-premature-live-storage-legality-claim";

inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipLegalityContractId =
        "objc3c.runtime.backed.storage.ownership.legality.v1";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipOwnedStorageModel =
        "explicit-strong-object-property-qualifiers-remain-legal-for-owned-runtime-backed-storage-while-conflicting-weak-or-unowned-modifiers-fail-closed";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipWeakUnownedModel =
        "explicit-weak-and-unsafe-unretained-object-property-qualifiers-bind-runtime-backed-storage-legality-and-reject-conflicting-property-modifiers";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipFailClosedModel =
        "fail-closed-on-runtime-backed-object-property-ownership-qualifier-modifier-drift";

inline constexpr const char
    *kObjc3RuntimeBackedAutoreleasepoolDestructionOrderContractId =
        "objc3c.runtime.backed.autoreleasepool.destruction.order.semantics.v1";
inline constexpr const char *kObjc3RuntimeBackedAutoreleasepoolModel =
    "autoreleasepool-scopes-remain-fail-closed-while-owned-runtime-backed-object-storage-publishes-destruction-order-edge-diagnostics";
inline constexpr const char *kObjc3RuntimeBackedDestructionOrderModel =
    "owned-runtime-backed-object-or-synthesized-property-storage-inside-autoreleasepool-requires-deferred-destruction-order-runtime-support";
inline constexpr const char
    *kObjc3RuntimeBackedAutoreleasepoolDestructionOrderFailClosedModel =
        "fail-closed-on-autoreleasepool-destruction-order-semantic-drift-for-owned-runtime-backed-storage";

inline constexpr const char *kObjc3OwnershipLoweringBaselineContractId =
    "objc3c.ownership.lowering.baseline.freeze.v1";
inline constexpr const char *kObjc3OwnershipLoweringBaselineQualifierModel =
    "ownership-qualifier-lowering-remains-legacy-summary-driven-for-runtime-backed-object-metadata";
inline constexpr const char *kObjc3OwnershipLoweringBaselineRuntimeHookModel =
    "retain-release-autorelease-and-weak-lowering-stays-summary-only-without-live-runtime-hook-emission";
inline constexpr const char *kObjc3OwnershipLoweringBaselineAutoreleasepoolModel =
    "autoreleasepool-lowering-remains-summary-only-without-emitted-push-pop-hooks";
inline constexpr const char *kObjc3OwnershipLoweringBaselineFailClosedModel =
    "no-live-ownership-runtime-hooks-no-arc-weak-side-table-entrypoints-no-destruction-lowering-yet";

inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionContractId =
    "objc3c.ownership.runtime.hook.emission.v1";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionAccessorModel =
    "synthesized-accessors-call-runtime-owned-current-property-and-ownership-hook-entrypoints";
inline constexpr const char
    *kObjc3OwnershipRuntimeHookEmissionPropertyContextModel =
        "runtime-dispatch-frame-selects-current-receiver-property-accessor-and-autorelease-queue";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel =
    "autorelease-values-drain-at-runtime-dispatch-return";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionFailClosedModel =
    "owned-and-weak-runtime-backed-accessors-may-not-fall-back-to-summary-only-lowering";

inline constexpr const char *kObjc3RuntimeReadCurrentPropertyI32Symbol =
    "objc3_runtime_read_current_property_i32";
inline constexpr const char *kObjc3RuntimeWriteCurrentPropertyI32Symbol =
    "objc3_runtime_write_current_property_i32";
inline constexpr const char *kObjc3RuntimeExchangeCurrentPropertyI32Symbol =
    "objc3_runtime_exchange_current_property_i32";
inline constexpr const char *kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol =
    "objc3_runtime_load_weak_current_property_i32";
inline constexpr const char *kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol =
    "objc3_runtime_store_weak_current_property_i32";
inline constexpr const char *kObjc3RuntimeRetainI32Symbol =
    "objc3_runtime_retain_i32";
inline constexpr const char *kObjc3RuntimeReleaseI32Symbol =
    "objc3_runtime_release_i32";
inline constexpr const char *kObjc3RuntimeAutoreleaseI32Symbol =
    "objc3_runtime_autorelease_i32";
inline constexpr const char *kObjc3RuntimePushAutoreleasepoolScopeSymbol =
    "objc3_runtime_push_autoreleasepool_scope";
inline constexpr const char *kObjc3RuntimePopAutoreleasepoolScopeSymbol =
    "objc3_runtime_pop_autoreleasepool_scope";

inline constexpr const char *kObjc3RuntimeMemoryManagementApiContractId =
    "objc3c.runtime.memory.management.api.freeze.v1";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiReferenceModel =
    "public-runtime-abi-stays-register-lookup-dispatch-while-reference-counting-helpers-remain-private-runtime-entrypoints";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiWeakModel =
    "weak-storage-remains-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel =
    "no-public-autoreleasepool-push-pop-api-yet-autorelease-helper-drains-only-on-dispatch-frame-return";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiFailClosedModel =
    "no-public-memory-management-header-widening-no-user-facing-arc-entrypoints-yet";

inline constexpr const char
    *kObjc3RuntimeMemoryManagementImplementationContractId =
        "objc3c.runtime.memory.management.implementation.v1";
inline constexpr const char
    *kObjc3RuntimeMemoryManagementImplementationRefcountModel =
        "runtime-managed-instance-retain-counts-destroy-strong-owned-storage-on-final-release";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationWeakModel =
    "weak-side-table-tracks-runtime-storage-observers-and-zeroes-them-on-final-release";
inline constexpr const char
    *kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel =
        "private-autoreleasepool-push-pop-scopes-retain-autoreleased-runtime-values-until-lifo-drain";
inline constexpr const char
    *kObjc3RuntimeMemoryManagementImplementationFailClosedModel =
        "memory-management-runtime-support-remains-private-lowered-and-runtime-probe-driven";

inline constexpr const char *kObjc3OwnershipRuntimeGateContractId =
    "objc3c.ownership.runtime.gate.freeze.v1";
inline constexpr const char *kObjc3OwnershipRuntimeGateSupportedModel =
    "runtime-backed-object-baseline-proves-strong-weak-and-autoreleasepool-behavior-through-private-runtime-hooks";
inline constexpr const char *kObjc3OwnershipRuntimeGateEvidenceModel =
    "gate-consumes-ownership-runtime-contract-summaries-and-runtime-probe-evidence";
inline constexpr const char *kObjc3OwnershipRuntimeGateNonGoalModel =
    "no-arc-automation-no-block-ownership-runtime-no-public-ownership-api-widening";
inline constexpr const char *kObjc3OwnershipRuntimeGateFailClosedModel =
    "integration-gate-must-not-claim-more-than-the-supported-runtime-backed-ownership-baseline";

std::string Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary();
std::string Objc3RetainableObjectSemanticRulesFreezeSummary();
std::string Objc3RuntimeBackedStorageOwnershipLegalitySummary();
std::string Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary();
std::string Objc3OwnershipLoweringBaselineSummary();
std::string Objc3OwnershipRuntimeHookEmissionSummary();
std::string Objc3RuntimeMemoryManagementApiSummary();
std::string Objc3RuntimeMemoryManagementImplementationSummary();
std::string Objc3OwnershipRuntimeGateSummary();
