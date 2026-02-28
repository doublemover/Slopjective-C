from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m156_frontend_message_send_selector_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M156 frontend message-send selector-lowering parser surface",
        "BuildMessageSendFormSymbol(...)",
        "BuildMessageSendSelectorLoweringSymbol(...)",
        "message->message_send_form = Expr::MessageSendForm::Keyword;",
        "message->message_send_form = Expr::MessageSendForm::Unary;",
        "message->message_send_form_symbol = BuildMessageSendFormSymbol(message->message_send_form);",
        "message->selector_lowering_symbol = BuildMessageSendSelectorLoweringSymbol(message->selector_lowering_pieces);",
        "message->selector_lowering_is_normalized = true;",
        "python -m pytest tests/tooling/test_objc3c_m156_frontend_message_send_selector_lowering_contract.py -q",
    ):
        assert text in fragment


def test_m156_frontend_message_send_selector_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "static std::string BuildMessageSendFormSymbol(",
        "static std::string BuildMessageSendSelectorLoweringSymbol(",
        'return "message-send-form:unary";',
        'return "message-send-form:keyword";',
        'return "message-send-form:none";',
        'return "selector-lowering:" + normalized_selector;',
        "message->message_send_form = Expr::MessageSendForm::Keyword;",
        "message->message_send_form = Expr::MessageSendForm::Unary;",
        "message->selector_lowering_pieces.push_back(head_piece);",
        "message->selector_lowering_pieces.push_back(std::move(keyword_piece));",
        "message->message_send_form_symbol = BuildMessageSendFormSymbol(message->message_send_form);",
        "message->selector_lowering_symbol = BuildMessageSendSelectorLoweringSymbol(message->selector_lowering_pieces);",
        "message->selector_lowering_is_normalized = true;",
    ):
        assert marker in parser_source

    for marker in (
        "enum class MessageSendForm { None, Unary, Keyword };",
        "struct MessageSendSelectorPiece {",
        "MessageSendForm message_send_form = MessageSendForm::None;",
        "std::string message_send_form_symbol;",
        "std::vector<MessageSendSelectorPiece> selector_lowering_pieces;",
        "std::string selector_lowering_symbol;",
        "bool selector_lowering_is_normalized = false;",
    ):
        assert marker in ast_source
