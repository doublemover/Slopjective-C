#pragma once

#include <string>

// ARC lifetime inference contracts own the supported runnable strong-owned
// inference slice and the zero-inference non-ARC fail-closed boundary.
inline constexpr const char *kObjc3ArcInferenceLifetimeContractId =
    "objc3c.arc.inference.lifetime.v1";
inline constexpr const char *kObjc3ArcInferenceLifetimeSourceModel =
    "explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice";
inline constexpr const char *kObjc3ArcInferenceLifetimeSemanticModel =
    "arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference";
inline constexpr const char *kObjc3ArcInferenceLifetimeFailClosedModel =
    "non-arc-mode-keeps-unqualified-object-signatures-non-inferred-and-zero-retain-release-lifetime-accounting";
inline constexpr const char *kObjc3ArcInferenceLifetimeNonGoalModel =
    "no-full-arc-cleanup-synthesis-no-weak-autorelease-return-property-synthesis-or-block-interaction-arc-semantics-yet";

std::string Objc3ArcInferenceLifetimeSummary();
