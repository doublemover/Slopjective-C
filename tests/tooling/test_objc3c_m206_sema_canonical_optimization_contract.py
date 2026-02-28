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


def test_m206_sema_canonical_optimization_stage1_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M206 sema/type canonical optimization pipeline stage-1",
        "stage-1 packet 1.1 deterministic canonical sema diagnostics ordering hooks",
        "m206_canonical_sema_diagnostics_stage1_packet",
        "### 1.1 Deterministic canonical sema diagnostics ordering packet",
        "Objc3SemaCompatibilityMode::Canonical",
        "kObjc3SemaPassOrder",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "IsCanonicalPassDiagnostics(pass_diagnostics);",
        "std::stable_sort(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);",
        "result.deterministic_semantic_diagnostics = deterministic_semantic_diagnostics;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "compatibility_mode",
        "frontend.pipeline.sema_pass_manager",
        "deterministic_semantic_diagnostics",
        "diagnostics_monotonic",
        "stage-1 packet 1.2 deterministic canonical type-metadata handoff hooks",
        "m206_canonical_type_metadata_stage1_packet",
        "### 1.2 Deterministic canonical type-metadata handoff packet",
        "BuildSemanticTypeMetadataHandoff(...)",
        "IsDeterministicSemanticTypeMetadataHandoff(...)",
        "std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());",
        "std::sort(function_names.begin(), function_names.end());",
        "std::is_sorted(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end())",
        "std::all_of(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),",
        "result.deterministic_type_metadata_handoff =",
        "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();",
        "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();",
        "IsReadyObjc3SemaParityContractSurface(...)",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "function_signature_surface",
        "scalar_return_i32",
        "scalar_return_bool",
        "scalar_return_void",
        "scalar_param_i32",
        "scalar_param_bool",
        "python -m pytest tests/tooling/test_objc3c_m206_sema_canonical_optimization_contract.py -q",
    ):
        assert text in fragment


def test_m206_sema_canonical_optimization_stage1_markers_map_to_sources() -> None:
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    semantic_passes_source = _read(SEMANTIC_PASSES_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "kObjc3SemaPassOrder",
        "IsReadyObjc3SemaParityContractSurface",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "Objc3SemaCompatibilityMode::Canonical",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "IsCanonicalPassDiagnostics(pass_diagnostics);",
        "std::stable_sort(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);",
        "result.deterministic_semantic_diagnostics = deterministic_semantic_diagnostics;",
        "result.deterministic_type_metadata_handoff =",
        "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();",
        "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();",
    ):
        assert marker in sema_pass_manager_source

    for marker in (
        "std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());",
        "std::sort(function_names.begin(), function_names.end());",
        "std::is_sorted(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end())",
        "std::all_of(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),",
        "IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff)",
    ):
        assert marker in semantic_passes_source

    for marker in (
        "Objc3SemaCompatibilityMode::Canonical;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
    ):
        assert marker in pipeline_source

    for marker in (
        '\\"compatibility_mode\\":\\"',
        '\\"diagnostics_monotonic\\":',
        '\\"deterministic_semantic_diagnostics\\":',
        '\\"deterministic_type_metadata_handoff\\":',
        '\\"parity_ready\\":',
        '\\"type_metadata_global_entries\\":',
        '\\"type_metadata_function_entries\\":',
        '\\"resolved_global_symbols\\":',
        '\\"resolved_function_symbols\\":',
        '\\"function_signature_surface\\":',
        '\\"scalar_return_i32\\":',
        '\\"scalar_return_bool\\":',
        '\\"scalar_return_void\\":',
        '\\"scalar_param_i32\\":',
        '\\"scalar_param_bool\\":',
    ):
        assert marker in artifacts_source
