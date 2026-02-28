from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m214_sema_daemonized_compiler_profile_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M214 sema/type daemonized compiler profile",
        "stable order: `daemon-bootstrap`, `watch-incremental`, `watch-replay`",
        "### 1.1 Deterministic sema diagnostics daemonized packet",
        "kObjc3SemaPassOrder",
        "CanonicalizePassDiagnostics(...)",
        "IsMonotonicObjc3SemaDiagnosticsAfterPass(...)",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "frontend.pipeline.sema_pass_manager",
        "diagnostics_after_build",
        "diagnostics_after_validate_bodies",
        "diagnostics_after_validate_pure_contract",
        "deterministic_semantic_diagnostics",
        "m214_daemon_bootstrap_sema_diagnostics_packet",
        "m214_watch_incremental_sema_diagnostics_packet",
        "m214_watch_replay_sema_diagnostics_packet",
        "### 1.2 Deterministic type-metadata handoff daemonized packet",
        "BuildSemanticTypeMetadataHandoff(...)",
        "IsDeterministicSemanticTypeMetadataHandoff(...)",
        "IsReadyObjc3SemaParityContractSurface(...)",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "frontend.pipeline.semantic_surface",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "function_signature_surface",
        "scalar_return_i32",
        "scalar_return_bool",
        "scalar_return_void",
        "scalar_param_i32",
        "scalar_param_bool",
        "m214_daemon_bootstrap_type_metadata_handoff_packet",
        "m214_watch_incremental_type_metadata_handoff_packet",
        "m214_watch_replay_type_metadata_handoff_packet",
        "python -m pytest tests/tooling/test_objc3c_m214_sema_daemonized_contract.py -q",
    ):
        assert text in fragment


def test_m214_sema_daemonized_compiler_profile_maps_to_source_anchors() -> None:
    sema_contract_header = _read(SEMA_CONTRACT_HEADER)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "kObjc3SemaPassOrder" in sema_contract_header
    assert "IsMonotonicObjc3SemaDiagnosticsAfterPass" in sema_contract_header
    assert "IsReadyObjc3SemaParityContractSurface" in sema_contract_header

    assert "CanonicalizePassDiagnostics(pass_diagnostics);" in sema_source
    assert "BuildSemanticTypeMetadataHandoff(result.integration_surface);" in sema_source
    assert "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);" in sema_source

    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline_source

    for marker in (
        '\\"diagnostics_after_build\\":',
        '\\"diagnostics_after_validate_bodies\\":',
        '\\"diagnostics_after_validate_pure_contract\\":',
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
