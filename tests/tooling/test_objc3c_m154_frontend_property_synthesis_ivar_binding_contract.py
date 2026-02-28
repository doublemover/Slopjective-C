from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

FRONTEND_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m154_frontend_property_synthesis_ivar_binding_section_exists() -> None:
    fragment = _read(FRONTEND_DOC_FRAGMENT)
    for text in (
        "## M154 frontend property-synthesis-ivar-binding parser surface",
        "BuildObjcPropertySynthesisSymbol(...)",
        "BuildObjcIvarBindingSymbol(...)",
        "BuildObjcPropertySynthesisSymbolsLexicographic(...)",
        "BuildObjcIvarBindingSymbolsLexicographic(...)",
        "AssignObjcPropertySynthesisIvarBindingSymbols(...)",
        "FinalizeObjcPropertySynthesisIvarBindingPackets(...)",
        'property.property_synthesis_symbol = synthesis_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property);',
        'property.ivar_binding_symbol = synthesis_owner_symbol + "::" + BuildObjcIvarBindingSymbol(property);',
        "decl->property_synthesis_symbols_lexicographic",
        "decl->ivar_binding_symbols_lexicographic",
        "python -m pytest tests/tooling/test_objc3c_m154_frontend_property_synthesis_ivar_binding_contract.py -q",
    ):
        assert text in fragment


def test_m154_frontend_property_synthesis_ivar_binding_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "static std::string BuildObjcPropertySynthesisSymbol(",
        "static std::string BuildObjcIvarBindingSymbol(",
        "static std::vector<std::string> BuildObjcPropertySynthesisSymbolsLexicographic(",
        "static std::vector<std::string> BuildObjcIvarBindingSymbolsLexicographic(",
        "void AssignObjcPropertySynthesisIvarBindingSymbols(",
        "void FinalizeObjcPropertySynthesisIvarBindingPackets(",
        'property.property_synthesis_symbol = synthesis_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property);',
        'property.ivar_binding_symbol = synthesis_owner_symbol + "::" + BuildObjcIvarBindingSymbol(property);',
        "AssignObjcPropertySynthesisIvarBindingSymbols(property, decl->semantic_link_symbol);",
        "decl->property_synthesis_symbols_lexicographic,",
        "decl->ivar_binding_symbols_lexicographic",
    ):
        assert marker in parser_source

    for marker in (
        "std::string property_synthesis_symbol;",
        "std::string ivar_binding_symbol;",
        "std::vector<std::string> property_synthesis_symbols_lexicographic;",
        "std::vector<std::string> ivar_binding_symbols_lexicographic;",
    ):
        assert marker in ast_source
