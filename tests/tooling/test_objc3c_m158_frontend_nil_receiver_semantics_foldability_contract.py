from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m158_frontend_nil_receiver_semantics_foldability_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M158 frontend nil-receiver semantics/foldability parser/AST surface",
        "BuildNilReceiverFoldingSymbol(...)",
        "message->nil_receiver_semantics_enabled = message->receiver->kind == Expr::Kind::NilLiteral;",
        "message->nil_receiver_foldable = message->nil_receiver_semantics_enabled;",
        "message->nil_receiver_requires_runtime_dispatch = !message->nil_receiver_foldable;",
        "message->nil_receiver_folding_symbol = BuildNilReceiverFoldingSymbol(...)",
        "message->nil_receiver_semantics_is_normalized = true;",
        "python -m pytest tests/tooling/test_objc3c_m158_frontend_nil_receiver_semantics_foldability_contract.py -q",
    ):
        assert text in fragment


def test_m158_frontend_nil_receiver_semantics_foldability_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "static std::string BuildNilReceiverFoldingSymbol(",
        'out << "nil-receiver:foldable=" << (nil_receiver_foldable ? "true" : "false")',
        '<< ";runtime-dispatch=" << (requires_runtime_dispatch ? "required" : "elided")',
        "message->nil_receiver_semantics_enabled = message->receiver->kind == Expr::Kind::NilLiteral;",
        "message->nil_receiver_foldable = message->nil_receiver_semantics_enabled;",
        "message->nil_receiver_requires_runtime_dispatch = !message->nil_receiver_foldable;",
        "message->nil_receiver_folding_symbol = BuildNilReceiverFoldingSymbol(",
        "message->nil_receiver_semantics_is_normalized = true;",
    ):
        assert marker in parser_source

    for marker in (
        "bool nil_receiver_semantics_enabled = false;",
        "bool nil_receiver_foldable = false;",
        "bool nil_receiver_requires_runtime_dispatch = true;",
        "std::string nil_receiver_folding_symbol;",
        "bool nil_receiver_semantics_is_normalized = false;",
    ):
        assert marker in ast_source
