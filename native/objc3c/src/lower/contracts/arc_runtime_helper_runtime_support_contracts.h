#pragma once

#include <string>

// ARC runtime helper runtime-support contracts own the executable private
// helper proof for the supported weak/property/autorelease-return slice.
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportContractId =
    "objc3c.runtime.arc.helper.runtime.support.v1";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportDependencyModel =
    "runtime-baseline-plus-runnable-arc-lowering-plus-private-helper-surface";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportWeakModel =
    "arc-generated-weak-current-property-access-lowers-and-links-through-private-runtime-helper-entrypoints";
inline constexpr const char
    *kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel =
        "arc-generated-autorelease-return-paths-link-and-execute-through-private-runtime-helper-entrypoints";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportExecutionModel =
    "runtime-library-backed-helper-entrypoints-remain-private-but-executable-through-linked-native-arc-programs";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel =
    "unsupported-arc-runtime-surfaces-stay-private-fixture-proven-and-fail-closed-outside-the-supported-slice";

std::string Objc3RuntimeArcHelperRuntimeSupportSummary();
