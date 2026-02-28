from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m202_frontend_derive_synthesis_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M202 frontend derive/synthesis pipeline",
        "BuildSemanticIntegrationSurface(...)",
        "BuildSemanticTypeMetadataHandoff(...)",
        "IsDeterministicSemanticTypeMetadataHandoff(...)",
        "global_names_lexicographic",
        "functions_lexicographic",
        "python -m pytest tests/tooling/test_objc3c_m202_frontend_derive_synthesis_contract.py -q",
    ):
        assert text in fragment


def test_m202_frontend_derive_synthesis_markers_map_to_sources() -> None:
    sema_source = _read(SEMA_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
        "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface)",
        "bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff)",
        "handoff.global_names_lexicographic.reserve(surface.globals.size());",
        "handoff.functions_lexicographic.reserve(function_names.size());",
    ):
        assert marker in sema_source

    for marker in (
        'manifest << "      \\"semantic_surface\\": {\\"declared_globals\\":" << program.globals.size()',
        '<< ",\\"declared_functions\\":" << manifest_functions.size()',
        '<< ",\\"resolved_global_symbols\\":" << pipeline_result.integration_surface.globals.size()',
        '<< ",\\"resolved_function_symbols\\":" << pipeline_result.integration_surface.functions.size()',
    ):
        assert marker in artifacts_source
