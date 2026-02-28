from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m218_frontend_rc_provenance_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M218 frontend RC provenance packet",
        "O3L005",
        "O3L006",
        "O3L007",
        "O3L008",
        "BuildObjc3AstFromTokens(...)",
        "frontend.language_version_pragma_contract",
        "Objc3SemaTokenMetadata",
        "python -m pytest tests/tooling/test_objc3c_m218_frontend_rc_provenance_contract.py -q",
    ):
        assert text in fragment


def test_m218_frontend_rc_provenance_markers_map_to_sources() -> None:
    lexer_source = _read(LEXER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ast_source = _read(AST_SOURCE)

    for code in ("O3L005", "O3L006", "O3L007", "O3L008"):
        assert f'"{code}"' in lexer_source
    assert "BuildObjc3AstFromTokens(tokens)" in pipeline_source
    assert "language_version_pragma_contract" in pipeline_source
    assert "language_version_pragma_contract" in artifacts_source
    assert "Objc3SemaTokenMetadata" in ast_source
