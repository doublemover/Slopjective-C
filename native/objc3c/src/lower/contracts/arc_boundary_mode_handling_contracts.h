#pragma once

#include <string>

// ARC mode handling contracts own explicit mode threading through native
// driver, frontend sema, IR metadata, and non-ARC rejection semantics.
inline constexpr const char *kObjc3ArcModeHandlingContractId =
    "objc3c.arc.mode.handling.v1";
inline constexpr const char *kObjc3ArcModeHandlingSourceModel =
    "ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode";
inline constexpr const char *kObjc3ArcModeHandlingModeModel =
    "driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir";
inline constexpr const char *kObjc3ArcModeHandlingFailClosedModel =
    "non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures";
inline constexpr const char *kObjc3ArcModeHandlingNonGoalModel =
    "no-implicit-nonarc-promotion-no-cross-module-arc-mode-inference-no-full-arc-automation-beyond-the-supported-helper-backed-slice";

std::string Objc3ArcModeHandlingSummary(bool arc_mode_enabled);
