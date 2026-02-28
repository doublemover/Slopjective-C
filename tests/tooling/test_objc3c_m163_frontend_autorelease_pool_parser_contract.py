from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
TOKEN_CONTRACT = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m163_frontend_autorelease_pool_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M163 frontend autorelease-pool scope grammar surface",
        "Objc3LexTokenKind::KwAtAutoreleasePool",
        "Match(TokenKind::KwAtAutoreleasePool)",
        "BuildAutoreleasePoolScopeSymbol(...)",
        "stmt->block_stmt->is_autoreleasepool_scope",
        "stmt->block_stmt->autoreleasepool_scope_symbol",
        "stmt->block_stmt->autoreleasepool_scope_depth",
        "`At(TokenKind::KwAtAutoreleasePool)` in `SynchronizeStatement()`",
        "python -m pytest tests/tooling/test_objc3c_m163_frontend_autorelease_pool_parser_contract.py -q",
    ):
        assert text in fragment


def test_m163_frontend_autorelease_pool_markers_map_to_sources() -> None:
    token_contract = _read(TOKEN_CONTRACT)
    lexer_source = _read(LEXER_SOURCE)
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "KwAtAutoreleasePool",
    ):
        assert marker in token_contract

    for marker in (
        'if (directive == "autoreleasepool") {',
        "TokenKind::KwAtAutoreleasePool",
        '"@autoreleasepool"',
    ):
        assert marker in lexer_source

    for marker in (
        "bool is_autoreleasepool_scope = false;",
        "std::string autoreleasepool_scope_symbol;",
        "unsigned autoreleasepool_scope_depth = 0;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildAutoreleasePoolScopeSymbol(",
        "At(TokenKind::KwAtAutoreleasePool)",
        "if (Match(TokenKind::KwAtAutoreleasePool)) {",
        "stmt->block_stmt->is_autoreleasepool_scope = true;",
        "stmt->block_stmt->autoreleasepool_scope_symbol =",
        "stmt->block_stmt->autoreleasepool_scope_depth = scope_depth;",
        "unsigned autoreleasepool_scope_depth_ = 0;",
        "unsigned autoreleasepool_scope_serial_ = 0;",
    ):
        assert marker in parser_source
