from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m224_sema_release_readiness_section_present() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M224 sema/type-system release readiness",
        "kObjc3SemaPassOrder",
        "CanonicalizePassDiagnostics(...)",
        "IsMonotonicObjc3SemaDiagnosticsAfterPass(...)",
        "IsReadyObjc3SemaParityContractSurface(...)",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "frontend.pipeline.sema_pass_manager",
        "deterministic_semantic_diagnostics",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "python -m pytest tests/tooling/test_objc3c_m224_sema_release_contract.py -q",
    ):
        assert text in fragment


def test_m224_sema_release_readiness_maps_to_implemented_markers() -> None:
    sema_contract_header = _read(SEMA_CONTRACT_HEADER)
    sema_source = _read(SEMA_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "kObjc3SemaPassOrder" in sema_contract_header
    assert "IsMonotonicObjc3SemaDiagnosticsAfterPass" in sema_contract_header
    assert "IsReadyObjc3SemaParityContractSurface" in sema_contract_header

    assert "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder)" in sema_source
    assert "CanonicalizePassDiagnostics(pass_diagnostics);" in sema_source
    assert "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);" in sema_source

    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline_source

    assert 'manifest << "      \\"sema_pass_manager\\": {\\"diagnostics_after_build\\":"' in artifacts_source
    assert '\\"deterministic_semantic_diagnostics\\":' in artifacts_source
    assert '\\"deterministic_type_metadata_handoff\\":' in artifacts_source
    assert '\\"parity_ready\\":' in artifacts_source
