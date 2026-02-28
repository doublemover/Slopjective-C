from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_AGGREGATE = ROOT / "docs" / "objc3c-native.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m223_frontend_docs_capture_language_version_pragma_contract() -> None:
    grammar_fragment = _read(GRAMMAR_DOC_FRAGMENT)
    aggregate_doc = _read(DOC_AGGREGATE)

    for text in (
        "## Language-version pragma prelude contract",
        "#pragma objc_language_version(3)",
        "non-leading directives emit deterministic `O3L008`",
        "duplicates emit deterministic `O3L007`",
        "Malformed directives emit deterministic `O3L005`",
        "Unsupported version payloads emit deterministic `O3L006`",
        "`language_version_pragma_contract` packet",
        "`directive_count`",
        "`duplicate`",
        "`non_leading`",
    ):
        assert text in grammar_fragment
        assert text in aggregate_doc


def test_m223_frontend_docs_contract_matches_lexer_and_manifest_surfaces() -> None:
    lexer_source = _read(LEXER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    assert '"O3L005"' in lexer_source
    assert '"O3L006"' in lexer_source
    assert '"O3L007"' in lexer_source
    assert '"O3L008"' in lexer_source
    assert "language_version_pragma_contract" in artifacts_source
    assert "pipeline_result.language_version_pragma_contract.directive_count" in artifacts_source
