from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m200_frontend_interop_packaging_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M200 frontend interop integration suite and packaging",
        "ParseMessageSendExpression()",
        "Expr::Kind::MessageSend",
        "selector",
        "args",
        "BuildObjc3AstFromTokens(tokens)",
        "result.program = std::move(parse_result.program);",
        "max_message_send_args = options.lowering.max_message_send_args",
        "python -m pytest tests/tooling/test_objc3c_m200_frontend_interop_packaging_contract.py -q",
    ):
        assert text in fragment


def test_m200_frontend_interop_packaging_markers_map_to_sources() -> None:
    ast_source = _read(AST_SOURCE)
    parser_source = _read(PARSER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)

    for marker in (
        "enum class Kind { Number, BoolLiteral, NilLiteral, Identifier, Binary, Conditional, Call, MessageSend };",
        "std::string selector;",
        "std::vector<std::unique_ptr<Expr>> args;",
    ):
        assert marker in ast_source

    for marker in (
        "if (Match(TokenKind::LBracket)) {",
        "return ParseMessageSendExpression();",
        "message->kind = Expr::Kind::MessageSend;",
        'message->selector += ":";',
        "message->args.push_back(std::move(first_arg));",
    ):
        assert marker in parser_source

    for marker in (
        "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);",
        "result.program = std::move(parse_result.program);",
        "semantic_options.max_message_send_args = options.lowering.max_message_send_args;",
    ):
        assert marker in pipeline_source
