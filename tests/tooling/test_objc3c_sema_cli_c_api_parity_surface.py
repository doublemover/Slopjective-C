from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DIAG_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp"
PIPELINE_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_c_api_and_cli_diagnostics_json_surface_match() -> None:
    frontend_anchor = _read(FRONTEND_ANCHOR)
    diag_artifacts = _read(DIAG_ARTIFACTS)

    assert "ParseFrontendDiagnostic(" in frontend_anchor
    assert '\\"severity\\":\\"' in frontend_anchor
    assert '\\"line\\":' in frontend_anchor
    assert '\\"column\\":' in frontend_anchor
    assert '\\"code\\":\\"' in frontend_anchor
    assert '\\"message\\":\\"' in frontend_anchor
    assert '\\"raw\\":\\"' in frontend_anchor

    assert '\\"severity\\":\\"' in diag_artifacts
    assert '\\"line\\":' in diag_artifacts
    assert '\\"column\\":' in diag_artifacts
    assert '\\"code\\":\\"' in diag_artifacts
    assert '\\"message\\":\\"' in diag_artifacts
    assert '\\"raw\\":\\"' in diag_artifacts


def test_manifest_emits_sema_parity_contract_fields() -> None:
    artifacts = _read(PIPELINE_ARTIFACTS)

    assert "diagnostics_after_build" in artifacts
    assert "diagnostics_after_validate_bodies" in artifacts
    assert "diagnostics_after_validate_pure_contract" in artifacts
    assert "diagnostics_emitted_by_build" in artifacts
    assert "diagnostics_emitted_by_validate_bodies" in artifacts
    assert "diagnostics_emitted_by_validate_pure_contract" in artifacts
    assert "diagnostics_monotonic" in artifacts
    assert "deterministic_type_metadata_handoff" in artifacts
    assert "parity_ready" in artifacts
    assert "type_metadata_global_entries" in artifacts
    assert "type_metadata_function_entries" in artifacts
