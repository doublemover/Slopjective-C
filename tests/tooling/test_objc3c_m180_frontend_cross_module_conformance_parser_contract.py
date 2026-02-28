from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m180_frontend_cross_module_conformance_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M180 frontend cross-module conformance suite parser/AST surface (M180-A001)",
        "BuildCrossModuleConformanceProfile(...)",
        "IsCrossModuleConformanceProfileNormalized(...)",
        "cross_module_conformance_profile",
        "cross_module_conformance_profile_is_normalized",
        "return_cross_module_conformance_profile",
        "return_cross_module_conformance_profile_is_normalized",
        "CopyMethodReturnTypeFromFunctionDecl(...)",
        "CopyPropertyTypeFromParam(...)",
        "python -m pytest tests/tooling/test_objc3c_m180_frontend_cross_module_conformance_parser_contract.py -q",
    ):
        assert text in fragment


def test_m180_frontend_cross_module_conformance_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool cross_module_conformance_profile_is_normalized = false;",
        "std::string cross_module_conformance_profile;",
        "bool return_cross_module_conformance_profile_is_normalized = false;",
        "std::string return_cross_module_conformance_profile;",
    ):
        assert marker in ast_header

    assert ast_header.count("bool cross_module_conformance_profile_is_normalized = false;") >= 2
    assert ast_header.count("std::string cross_module_conformance_profile;") >= 2
    assert ast_header.count("bool return_cross_module_conformance_profile_is_normalized = false;") >= 2
    assert ast_header.count("std::string return_cross_module_conformance_profile;") >= 2

    for marker in (
        "BuildCrossModuleConformanceProfile(",
        "IsCrossModuleConformanceProfileNormalized(",
        "if (TryParseVectorTypeSpelling(type_token, vector_type, vector_base_spelling, vector_lane_count))",
        "param.cross_module_conformance_profile =",
        "param.cross_module_conformance_profile_is_normalized =",
        "fn.return_cross_module_conformance_profile =",
        "fn.return_cross_module_conformance_profile_is_normalized =",
        "CopyMethodReturnTypeFromFunctionDecl(",
        "target.return_cross_module_conformance_profile =",
        "CopyPropertyTypeFromParam(",
        "target.cross_module_conformance_profile =",
    ):
        assert marker in parser_source
