from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m152_frontend_class_protocol_category_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M152 frontend class-protocol-category semantic-linking parser surface",
        "BuildProtocolSemanticLinkTargetsLexicographic(...)",
        "BuildObjcCategorySemanticLinkSymbol(...)",
        'decl->semantic_link_symbol = "protocol:" + decl->name;',
        "decl->adopted_protocols_lexicographic = BuildProtocolSemanticLinkTargetsLexicographic(...)",
        "decl->semantic_link_interface_symbol = BuildObjcContainerScopeOwner(\"interface\", ...)",
        "python -m pytest tests/tooling/test_objc3c_m152_frontend_class_protocol_category_linking_contract.py -q",
    ):
        assert text in fragment


def test_m152_frontend_class_protocol_category_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "static std::vector<std::string> BuildProtocolSemanticLinkTargetsLexicographic(",
        "static std::string BuildObjcCategorySemanticLinkSymbol(",
        'decl->semantic_link_symbol = "protocol:" + decl->name;',
        "decl->inherited_protocols_lexicographic =",
        "decl->adopted_protocols_lexicographic = BuildProtocolSemanticLinkTargetsLexicographic(decl->adopted_protocols);",
        "decl->semantic_link_symbol =",
        "decl->semantic_link_super_symbol = \"interface:\" + decl->super_name;",
        "decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(decl->name, decl->category_name);",
        "decl->semantic_link_interface_symbol = BuildObjcContainerScopeOwner(\"interface\", decl->name, false, \"\");",
    ):
        assert marker in parser_source

    for marker in (
        "std::vector<std::string> inherited_protocols_lexicographic;",
        "std::vector<std::string> adopted_protocols_lexicographic;",
        "std::string semantic_link_symbol;",
        "std::string semantic_link_super_symbol;",
        "std::string semantic_link_interface_symbol;",
        "std::string semantic_link_category_symbol;",
    ):
        assert marker in ast_source
