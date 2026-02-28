from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m173_frontend_protocol_qualified_object_type_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M173 frontend protocol-qualified object type parser/AST surface (M173-A001)",
        "BuildProtocolQualifiedObjectTypeProfile(...)",
        "IsProtocolQualifiedObjectTypeProfileNormalized(...)",
        "protocol_qualified_object_type_profile",
        "protocol_qualified_object_type_profile_is_normalized",
        "return_protocol_qualified_object_type_profile",
        "return_protocol_qualified_object_type_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m173_frontend_protocol_qualified_object_type_parser_contract.py -q",
    ):
        assert text in fragment


def test_m173_frontend_protocol_qualified_object_type_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool protocol_qualified_object_type_profile_is_normalized = false;",
        "std::string protocol_qualified_object_type_profile;",
        "bool return_protocol_qualified_object_type_profile_is_normalized = false;",
        "std::string return_protocol_qualified_object_type_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildProtocolQualifiedObjectTypeProfile(",
        "IsProtocolQualifiedObjectTypeProfileNormalized(",
        "ParseFunctionReturnType(",
        "ParseParameterType(",
        "param.protocol_qualified_object_type_profile =",
        "param.protocol_qualified_object_type_profile_is_normalized =",
        "fn.return_protocol_qualified_object_type_profile =",
        "fn.return_protocol_qualified_object_type_profile_is_normalized =",
        "target.protocol_qualified_object_type_profile =",
        "target.return_protocol_qualified_object_type_profile =",
    ):
        assert marker in parser_source
