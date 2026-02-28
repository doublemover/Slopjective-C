from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m209_frontend_pgo_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M209 frontend profile-guided optimization hooks",
        "result.migration_hints.legacy_yes_count",
        "legacy_no_count",
        "legacy_null_count",
        "BuildObjc3AstFromTokens(tokens)",
        "result.stage_diagnostics.parser = std::move(parse_result.diagnostics);",
        "directive_count",
        "duplicate",
        "non_leading",
        '"migration_hints":{"legacy_yes":...,"legacy_no":...,"legacy_null":...,"legacy_total":...}',
        "python -m pytest tests/tooling/test_objc3c_m209_frontend_pgo_contract.py -q",
    ):
        assert text in fragment


def test_m209_frontend_pgo_markers_map_to_sources() -> None:
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    lexer_source = _read(LEXER_SOURCE)

    for marker in (
        "result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;",
        "result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;",
        "result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;",
        "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);",
        "result.stage_diagnostics.parser = std::move(parse_result.diagnostics);",
        "result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;",
        "result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;",
        "result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;",
    ):
        assert marker in pipeline_source

    for marker in (
        'manifest << "    \\"migration_hints\\":{\\"legacy_yes\\":" << pipeline_result.migration_hints.legacy_yes_count',
        '<< ",\\"legacy_no\\":" << pipeline_result.migration_hints.legacy_no_count << ",\\"legacy_null\\":"',
        '<< pipeline_result.migration_hints.legacy_null_count',
        '<< ",\\"legacy_total\\":" << pipeline_result.migration_hints.legacy_total() << "},\\n";',
    ):
        assert marker in artifacts_source

    for marker in (
        "language_version_pragma_contract_.directive_count",
        "language_version_pragma_contract_.duplicate",
        "language_version_pragma_contract_.non_leading",
    ):
        assert marker in lexer_source
