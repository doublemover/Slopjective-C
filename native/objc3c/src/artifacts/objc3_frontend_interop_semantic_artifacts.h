#pragma once

#include <string>

#include "lower/contracts/interop_foreign_call_lowering_contracts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] Objc3InteropInteropLoweringContract
BuildInteropInteropLoweringContract(
    const Objc3InteropInteropSemanticModelSummary &semantic_summary,
    const Objc3InteropInteropRuntimeParitySummary &runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropSwiftInteropIsolationSummary &swift_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary);

[[nodiscard]] Objc3InteropForeignCallLifetimeLoweringContract
BuildInteropForeignCallLifetimeLoweringContract(
    const Objc3Program &program,
    const Objc3InteropInteropLoweringContract &dependency_contract,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary);

[[nodiscard]] std::string BuildInteropInteropSemanticModelSummaryJson(
    const Objc3InteropInteropSemanticModelSummary &summary);

[[nodiscard]] std::string BuildInteropInteropRuntimeParitySummaryJson(
    const Objc3InteropInteropRuntimeParitySummary &summary);

[[nodiscard]] std::string BuildInteropCppInteropInteractionSummaryJson(
    const Objc3InteropCppInteropInteractionSummary &summary);

[[nodiscard]] std::string BuildInteropSwiftInteropIsolationSummaryJson(
    const Objc3InteropSwiftInteropIsolationSummary &summary);

[[nodiscard]] std::string
BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary);

[[nodiscard]] std::string BuildInteropHeaderModuleBridgeGenerationSummaryJson(
    const Objc3InteropHeaderModuleBridgeGenerationSummary &summary);

[[nodiscard]] std::string BuildInteropInteropLoweringContractJson(
    const Objc3InteropInteropSemanticModelSummary &semantic_summary,
    const Objc3InteropInteropRuntimeParitySummary &runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropSwiftInteropIsolationSummary &swift_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropInteropLoweringContract &contract,
    const std::string &replay_key);

[[nodiscard]] std::string BuildInteropForeignCallLifetimeLoweringContractJson(
    const Objc3InteropInteropLoweringContract &dependency_contract,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropForeignCallLifetimeLoweringContract &contract,
    const std::string &replay_key);

}  // namespace objc3::artifacts::frontend
