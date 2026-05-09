#pragma once

#include <string>

// Ownership runtime semantics own the source-to-runtime contract for
// runtime-backed object ownership attributes, retainable semantic freezing,
// storage qualifier legality, and autoreleasepool destruction-order limits.
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

std::string Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary();
std::string Objc3RetainableObjectSemanticRulesFreezeSummary();
std::string Objc3RuntimeBackedStorageOwnershipLegalitySummary();
std::string Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary();
