from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
TOKEN_CONTRACT = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_SOURCE = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m149_frontend_property_attribute_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M149 frontend @property grammar and attribute parsing",
        "KwAtProperty",
        "ParseObjcPropertyDecl(...)",
        "ParseObjcPropertyAttributes(...)",
        "ApplyObjcPropertyAttributes(...)",
        "missing ';' after Objective-C @property declaration",
        "python -m pytest tests/tooling/test_objc3c_m149_frontend_property_attribute_contract.py -q",
    ):
        assert text in fragment


def test_m149_frontend_property_attribute_markers_map_to_sources() -> None:
    token_contract = _read(TOKEN_CONTRACT)
    lexer_source = _read(LEXER_SOURCE)
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    assert "KwAtProperty" in token_contract

    for marker in (
        'tokens.push_back(Token{TokenKind::KwAtProperty, "@property"',
        "unsupported '@' directive '@",
    ):
        assert marker in lexer_source

    for marker in (
        "ParseObjcPropertyDecl(Objc3PropertyDecl &property)",
        "ParseObjcPropertyAttributes(std::vector<Objc3PropertyAttributeDecl> &attributes)",
        "ParseObjcPropertyAttributeValueText()",
        "ApplyObjcPropertyAttributes(Objc3PropertyDecl &property)",
        "missing ')' after Objective-C @property attribute list",
        "missing ';' after Objective-C @property declaration",
        "decl->properties.push_back(std::move(property));",
    ):
        assert marker in parser_source

    for marker in (
        "struct Objc3PropertyAttributeDecl",
        "struct Objc3PropertyDecl",
        "std::vector<Objc3PropertyDecl> properties;",
        "bool has_getter = false;",
        "std::string setter_selector;",
    ):
        assert marker in ast_source
