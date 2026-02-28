from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m168_frontend_block_storage_escape_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M168 frontend __block storage and escape parser/AST surface (M168-A001)",
        "BuildBlockStorageEscapeProfile(...)",
        "BuildBlockStorageByrefLayoutSymbol(...)",
        "block_storage_mutable_capture_count",
        "block_storage_escape_to_heap",
        "block_storage_byref_layout_symbol",
        "python -m pytest tests/tooling/test_objc3c_m168_frontend_block_storage_escape_parser_contract.py -q",
    ):
        assert text in fragment


def test_m168_frontend_block_storage_escape_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "std::size_t block_storage_mutable_capture_count = 0;",
        "std::size_t block_storage_byref_slot_count = 0;",
        "bool block_storage_requires_byref_cells = false;",
        "bool block_storage_escape_analysis_enabled = false;",
        "bool block_storage_escape_to_heap = false;",
        "bool block_storage_escape_profile_is_normalized = false;",
        "std::string block_storage_escape_profile;",
        "std::string block_storage_byref_layout_symbol;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildBlockStorageEscapeProfile(",
        "BuildBlockStorageByrefLayoutSymbol(",
        "ParseBlockLiteralExpression(",
        "block->block_storage_mutable_capture_count =",
        "block->block_storage_byref_slot_count =",
        "block->block_storage_requires_byref_cells =",
        "block->block_storage_escape_analysis_enabled = true;",
        "block->block_storage_escape_to_heap =",
        "block->block_storage_escape_profile =",
        "block->block_storage_byref_layout_symbol =",
        "block->block_storage_escape_profile_is_normalized =",
    ):
        assert marker in parser_source
