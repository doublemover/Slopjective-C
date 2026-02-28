from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m181_frontend_throws_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M181 frontend throws declarations and propagation parser/AST surface (M181-A001)",
        'function_decl   = "fn" ident "(" [ param { "," param } ] ")" [ throws_clause ] [ "->" return_type ] [ throws_clause ] ( block | ";" ) ;',
        'throws_clause   = "throws" ;',
        "BuildThrowsDeclarationProfile(...)",
        "IsThrowsDeclarationProfileNormalized(...)",
        "ParseOptionalThrowsClause(FunctionDecl &fn)",
        "ParseOptionalThrowsClause(Objc3MethodDecl &method)",
        "target.throws_declaration_profile = source.throws_declaration_profile;",
        "python -m pytest tests/tooling/test_objc3c_m181_frontend_throws_parser_contract.py -q",
    ):
        assert text in fragment


def test_m181_frontend_throws_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool throws_declared = false;",
        "bool throws_declaration_profile_is_normalized = false;",
        "std::string throws_declaration_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildThrowsDeclarationProfile(",
        "IsThrowsDeclarationProfileNormalized(",
        "AtThrowsClauseKeyword()",
        "ParseOptionalThrowsClause(FunctionDecl &fn)",
        "ParseOptionalThrowsClause(Objc3MethodDecl &method)",
        "FinalizeThrowsDeclarationProfile(FunctionDecl &fn, bool has_return_annotation)",
        "FinalizeThrowsDeclarationProfile(Objc3MethodDecl &method)",
        "duplicate 'throws' declaration modifier",
        "target.throws_declared = source.throws_declared;",
        "target.throws_declaration_profile = source.throws_declaration_profile;",
        "if (!ParseOptionalThrowsClause(*fn)) {",
        "if (!ParseOptionalThrowsClause(method)) {",
        "FinalizeThrowsDeclarationProfile(*fn, has_return_annotation);",
        "FinalizeThrowsDeclarationProfile(method);",
    ):
        assert marker in parser_source
