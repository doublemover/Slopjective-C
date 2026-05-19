#pragma once

#include <string>

// Source-mode boundary contracts own driver admission, public source-surface
// preservation, and the fail-closed boundary before generalized ARC automation.
inline constexpr const char *kObjc3ArcSourceModeBoundaryContractId =
    "objc3c.arc.source.mode.boundary.freeze.v1";
inline constexpr const char *kObjc3ArcSourceModeBoundarySourceModel =
    "ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode";
inline constexpr const char *kObjc3ArcSourceModeBoundaryModeModel =
    "native-driver-admits-fobjc-arc-and-fno-objc-arc-while-runnable-arc-stays-bounded-to-the-helper-backed-supported-slice";
inline constexpr const char *kObjc3ArcSourceModeBoundaryNonGoalModel =
    "no-generalized-arc-cleanup-insertion-no-public-arc-runtime-abi-mode-split-no-full-arc-automation-beyond-the-supported-helper-backed-slice";
inline constexpr const char *kObjc3ArcSourceModeBoundaryFailClosedModel =
    "fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation";

std::string Objc3ArcSourceModeBoundarySummary();
