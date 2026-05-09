#pragma once

#include <string>

// Memory-management helper contracts own the retain/release/autorelease and
// autoreleasepool runtime ABI surface, plus the private implementation claims.
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

std::string Objc3RuntimeMemoryManagementApiSummary();
std::string Objc3RuntimeMemoryManagementImplementationSummary();
