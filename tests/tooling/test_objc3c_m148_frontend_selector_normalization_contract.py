from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m148_frontend_selector_normalization_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M148 frontend selector-normalized method declaration grammar",
        "BuildNormalizedObjcSelector(...)",
        "ParseObjcMethodDecl(...)",
        "selector_pieces",
        "selector_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m148_frontend_selector_normalization_contract.py -q",
    ):
        assert text in fragment


def test_m148_frontend_selector_normalization_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "BuildNormalizedObjcSelector(const std::vector<Objc3MethodDecl::SelectorPiece> &pieces)",
        "method.selector_pieces.push_back(std::move(head_piece));",
        "method.selector = BuildNormalizedObjcSelector(method.selector_pieces);",
        "method.selector_is_normalized = true;",
    ):
        assert marker in parser_source

    for marker in (
        "struct SelectorPiece",
        "std::vector<SelectorPiece> selector_pieces;",
        "bool selector_is_normalized = false;",
        "std::string parameter_name;",
    ):
        assert marker in ast_source
