#pragma once

#include <string>

// ARC runtime helper API surface contracts own the private helper ABI freeze
// for helper entrypoints that must not widen the public runtime header.
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceContractId =
    "objc3c.runtime.arc.helper.api.surface.freeze.v1";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceReferenceModel =
    "public-runtime-abi-stays-register-lookup-dispatch-while-arc-helper-entrypoints-remain-private-bootstrap-internal-runtime-abi";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceWeakModel =
    "weak-storage-and-current-property-access-remain-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel =
    "autorelease-return-and-autoreleasepool-support-remain-private-runtime-helper-behavior-without-public-abi-widening";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceFailClosedModel =
    "no-public-runtime-arc-helper-api-no-user-facing-arc-runtime-header-widening-yet";

std::string Objc3RuntimeArcHelperApiSurfaceSummary();
