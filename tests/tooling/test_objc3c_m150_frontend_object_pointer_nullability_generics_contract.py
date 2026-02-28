from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m150_frontend_object_pointer_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M150 frontend object pointer declarators, nullability, lightweight generics parse",
        "ParseFunctionReturnType(...)",
        "ParseParameterType(...)",
        "object_pointer_type_spelling",
        "return_object_pointer_type_spelling",
        "CopyPropertyTypeFromParam(...)",
        "python -m pytest tests/tooling/test_objc3c_m150_frontend_object_pointer_nullability_generics_contract.py -q",
    ):
        assert text in fragment


def test_m150_frontend_object_pointer_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "fn.return_object_pointer_type_spelling = false;",
        "fn.return_object_pointer_type_name.clear();",
        "fn.return_object_pointer_type_spelling = true;",
        "param.object_pointer_type_spelling = false;",
        "param.object_pointer_type_name.clear();",
        "param.object_pointer_type_spelling = true;",
        "target.object_pointer_type_spelling = source.object_pointer_type_spelling;",
    ):
        assert marker in parser_source

    for marker in (
        "bool object_pointer_type_spelling = false;",
        "std::string object_pointer_type_name;",
        "bool return_object_pointer_type_spelling = false;",
        "std::string return_object_pointer_type_name;",
    ):
        assert marker in ast_source
