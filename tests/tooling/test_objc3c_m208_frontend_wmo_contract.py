from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m208_frontend_wmo_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M208 frontend whole-module optimization controls",
        "BuildObjc3AstFromTokens(tokens)",
        "const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);",
        "manifest_functions.reserve(program.functions.size())",
        "std::unordered_set<std::string> manifest_function_names",
        '"declared_globals"',
        '"declared_functions"',
        '"resolved_global_symbols"',
        '"resolved_function_symbols"',
        "python -m pytest tests/tooling/test_objc3c_m208_frontend_wmo_contract.py -q",
    ):
        assert text in fragment


def test_m208_frontend_wmo_markers_map_to_sources() -> None:
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);" in pipeline_source
    assert "const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);" in artifacts_source
    assert "manifest_functions.reserve(program.functions.size());" in artifacts_source
    assert "std::unordered_set<std::string> manifest_function_names;" in artifacts_source

    for marker in (
        'manifest << "      \\"semantic_surface\\": {\\"declared_globals\\":" << program.globals.size()',
        '<< ",\\"declared_functions\\":" << manifest_functions.size()',
        '<< ",\\"resolved_global_symbols\\":" << pipeline_result.integration_surface.globals.size()',
        '<< ",\\"resolved_function_symbols\\":" << pipeline_result.integration_surface.functions.size()',
    ):
        assert marker in artifacts_source
