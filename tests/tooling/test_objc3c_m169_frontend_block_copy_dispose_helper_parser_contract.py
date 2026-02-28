from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m169_frontend_block_copy_dispose_helper_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M169 frontend block copy/dispose helper parser/AST surface (M169-A001)",
        "BuildBlockCopyDisposeProfile(...)",
        "BuildBlockCopyHelperSymbol(...)",
        "BuildBlockDisposeHelperSymbol(...)",
        "block_copy_helper_required",
        "block_dispose_helper_required",
        "block_copy_helper_symbol",
        "block_dispose_helper_symbol",
        "python -m pytest tests/tooling/test_objc3c_m169_frontend_block_copy_dispose_helper_parser_contract.py -q",
    ):
        assert text in fragment


def test_m169_frontend_block_copy_dispose_helper_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool block_copy_helper_required = false;",
        "bool block_dispose_helper_required = false;",
        "bool block_copy_dispose_profile_is_normalized = false;",
        "std::string block_copy_dispose_profile;",
        "std::string block_copy_helper_symbol;",
        "std::string block_dispose_helper_symbol;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildBlockCopyDisposeProfile(",
        "BuildBlockCopyHelperSymbol(",
        "BuildBlockDisposeHelperSymbol(",
        "ParseBlockLiteralExpression(",
        "block->block_copy_helper_required =",
        "block->block_dispose_helper_required =",
        "block->block_copy_dispose_profile =",
        "block->block_copy_helper_symbol =",
        "block->block_dispose_helper_symbol =",
        "block->block_copy_dispose_profile_is_normalized =",
    ):
        assert marker in parser_source
