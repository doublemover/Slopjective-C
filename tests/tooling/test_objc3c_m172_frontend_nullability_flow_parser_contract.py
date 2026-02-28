from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m172_frontend_nullability_flow_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M172 frontend nullability-flow parser/AST surface (M172-A001)",
        "BuildNullabilityFlowProfile(...)",
        "IsNullabilityFlowProfileNormalized(...)",
        "nullability_flow_profile",
        "nullability_flow_profile_is_normalized",
        "return_nullability_flow_profile",
        "return_nullability_flow_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m172_frontend_nullability_flow_parser_contract.py -q",
    ):
        assert text in fragment


def test_m172_frontend_nullability_flow_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool nullability_flow_profile_is_normalized = false;",
        "std::string nullability_flow_profile;",
        "bool return_nullability_flow_profile_is_normalized = false;",
        "std::string return_nullability_flow_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildNullabilityFlowProfile(",
        "IsNullabilityFlowProfileNormalized(",
        "ParseFunctionReturnType(",
        "ParseParameterType(",
        "param.nullability_flow_profile =",
        "param.nullability_flow_profile_is_normalized =",
        "fn.return_nullability_flow_profile =",
        "fn.return_nullability_flow_profile_is_normalized =",
        "target.nullability_flow_profile =",
        "target.return_nullability_flow_profile =",
    ):
        assert marker in parser_source
