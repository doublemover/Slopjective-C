#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"
#include "lower/contracts/interop_ffi_metadata_contracts.h"
#include "lower/contracts/interop_foreign_call_lowering_contracts.h"
#include "sema/model/semantic_ownership.h"
#include "sema/model/semantic_symbol_metaprogramming_interop_summaries.h"
#include "sema/objc3_sema_contract_interop_diagnostics.h"

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

[[nodiscard]] Objc3InteropForeignSurfaceInterfacePreservationSummary
BuildInteropForeignSurfaceInterfacePreservationSummary(
    const Objc3Program &program,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &foreign_import_source_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &cpp_swift_source_summary,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces);

[[nodiscard]] Objc3InteropFfiMetadataInterfacePreservationContract
BuildInteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    bool runtime_import_artifact_ready,
    std::string &replay_key_out);

[[nodiscard]] Objc3InteropHeaderModuleBridgeGenerationSummary
BuildInteropHeaderModuleBridgeGenerationSummary(
    const Objc3Program &program,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &ffi_preservation_contract,
    const std::string &ffi_preservation_replay_key,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces);

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

[[nodiscard]] std::string BuildInteropFfiMetadataInterfacePreservationContractJson(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract,
    const std::string &replay_key);

}  // namespace objc3::artifacts::frontend
