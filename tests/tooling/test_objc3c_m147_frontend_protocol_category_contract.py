from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
TOKEN_CONTRACT = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m147_frontend_protocol_category_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M147 frontend @protocol/@category grammar",
        "ParseObjcProtocolDecl()",
        "ParseObjcProtocolCompositionClause(...)",
        "ParseObjcCategoryClause(...)",
        "KwAtProtocol",
        "missing '@end' after @protocol",
        "invalid Objective-C protocol composition identifier",
        "python -m pytest tests/tooling/test_objc3c_m147_frontend_protocol_category_contract.py -q",
    ):
        assert text in fragment


def test_m147_frontend_protocol_category_markers_map_to_sources() -> None:
    token_contract = _read(TOKEN_CONTRACT)
    lexer_source = _read(LEXER_SOURCE)
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    assert "KwAtProtocol" in token_contract

    for marker in (
        'tokens.push_back(Token{TokenKind::KwAtProtocol, "@protocol"',
        "unsupported '@' directive '@",
    ):
        assert marker in lexer_source

    for marker in (
        "ParseObjcProtocolDecl()",
        "ParseObjcProtocolCompositionClause(std::vector<std::string> &protocols)",
        "ParseObjcCategoryClause(std::string &category_name, bool &has_category)",
        "missing '@end' after @protocol",
        "invalid Objective-C protocol composition identifier",
        "missing ')' after Objective-C category name",
    ):
        assert marker in parser_source

    for marker in (
        "struct Objc3ProtocolDecl",
        "std::vector<Objc3ProtocolDecl> protocols;",
        "std::string category_name;",
        "bool has_category = false;",
        "std::vector<std::string> adopted_protocols;",
    ):
        assert marker in ast_source
