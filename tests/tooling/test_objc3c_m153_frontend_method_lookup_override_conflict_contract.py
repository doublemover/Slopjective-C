from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

FRONTEND_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-frontend.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m153_frontend_method_lookup_override_conflict_section_exists() -> None:
    fragment = _read(FRONTEND_DOC_FRAGMENT)
    for text in (
        "## M153 frontend method lookup-override-conflict parser surface",
        "BuildObjcMethodLookupSymbol(...)",
        "BuildObjcMethodOverrideLookupSymbol(...)",
        "BuildObjcMethodConflictLookupSymbol(...)",
        "AssignObjcMethodLookupOverrideConflictSymbols(...)",
        "FinalizeObjcMethodLookupOverrideConflictPackets(...)",
        'method.method_lookup_symbol = lookup_owner_symbol + "::" + BuildObjcMethodLookupSymbol(method);',
        'decl->semantic_link_symbol = "protocol:" + decl->name;',
        "decl->method_lookup_symbols_lexicographic",
        "decl->override_lookup_symbols_lexicographic",
        "decl->conflict_lookup_symbols_lexicographic",
        "python -m pytest tests/tooling/test_objc3c_m153_frontend_method_lookup_override_conflict_contract.py -q",
    ):
        assert text in fragment


def test_m153_frontend_method_lookup_override_conflict_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "static std::string BuildObjcMethodLookupSymbol(",
        "static std::string BuildObjcMethodOverrideLookupSymbol(",
        "static std::string BuildObjcMethodConflictLookupSymbol(",
        "static std::vector<std::string> BuildObjcMethodLookupSymbolsLexicographic(",
        "static std::vector<std::string> BuildObjcMethodOverrideLookupSymbolsLexicographic(",
        "static std::vector<std::string> BuildObjcMethodConflictLookupSymbolsLexicographic(",
        "void AssignObjcMethodLookupOverrideConflictSymbols(",
        "void FinalizeObjcMethodLookupOverrideConflictPackets(",
        'method.method_lookup_symbol = lookup_owner_symbol + "::" + BuildObjcMethodLookupSymbol(method);',
        'method.override_lookup_symbol = override_owner_symbol + "::" + BuildObjcMethodOverrideLookupSymbol(method);',
        "method.conflict_lookup_symbol = BuildObjcMethodConflictLookupSymbol(method);",
        'decl->semantic_link_symbol = "protocol:" + decl->name;',
        "decl->method_lookup_symbols_lexicographic,",
        "decl->override_lookup_symbols_lexicographic,",
        "decl->conflict_lookup_symbols_lexicographic",
    ):
        assert marker in parser_source

    for marker in (
        "std::string method_lookup_symbol;",
        "std::string override_lookup_symbol;",
        "std::string conflict_lookup_symbol;",
        "std::vector<std::string> method_lookup_symbols_lexicographic;",
        "std::vector<std::string> override_lookup_symbols_lexicographic;",
        "std::vector<std::string> conflict_lookup_symbols_lexicographic;",
    ):
        assert marker in ast_source
