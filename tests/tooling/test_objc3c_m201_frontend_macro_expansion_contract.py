from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m201_frontend_macro_expansion_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M201 frontend macro expansion architecture and isolation",
        "migration_hints_.legacy_yes_count",
        "legacy_no_count",
        "legacy_null_count",
        "language_version_pragma_contract_",
        "result.migration_hints.*",
        "result.language_version_pragma_contract.*",
        "sema_input.migration_hints.*",
        'append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true")',
        "python -m pytest tests/tooling/test_objc3c_m201_frontend_macro_expansion_contract.py -q",
    ):
        assert text in fragment


def test_m201_frontend_macro_expansion_markers_map_to_sources() -> None:
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    sema_source = _read(SEMA_PASS_MANAGER_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "++migration_hints_.legacy_yes_count;",
        "++migration_hints_.legacy_no_count;",
        "++migration_hints_.legacy_null_count;",
        "language_version_pragma_contract_.directive_count",
    ):
        assert marker in lexer_source

    for marker in (
        "result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;",
        "result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;",
        "result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;",
        "result.language_version_pragma_contract.seen = pragma_contract.seen;",
        "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;",
        "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;",
        "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;",
    ):
        assert marker in pipeline_source

    for marker in (
        'append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");',
        'append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");',
        'append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");',
    ):
        assert marker in sema_source

    for marker in (
        'manifest << "    \\"migration_hints\\":{\\"legacy_yes\\":" << pipeline_result.migration_hints.legacy_yes_count',
        '<< ",\\"legacy_no\\":" << pipeline_result.migration_hints.legacy_no_count << ",\\"legacy_null\\":"',
        '<< pipeline_result.migration_hints.legacy_null_count',
    ):
        assert marker in artifacts_source
