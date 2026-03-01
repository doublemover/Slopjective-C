from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m199_frontend_foreign_type_diagnostics_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M199 frontend foreign type import diagnostics",
        "O3P108",
        "O3P114",
        "O3S206",
        "param.id_spelling",
        "param.class_spelling",
        "param.instancetype_spelling",
        "param.object_pointer_type_spelling",
        "param.object_pointer_type_name",
        "fn.return_id_spelling",
        "fn.return_class_spelling",
        "fn.return_instancetype_spelling",
        "fn.return_object_pointer_type_spelling",
        "fn.return_object_pointer_type_name",
        "SupportsGenericParamTypeSuffix",
        "SupportsGenericReturnTypeSuffix",
        "python -m pytest tests/tooling/test_objc3c_m199_frontend_foreign_type_diagnostics_contract.py -q",
    ):
        assert text in fragment


def test_m199_frontend_foreign_type_diagnostics_markers_map_to_sources() -> None:
    ast_source = _read(AST_SOURCE)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool id_spelling = false;",
        "bool class_spelling = false;",
        "bool instancetype_spelling = false;",
        "bool object_pointer_type_spelling = false;",
        "bool return_id_spelling = false;",
        "bool return_class_spelling = false;",
        "bool return_instancetype_spelling = false;",
        "bool return_object_pointer_type_spelling = false;",
    ):
        assert marker in ast_source

    for marker in (
        'MakeDiag(token.line, token.column, "O3P108",',
        'MakeDiag(open.line, open.column, "O3P108",',
        'MakeDiag(token.line, token.column, "O3P114",',
        "param.id_spelling = true;",
        "param.class_spelling = true;",
        "param.instancetype_spelling = true;",
        "param.object_pointer_type_spelling = true;",
        "param.object_pointer_type_name = type_token.text;",
        "fn.return_id_spelling = true;",
        "fn.return_class_spelling = true;",
        "fn.return_instancetype_spelling = true;",
        "fn.return_object_pointer_type_spelling = true;",
        "fn.return_object_pointer_type_name = type_token.text;",
    ):
        assert marker in parser_source
