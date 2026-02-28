from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m167_frontend_block_abi_invoke_trampoline_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M167 frontend block ABI invoke-trampoline parser/AST surface (M167-A001)",
        "BuildBlockLiteralAbiLayoutProfile(...)",
        "BuildBlockLiteralAbiDescriptorSymbol(...)",
        "BuildBlockLiteralInvokeTrampolineSymbol(...)",
        "block_abi_layout_profile",
        "block_invoke_trampoline_symbol",
        "python -m pytest tests/tooling/test_objc3c_m167_frontend_block_abi_invoke_trampoline_parser_contract.py -q",
    ):
        assert text in fragment


def test_m167_frontend_block_abi_invoke_trampoline_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "std::size_t block_abi_invoke_argument_slots = 0;",
        "std::size_t block_abi_capture_word_count = 0;",
        "std::string block_abi_layout_profile;",
        "std::string block_abi_descriptor_symbol;",
        "std::string block_invoke_trampoline_symbol;",
        "bool block_abi_has_invoke_trampoline = false;",
        "bool block_abi_layout_is_normalized = false;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildBlockLiteralAbiLayoutProfile(",
        "BuildBlockLiteralAbiDescriptorSymbol(",
        "BuildBlockLiteralInvokeTrampolineSymbol(",
        "ParseBlockLiteralExpression(",
        "block->block_abi_invoke_argument_slots =",
        "block->block_abi_capture_word_count =",
        "block->block_abi_layout_profile =",
        "block->block_abi_descriptor_symbol =",
        "block->block_invoke_trampoline_symbol =",
        "block->block_abi_has_invoke_trampoline = true;",
        "block->block_abi_layout_is_normalized =",
    ):
        assert marker in parser_source
