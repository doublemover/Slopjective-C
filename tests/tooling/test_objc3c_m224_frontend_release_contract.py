from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m224_frontend_release_readiness_section_present() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M224 frontend release-readiness compliance profile",
        "BuildObjc3AstFromTokens(...)",
        "O3L005",
        "O3L006",
        "O3L007",
        "O3L008",
        "Objc3SemaTokenMetadata",
        "python -m pytest tests/tooling/test_objc3c_m224_frontend_release_contract.py -q",
    ):
        assert text in fragment


def test_m224_frontend_release_readiness_maps_to_implemented_markers() -> None:
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    assert '"O3L005"' in lexer_source
    assert '"O3L006"' in lexer_source
    assert '"O3L007"' in lexer_source
    assert '"O3L008"' in lexer_source
    assert "BuildObjc3AstFromTokens(tokens)" in pipeline_source
    assert "language_version_pragma_contract" in pipeline_source
    assert "language_version_pragma_contract" in artifacts_source
