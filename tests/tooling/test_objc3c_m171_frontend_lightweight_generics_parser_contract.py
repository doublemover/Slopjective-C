from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m171_frontend_lightweight_generics_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M171 frontend lightweight generics constraint parser/AST surface (M171-A001)",
        "BuildLightweightGenericConstraintProfile(...)",
        "IsLightweightGenericConstraintProfileNormalized(...)",
        "lightweight_generic_constraint_profile",
        "lightweight_generic_constraint_profile_is_normalized",
        "return_lightweight_generic_constraint_profile",
        "return_lightweight_generic_constraint_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m171_frontend_lightweight_generics_parser_contract.py -q",
    ):
        assert text in fragment


def test_m171_frontend_lightweight_generics_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool lightweight_generic_constraint_profile_is_normalized = false;",
        "std::string lightweight_generic_constraint_profile;",
        "bool return_lightweight_generic_constraint_profile_is_normalized = false;",
        "std::string return_lightweight_generic_constraint_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildLightweightGenericConstraintProfile(",
        "IsLightweightGenericConstraintProfileNormalized(",
        "ParseFunctionReturnType(",
        "ParseParameterType(",
        "param.lightweight_generic_constraint_profile =",
        "param.lightweight_generic_constraint_profile_is_normalized =",
        "fn.return_lightweight_generic_constraint_profile =",
        "fn.return_lightweight_generic_constraint_profile_is_normalized =",
        "target.lightweight_generic_constraint_profile =",
        "target.return_lightweight_generic_constraint_profile =",
    ):
        assert marker in parser_source
