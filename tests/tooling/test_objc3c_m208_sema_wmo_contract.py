from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m208_sema_wmo_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M208 sema/type whole-module optimization controls",
        "deterministic sema/type whole-module optimization (WMO) controls",
        "wmo control packet 1.1 deterministic sema pass-order + integration surface",
        "m208_sema_pass_order_wmo_control_packet",
        "### 1.1 Deterministic sema pass-order + integration surface control packet",
        "kObjc3SemaPassOrder",
        "BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);",
        "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);",
        "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
        "CanonicalizePassDiagnostics(...)",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "wmo control packet 1.2 deterministic type/symbol module-shape parity",
        "m208_type_symbol_module_shape_wmo_control_packet",
        "### 1.2 Deterministic type/symbol module-shape parity control packet",
        "BuildSemanticTypeMetadataHandoff(...)",
        "result.parity_surface.globals_total = result.integration_surface.globals.size();",
        "result.parity_surface.functions_total = result.integration_surface.functions.size();",
        "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();",
        "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();",
        "IsReadyObjc3SemaParityContractSurface(...)",
        "frontend.pipeline.sema_pass_manager",
        "parity_ready",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "frontend.pipeline.semantic_surface",
        "declared_globals",
        "declared_functions",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "python -m pytest tests/tooling/test_objc3c_m208_sema_wmo_contract.py -q",
    ):
        assert text in fragment


def test_m208_sema_wmo_markers_map_to_source_anchors() -> None:
    sema_contract_header = _read(SEMA_CONTRACT_HEADER)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "kObjc3SemaPassOrder" in sema_contract_header
    assert "IsReadyObjc3SemaParityContractSurface" in sema_contract_header

    assert "result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);" in sema_source
    assert "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);" in sema_source
    assert (
        "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);"
        in sema_source
    )
    assert "CanonicalizePassDiagnostics(pass_diagnostics);" in sema_source
    assert "BuildSemanticTypeMetadataHandoff(result.integration_surface);" in sema_source
    assert "result.parity_surface.globals_total = result.integration_surface.globals.size();" in sema_source
    assert "result.parity_surface.functions_total = result.integration_surface.functions.size();" in sema_source
    assert "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();" in sema_source
    assert (
        "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();"
        in sema_source
    )

    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline_source

    for marker in (
        '\\"parity_ready\\":',
        '\\"type_metadata_global_entries\\":',
        '\\"type_metadata_function_entries\\":',
        '\\"semantic_surface\\":',
        '\\"declared_globals\\":',
        '\\"declared_functions\\":',
        '\\"resolved_global_symbols\\":',
        '\\"resolved_function_symbols\\":',
    ):
        assert marker in artifacts_source
