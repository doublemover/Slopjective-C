from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m210_frontend_perf_regression_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M210 frontend performance budgets and regression gates",
        "std::vector<Objc3LexToken> tokens = lexer.Run(result.stage_diagnostics.lexer);",
        "BuildObjc3AstFromTokens(tokens)",
        "result.stage_diagnostics.parser = std::move(parse_result.diagnostics);",
        '"lexer": {"diagnostics":...}',
        '"parser": {"diagnostics":...}',
        "frontend.language_version_pragma_contract",
        "python -m pytest tests/tooling/test_objc3c_m210_frontend_perf_regression_contract.py -q",
    ):
        assert text in fragment


def test_m210_frontend_perf_regression_markers_map_to_sources() -> None:
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "std::vector<Objc3LexToken> tokens = lexer.Run(result.stage_diagnostics.lexer);" in pipeline_source
    assert "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);" in pipeline_source
    assert "result.stage_diagnostics.parser = std::move(parse_result.diagnostics);" in pipeline_source

    for marker in (
        "ConsumeLanguageVersionPragmas(diagnostics);",
        "language_version_pragma_contract_.directive_count",
        "language_version_pragma_contract_.duplicate",
        "language_version_pragma_contract_.non_leading",
    ):
        assert marker in lexer_source

    for marker in (
        'manifest << "        \\"lexer\\": {\\"diagnostics\\":" << bundle.stage_diagnostics.lexer.size()',
        'manifest << "        \\"parser\\": {\\"diagnostics\\":" << bundle.stage_diagnostics.parser.size()',
        'manifest << "    \\"language_version_pragma_contract\\":{\\"seen\\":"',
    ):
        assert marker in artifacts_source
