from __future__ import annotations

from objc3c_sema_pass_manager_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_in_order,
)


def assert_pass_manager_contract_exposes_pass_order_and_diagnostics_bus(
    contract: str,
) -> None:
    assert_contains_all(
        contract,
        [
            "kObjc3SemaPassManagerContractVersionMajor",
            "enum class Objc3SemaPassId {",
            "enum class Objc3SemaLanguageProfile : std::uint8_t {",
            "struct Objc3SemaCanonicalLiteralRejectionCounts {",
            "BuildIntegrationSurface",
            "ValidateBodies",
            "ValidatePureContract",
            "kObjc3SemaPassOrder",
            "IsMonotonicObjc3SemaDiagnosticsAfterPass(",
            "struct Objc3SemaDiagnosticsBus {",
            "PublishBatch(const std::vector<std::string> &batch) const",
            "std::size_t Count() const",
            "Objc3SemaLanguageProfile language_profile = Objc3SemaLanguageProfile::Canonical;",
            "Objc3SemaCanonicalLiteralRejectionCounts canonical_literal_rejection_counts;",
            "std::vector<std::string> diagnostics;",
            "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};",
            "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;",
            "bool deterministic_semantic_diagnostics = false;",
            "bool deterministic_type_metadata_handoff = false;",
            "struct Objc3SemaParityContractSurface {",
            "bool IsReadyObjc3SemaParityContractSurface(",
            "Objc3SemaParityContractSurface parity_surface;",
        ],
    )
    assert "Legacy = 1" not in contract

    assert_in_order(
        contract,
        [
            "Objc3SemaPassId::BuildIntegrationSurface,",
            "Objc3SemaPassId::ValidateBodies,",
            "Objc3SemaPassId::ValidatePureContract,",
        ],
    )

    assert_in_order(
        contract,
        [
            "std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};",
            "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};",
            "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;\n  bool deterministic_semantic_diagnostics = false;",
            "Objc3SemaParityContractSurface parity_surface;",
        ],
    )


def assert_pass_manager_module_orchestrates_semantic_passes(
    header: str,
    source: str,
) -> None:
    assert "RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input);" in header
    assert_contains_all(
        source,
        [
            "RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input)",
            "BuildSemanticIntegrationSurface(",
            "*input.program,",
            "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);",
            "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
            "result.diagnostics.insert(result.diagnostics.end(), pass_diagnostics.begin(), pass_diagnostics.end());",
            "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
            "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
            "result.diagnostics_emitted_by_pass[pass_index] = pass_diagnostics.size();",
            "CanonicalizePassDiagnostics(pass_diagnostics);",
            "result.deterministic_semantic_diagnostics =",
            "deterministic_semantic_diagnostics &&",
            "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
            "result.deterministic_type_metadata_handoff =",
            "result.parity_surface.diagnostics_after_pass = result.diagnostics_after_pass;",
            "result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;",
            "result.parity_surface.diagnostics_after_pass_monotonic =",
            "result.parity_surface.deterministic_semantic_diagnostics = result.deterministic_semantic_diagnostics;",
            "result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;",
            "result.parity_surface.ready =",
        ],
    )
    assert_excludes_all(source, ["AppendMigrationAssistDiagnostics", "O3S216"])

    assert_in_order(
        source,
        [
            "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
            "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
            "CanonicalizePassDiagnostics(pass_diagnostics);",
            "result.diagnostics.insert(result.diagnostics.end(), pass_diagnostics.begin(), pass_diagnostics.end());",
            "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
            "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
            "result.diagnostics_emitted_by_pass[pass_index] = pass_diagnostics.size();",
            "result.deterministic_semantic_diagnostics =",
            "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
            "result.deterministic_type_metadata_handoff =",
            "result.parity_surface.diagnostics_after_pass = result.diagnostics_after_pass;",
            "result.parity_surface.diagnostics_after_pass_monotonic =",
            "result.parity_surface.ready =",
        ],
    )


def assert_pipeline_uses_pass_manager_and_diagnostics_bus(pipeline: str) -> None:
    assert_contains_all(
        pipeline,
        [
            '#include "sema/objc3_sema_pass_manager.h"',
            "Objc3SemaPassManagerInput sema_input;",
            "sema_input.language_profile = Objc3SemaLanguageProfile::Canonical;",
            "sema_input.canonical_literal_rejection_counts.yes_literal_sites =",
            "result.canonical_literal_rejection_counts.yes_literal_sites;",
            "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
            "RunObjc3SemaPassManager(sema_input)",
        ],
    )
    assert_excludes_all(
        pipeline,
        [
            "BuildSemanticIntegrationSurface(result.program, result.stage_diagnostics.semantic);",
            "ValidateSemanticBodies(result.program, result.integration_surface, semantic_options,",
            "ValidatePureContractSemanticDiagnostics(result.program, result.integration_surface.functions,",
        ],
    )


def assert_build_surfaces_register_pass_manager_source(
    cmake: str,
    build_script: str,
) -> None:
    assert_contains_all(
        cmake,
        [
            "objc3_sema_pass_manager.cpp",
            "objc3c_diag",
            "add_library(objc3c_sema_type_system INTERFACE)",
            "target_link_libraries(objc3c_sema_type_system INTERFACE",
            "objc3c_sema_type_system",
        ],
    )

    assert_in_order(
        cmake,
        [
            "add_library(objc3c_sema STATIC",
            "objc3_sema_pass_manager_contract_flow.h",
            "objc3_sema_pass_manager.cpp",
            "target_link_libraries(objc3c_sema PUBLIC",
            "add_library(objc3c_sema_type_system INTERFACE)",
            "target_link_libraries(objc3c_sema_type_system INTERFACE",
        ],
    )

    assert '"native/objc3c/src/sema/objc3_sema_pass_manager.cpp"' in build_script
