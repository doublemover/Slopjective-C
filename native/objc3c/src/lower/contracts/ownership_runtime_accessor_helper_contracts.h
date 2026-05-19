#pragma once

#include <string>

// Accessor helper contracts own the runtime-backed property, weak storage, and
// synthesized ownership hook entrypoints that lowering is allowed to emit.
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

std::string Objc3OwnershipRuntimeHookEmissionSummary();
