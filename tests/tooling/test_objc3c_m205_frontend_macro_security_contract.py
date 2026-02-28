from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m205_frontend_macro_security_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M205 frontend macro security policy enforcement",
        "ConsumeLanguageVersionPragmas(diagnostics)",
        "ConsumeLanguageVersionPragmaDirective(...)",
        "LanguageVersionPragmaPlacement::kNonLeading",
        "O3L005",
        "O3L006",
        "O3L007",
        "O3L008",
        "frontend.language_version_pragma_contract",
        "directive_count",
        "duplicate",
        "non_leading",
        "python -m pytest tests/tooling/test_objc3c_m205_frontend_macro_security_contract.py -q",
    ):
        assert text in fragment


def test_m205_frontend_macro_security_markers_map_to_sources() -> None:
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert "ConsumeLanguageVersionPragmas(diagnostics);" in lexer_source
    assert "ConsumeLanguageVersionPragmaDirective(" in lexer_source
    assert "LanguageVersionPragmaPlacement::kNonLeading" in lexer_source
    for code in ("O3L005", "O3L006", "O3L007", "O3L008"):
        assert f'"{code}"' in lexer_source

    for marker in (
        "result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;",
        "result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;",
        "result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;",
    ):
        assert marker in pipeline_source

    for marker in (
        'manifest << "    \\"language_version_pragma_contract\\":{\\"seen\\":"',
        '<< ",\\"directive_count\\":" << pipeline_result.language_version_pragma_contract.directive_count',
        '<< ",\\"duplicate\\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")',
        '<< ",\\"non_leading\\":"',
    ):
        assert marker in artifacts_source
