from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m209_sema_pgo_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M209 sema/type profile-guided optimization hooks",
        "deterministic sema/type profile-guided optimization (PGO) hooks",
        "pgo hook packet 1.1 deterministic sema diagnostics emission profile",
        "m209_sema_diagnostics_emission_pgo_hook_packet",
        "### 1.1 Deterministic sema diagnostics emission profile hook packet",
        "kObjc3SemaPassOrder",
        "CanonicalizePassDiagnostics(...)",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "frontend.pipeline.sema_pass_manager",
        "diagnostics_emitted_by_build",
        "diagnostics_emitted_by_validate_bodies",
        "diagnostics_emitted_by_validate_pure_contract",
        "diagnostics_monotonic",
        "pgo hook packet 1.2 deterministic type/symbol surface profile",
        "m209_type_symbol_surface_pgo_hook_packet",
        "### 1.2 Deterministic type/symbol surface profile hook packet",
        "BuildSemanticTypeMetadataHandoff(...)",
        "IsDeterministicSemanticTypeMetadataHandoff(...)",
        "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();",
        "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "frontend.pipeline.semantic_surface",
        "declared_globals",
        "declared_functions",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "function_signature_surface",
        "scalar_return_i32",
        "scalar_return_bool",
        "scalar_return_void",
        "scalar_param_i32",
        "scalar_param_bool",
        "python -m pytest tests/tooling/test_objc3c_m209_sema_pgo_contract.py -q",
    ):
        assert text in fragment


def test_m209_sema_pgo_markers_map_to_source_anchors() -> None:
    sema_contract_header = _read(SEMA_CONTRACT_HEADER)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "kObjc3SemaPassOrder" in sema_contract_header

    assert "CanonicalizePassDiagnostics(pass_diagnostics);" in sema_source
    assert "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();" in sema_source
    assert "result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;" in sema_source
    assert "BuildSemanticTypeMetadataHandoff(result.integration_surface);" in sema_source
    assert "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);" in sema_source
    assert (
        "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();"
        in sema_source
    )
    assert (
        "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();"
        in sema_source
    )

    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline_source

    for marker in (
        '\\"diagnostics_emitted_by_build\\":',
        '\\"diagnostics_emitted_by_validate_bodies\\":',
        '\\"diagnostics_emitted_by_validate_pure_contract\\":',
        '\\"diagnostics_monotonic\\":',
        '\\"deterministic_type_metadata_handoff\\":',
        '\\"parity_ready\\":',
        '\\"type_metadata_global_entries\\":',
        '\\"type_metadata_function_entries\\":',
        '\\"semantic_surface\\":',
        '\\"declared_globals\\":',
        '\\"declared_functions\\":',
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
