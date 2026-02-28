from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m176_frontend_module_import_graph_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M176 frontend module map ingestion and import graph parser/AST surface (M176-A001)",
        "BuildModuleImportGraphProfile(...)",
        "IsModuleImportGraphProfileNormalized(...)",
        "module_import_graph_profile",
        "module_import_graph_profile_is_normalized",
        "return_module_import_graph_profile",
        "return_module_import_graph_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m176_frontend_module_import_graph_parser_contract.py -q",
    ):
        assert text in fragment


def test_m176_frontend_module_import_graph_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool module_import_graph_profile_is_normalized = false;",
        "std::string module_import_graph_profile;",
        "bool return_module_import_graph_profile_is_normalized = false;",
        "std::string return_module_import_graph_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildModuleImportGraphProfile(",
        "IsModuleImportGraphProfileNormalized(",
        "CountNamespaceSegments(",
        "param.module_import_graph_profile =",
        "param.module_import_graph_profile_is_normalized =",
        "fn.return_module_import_graph_profile =",
        "fn.return_module_import_graph_profile_is_normalized =",
        "target.module_import_graph_profile =",
        "target.return_module_import_graph_profile =",
    ):
        assert marker in parser_source
