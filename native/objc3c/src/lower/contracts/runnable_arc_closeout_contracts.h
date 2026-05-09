#pragma once

#include <string>

// Runnable ARC closeout contracts own the integrated closeout matrix for the
// current supported ARC slice without claiming broader runtime completeness.
inline constexpr const char *kObjc3RunnableArcCloseoutContractId =
    "objc3c.runnable.arc.closeout.v1";
inline constexpr const char *kObjc3RunnableArcCloseoutMatrixModel =
    "closeout-matrix-consumes-lowering-runtime-and-integrated-evidence-without-widening-the-supported-runnable-arc-slice";
inline constexpr const char *kObjc3RunnableArcCloseoutSmokeModel =
    "integrated-arc-fixtures-and-private-property-runtime-probes-prove-supported-cleanup-block-and-property-behavior-through-native-toolchain-and-runtime";
inline constexpr const char *kObjc3RunnableArcCloseoutFailClosedModel =
    "fail-closed-on-runnable-arc-closeout-drift-or-runbook-mismatch";

std::string Objc3RunnableArcCloseoutSummary();
