from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m151_frontend_symbol_graph_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M151 frontend symbol graph and scope-resolution parser surface",
        "BuildScopePathLexicographic(...)",
        "BuildObjcContainerScopeOwner(...)",
        'fn->scope_owner_symbol = "global";',
        "decl->scope_owner_symbol = BuildObjcContainerScopeOwner(...)",
        "method.scope_owner_symbol = decl->scope_owner_symbol;",
        "property.scope_owner_symbol = decl->scope_owner_symbol;",
        "python -m pytest tests/tooling/test_objc3c_m151_frontend_symbol_graph_scope_resolution_contract.py -q",
    ):
        assert text in fragment


def test_m151_frontend_symbol_graph_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "static std::vector<std::string> BuildScopePathLexicographic(",
        "static std::string BuildObjcContainerScopeOwner(",
        "static std::string BuildObjcMethodScopePathSymbol(",
        "static std::string BuildObjcPropertyScopePathSymbol(",
        'fn->scope_owner_symbol = "global";',
        "fn->scope_path_lexicographic =",
        "decl->scope_owner_symbol = BuildObjcContainerScopeOwner(",
        "decl->scope_path_lexicographic =",
        "method.scope_owner_symbol = decl->scope_owner_symbol;",
        "method.scope_path_symbol = decl->scope_owner_symbol + \"::\" + BuildObjcMethodScopePathSymbol(method);",
        "property.scope_owner_symbol = decl->scope_owner_symbol;",
        "property.scope_path_symbol = decl->scope_owner_symbol + \"::\" + BuildObjcPropertyScopePathSymbol(property);",
    ):
        assert marker in parser_source

    for marker in (
        "std::string scope_owner_symbol;",
        "std::vector<std::string> scope_path_lexicographic;",
        "std::string scope_path_symbol;",
    ):
        assert marker in ast_source
