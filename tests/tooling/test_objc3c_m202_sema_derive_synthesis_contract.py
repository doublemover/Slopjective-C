from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMANTIC_PASSES_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m202_sema_derive_synthesis_pipeline_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M202 sema/type derive/synthesis pipeline",
        "derive packet 1.1 deterministic sema integration-surface derive hooks",
        "m202_sema_integration_surface_derive_packet",
        "### 1.1 Deterministic sema integration-surface derive packet",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);",
        "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);",
        "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
        "Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
        "surface.built = true;",
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        "result.integration_surface = std::move(sema_result.integration_surface);",
        "result.sema_parity_surface = sema_result.parity_surface;",
        "synthesis packet 1.2 deterministic type-metadata synthesis hooks",
        "m202_type_metadata_synthesis_packet",
        "### 1.2 Deterministic type-metadata synthesis packet",
        "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {",
        "handoff.global_names_lexicographic.reserve(surface.globals.size());",
        "std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());",
        "std::sort(function_names.begin(), function_names.end());",
        "handoff.functions_lexicographic.reserve(function_names.size());",
        "bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {",
        "return std::all_of(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "IsReadyObjc3SemaParityContractSurface(const Objc3SemaParityContractSurface &surface)",
        "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();",
        "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();",
        "result.parity_surface.ready =",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "python -m pytest tests/tooling/test_objc3c_m202_sema_derive_synthesis_contract.py -q",
    ):
        assert text in fragment


def test_m202_sema_derive_synthesis_markers_map_to_sources() -> None:
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    semantic_passes_source = _read(SEMANTIC_PASSES_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "kObjc3SemaPassOrder",
        "IsReadyObjc3SemaParityContractSurface(const Objc3SemaParityContractSurface &surface)",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);",
        "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);",
        "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();",
        "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();",
        "result.parity_surface.ready =",
    ):
        assert marker in sema_pass_manager_source

    for marker in (
        "Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
        "surface.built = true;",
        "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {",
        "handoff.global_names_lexicographic.reserve(surface.globals.size());",
        "std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());",
        "std::sort(function_names.begin(), function_names.end());",
        "handoff.functions_lexicographic.reserve(function_names.size());",
        "bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {",
        "return std::all_of(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),",
    ):
        assert marker in semantic_passes_source

    for marker in (
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        "result.integration_surface = std::move(sema_result.integration_surface);",
        "result.sema_parity_surface = sema_result.parity_surface;",
    ):
        assert marker in pipeline_source

    for marker in (
        "IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface)",
        '\\"deterministic_type_metadata_handoff\\":',
        '\\"parity_ready\\":',
        '\\"type_metadata_global_entries\\":',
        '\\"type_metadata_function_entries\\":',
        '\\"resolved_global_symbols\\":',
        '\\"resolved_function_symbols\\":',
    ):
        assert marker in artifacts_source
