from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m207_frontend_dispatch_optimizations_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M207 frontend dispatch-specific optimization passes",
        "ParseMessageSendExpression()",
        "message->kind = Expr::Kind::MessageSend",
        "if (Match(TokenKind::LBracket))",
        '"runtime_dispatch_symbol"',
        '"runtime_dispatch_arg_slots"',
        '"selector_global_ordering"',
        '"max_message_send_args"',
        "python -m pytest tests/tooling/test_objc3c_m207_frontend_dispatch_optimizations_contract.py -q",
    ):
        assert text in fragment


def test_m207_frontend_dispatch_optimizations_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)

    for marker in (
        "if (Match(TokenKind::LBracket)) {",
        "return ParseMessageSendExpression();",
        "std::unique_ptr<Expr> ParseMessageSendExpression() {",
        "message->kind = Expr::Kind::MessageSend;",
    ):
        assert marker in parser_source

    for marker in (
        'manifest << "    \\"max_message_send_args\\":" << options.lowering.max_message_send_args << ",\\n";',
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
    ):
        assert marker in artifacts_source
