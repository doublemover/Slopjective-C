#pragma once

#include <string>

// Runnable ARC runtime gate contracts own the supported ARC evidence gate that
// composes source, sema, lowering, and runtime proofs without widening scope.
inline constexpr const char *kObjc3RunnableArcRuntimeGateContractId =
    "objc3c.runnable.arc.runtime.gate.v1";
inline constexpr const char *kObjc3RunnableArcRuntimeGateEvidenceModel =
    "source-sema-lowering-runtime-summary-chain";
inline constexpr const char *kObjc3RunnableArcRuntimeGateActiveModel =
    "runnable-arc-gate-consumes-arc-mode-semantics-lowering-and-runtime-proofs-rather-than-parser-only-or-metadata-only-claims";
inline constexpr const char *kObjc3RunnableArcRuntimeGateNonGoalModel =
    "no-runnable-arc-closeout-matrix-no-public-runtime-abi-widening-no-cross-module-arc-claims-before-the-runnable-arc-closeout";
inline constexpr const char *kObjc3RunnableArcRuntimeGateFailClosedModel =
    "fail-closed-on-runnable-arc-runtime-evidence-drift";

std::string Objc3RunnableArcRuntimeGateSummary();
