from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m210_sema_perf_regression_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M210 sema/type performance budgets and regression gates",
        "budget packet 1.1 deterministic sema pass-manager budget counters",
        "m210_sema_pass_manager_budget_packet",
        "### 1.1 Deterministic sema pass-manager budget packet",
        "kObjc3SemaPassOrder",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "IsMonotonicObjc3SemaDiagnosticsAfterPass(...)",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "frontend.pipeline.sema_pass_manager",
        "diagnostics_after_build",
        "diagnostics_after_validate_bodies",
        "diagnostics_after_validate_pure_contract",
        "diagnostics_emitted_by_build",
        "diagnostics_emitted_by_validate_bodies",
        "diagnostics_emitted_by_validate_pure_contract",
        "budget packet 1.2 deterministic sema/type regression gate anchors",
        "m210_sema_type_regression_gate_packet",
        "### 1.2 Deterministic sema/type regression gate anchor packet",
        "BuildSemanticTypeMetadataHandoff(...)",
        "IsDeterministicSemanticTypeMetadataHandoff(...)",
        "IsReadyObjc3SemaParityContractSurface(...)",
        "result.parity_surface.ready =",
        "diagnostics_monotonic",
        "deterministic_semantic_diagnostics",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "frontend.pipeline.semantic_surface",
        "declared_globals",
        "declared_functions",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "python -m pytest tests/tooling/test_objc3c_m210_sema_perf_regression_contract.py -q",
    ):
        assert text in fragment


def test_m210_sema_perf_regression_markers_map_to_sources() -> None:
    sema_contract_header = _read(SEMA_CONTRACT_HEADER)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "kObjc3SemaPassOrder" in sema_contract_header
    assert "IsMonotonicObjc3SemaDiagnosticsAfterPass" in sema_contract_header
    assert "IsReadyObjc3SemaParityContractSurface" in sema_contract_header

    assert "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();" in sema_source
    assert "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();" in sema_source
    assert "BuildSemanticTypeMetadataHandoff(result.integration_surface);" in sema_source
    assert "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);" in sema_source
    assert "result.parity_surface.ready =" in sema_source
    assert "result.parity_surface.diagnostics_after_pass_monotonic &&" in sema_source
    assert "result.parity_surface.deterministic_semantic_diagnostics &&" in sema_source
    assert "result.parity_surface.deterministic_type_metadata_handoff &&" in sema_source

    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline_source

    for marker in (
        '\\"diagnostics_after_build\\":',
        '\\"diagnostics_after_validate_bodies\\":',
        '\\"diagnostics_after_validate_pure_contract\\":',
        '\\"diagnostics_emitted_by_build\\":',
        '\\"diagnostics_emitted_by_validate_bodies\\":',
        '\\"diagnostics_emitted_by_validate_pure_contract\\":',
        '\\"diagnostics_monotonic\\":',
        '\\"deterministic_semantic_diagnostics\\":',
        '\\"deterministic_type_metadata_handoff\\":',
        '\\"parity_ready\\":',
        '\\"type_metadata_global_entries\\":',
        '\\"type_metadata_function_entries\\":',
        '\\"declared_globals\\":',
        '\\"declared_functions\\":',
        '\\"resolved_global_symbols\\":',
        '\\"resolved_function_symbols\\":',
    ):
        assert marker in artifacts_source
