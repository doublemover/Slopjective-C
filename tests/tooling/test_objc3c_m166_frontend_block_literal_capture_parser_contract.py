from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m166_frontend_block_literal_capture_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M166 frontend block literal syntax and capture-set parser/AST surface",
        "ParseBlockLiteralExpression(...)",
        "BuildBlockLiteralCaptureSet(...)",
        "Expr::Kind::BlockLiteral",
        "block_capture_names_lexicographic",
        "block_capture_profile",
        "python -m pytest tests/tooling/test_objc3c_m166_frontend_block_literal_capture_parser_contract.py -q",
    ):
        assert text in fragment


def test_m166_frontend_block_literal_capture_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "BlockLiteral",
        "std::vector<std::string> block_parameter_names_lexicographic;",
        "std::size_t block_parameter_count = 0;",
        "std::vector<std::string> block_capture_names_lexicographic;",
        "std::size_t block_capture_count = 0;",
        "std::size_t block_body_statement_count = 0;",
        "std::string block_capture_profile;",
        "bool block_capture_set_deterministic = false;",
        "bool block_literal_is_normalized = false;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildBlockLiteralCaptureProfile(",
        "CollectBlockLiteralExprIdentifiers(",
        "CollectBlockLiteralStmtIdentifiers(",
        "BuildBlockLiteralCaptureSet(",
        "ParseBlockLiteralExpression(",
        "block->kind = Expr::Kind::BlockLiteral;",
        "block->block_capture_names_lexicographic =",
        "block->block_capture_profile =",
        "if (Match(TokenKind::Caret)) {",
    ):
        assert marker in parser_source
