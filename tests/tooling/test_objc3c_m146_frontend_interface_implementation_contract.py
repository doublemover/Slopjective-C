from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
TOKEN_CONTRACT = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m146_frontend_interface_implementation_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M146 frontend @interface/@implementation grammar",
        "ParseObjcInterfaceDecl()",
        "ParseObjcImplementationDecl()",
        "ParseObjcMethodDecl(...)",
        "KwAtInterface",
        "KwAtImplementation",
        "KwAtEnd",
        "missing '@end' after @interface",
        "missing '@end' after @implementation",
        "python -m pytest tests/tooling/test_objc3c_m146_frontend_interface_implementation_contract.py -q",
    ):
        assert text in fragment


def test_m146_frontend_interface_implementation_markers_map_to_sources() -> None:
    token_contract = _read(TOKEN_CONTRACT)
    lexer_source = _read(LEXER_SOURCE)
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "KwAtInterface",
        "KwAtImplementation",
        "KwAtEnd",
    ):
        assert marker in token_contract

    for marker in (
        'tokens.push_back(Token{TokenKind::KwAtInterface, "@interface"',
        'tokens.push_back(Token{TokenKind::KwAtImplementation, "@implementation"',
        'tokens.push_back(Token{TokenKind::KwAtEnd, "@end"',
        "unsupported '@' directive '@",
    ):
        assert marker in lexer_source

    for marker in (
        "ParseObjcInterfaceDecl()",
        "ParseObjcImplementationDecl()",
        "ParseObjcMethodDecl(Objc3MethodDecl &method, bool allow_body)",
        "missing '@end' after @interface",
        "missing '@end' after @implementation",
    ):
        assert marker in parser_source

    for marker in (
        "struct Objc3MethodDecl",
        "struct Objc3InterfaceDecl",
        "struct Objc3ImplementationDecl",
        "std::vector<Objc3InterfaceDecl> interfaces;",
        "std::vector<Objc3ImplementationDecl> implementations;",
    ):
        assert marker in ast_source
