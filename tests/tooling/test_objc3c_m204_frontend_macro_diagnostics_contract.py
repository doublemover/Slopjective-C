from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m204_frontend_macro_diagnostics_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M204 frontend macro diagnostics and provenance",
        "MakeDiag(...)",
        "error:<line>:<column>: <message> [<code>]",
        "first_line",
        "first_column",
        "last_line",
        "last_column",
        "result.language_version_pragma_contract.*",
        "frontend.language_version_pragma_contract",
        "O3L005",
        "O3L006",
        "O3L007",
        "O3L008",
        "python -m pytest tests/tooling/test_objc3c_m204_frontend_macro_diagnostics_contract.py -q",
    ):
        assert text in fragment


def test_m204_frontend_macro_diagnostics_markers_map_to_sources() -> None:
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert 'out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";' in lexer_source
    for marker in (
        "language_version_pragma_contract_.first_line = line;",
        "language_version_pragma_contract_.first_column = column;",
        "language_version_pragma_contract_.last_line = line;",
        "language_version_pragma_contract_.last_column = column;",
    ):
        assert marker in lexer_source

    for marker in (
        "result.language_version_pragma_contract.first_line = pragma_contract.first_line;",
        "result.language_version_pragma_contract.first_column = pragma_contract.first_column;",
        "result.language_version_pragma_contract.last_line = pragma_contract.last_line;",
        "result.language_version_pragma_contract.last_column = pragma_contract.last_column;",
    ):
        assert marker in pipeline_source

    for marker in (
        '<< ",\\"first_line\\":" << pipeline_result.language_version_pragma_contract.first_line',
        '<< ",\\"first_column\\":" << pipeline_result.language_version_pragma_contract.first_column',
        '<< ",\\"last_line\\":" << pipeline_result.language_version_pragma_contract.last_line',
        '<< ",\\"last_column\\":" << pipeline_result.language_version_pragma_contract.last_column << "},\\n";',
    ):
        assert marker in artifacts_source
