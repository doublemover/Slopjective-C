from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m200_sema_interop_integration_suite_and_packaging_section_marker_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    assert "## M200 sema/type interop integration suite and packaging" in fragment


def test_m200_sema_interop_source_anchor_phrases_mapped_in_doc_content() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "interop packet 1.1 deterministic sema/type interop architecture anchors",
        "m200_sema_type_interop_architecture_packet",
        "interop packet 1.2 deterministic sema/type interop isolation + packaging anchors",
        "m200_sema_type_interop_isolation_packaging_packet",
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "if (pass == Objc3SemaPassId::BuildIntegrationSurface) {",
        "result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);",
        "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);",
        "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "result.parity_surface.ready =",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        "result.integration_surface = std::move(sema_result.integration_surface);",
        "result.sema_parity_surface = sema_result.parity_surface;",
        "diagnostics_after_build",
        "diagnostics_after_validate_bodies",
        "diagnostics_after_validate_pure_contract",
        "diagnostics_emitted_by_build",
        "diagnostics_emitted_by_validate_bodies",
        "diagnostics_emitted_by_validate_pure_contract",
        "deterministic_semantic_diagnostics",
        "deterministic_type_metadata_handoff",
        "parity_ready",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "function_signature_surface",
        "python -m pytest tests/tooling/test_objc3c_m200_sema_interop_packaging_contract.py -q",
    ):
        assert text in fragment

