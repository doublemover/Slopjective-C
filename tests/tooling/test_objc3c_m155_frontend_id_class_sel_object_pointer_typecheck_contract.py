from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
AST_SOURCE = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m155_frontend_typecheck_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M155 frontend id/Class/SEL/object-pointer typecheck parser surface",
        "BuildObjcTypecheckParamFamilySymbol(...)",
        "BuildObjcTypecheckReturnFamilySymbol(...)",
        "fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);",
        "param.typecheck_family_symbol = BuildObjcTypecheckParamFamilySymbol(param);",
        "target.return_typecheck_family_symbol = source.return_typecheck_family_symbol;",
        "target.typecheck_family_symbol = source.typecheck_family_symbol;",
        "typecheck_family_symbol",
        "return_typecheck_family_symbol",
        "python -m pytest tests/tooling/test_objc3c_m155_frontend_id_class_sel_object_pointer_typecheck_contract.py -q",
    ):
        assert text in fragment


def test_m155_frontend_typecheck_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    ast_source = _read(AST_SOURCE)

    for marker in (
        "static std::string BuildObjcTypecheckParamFamilySymbol(",
        "static std::string BuildObjcTypecheckReturnFamilySymbol(",
        'return "object-pointer:" + param.object_pointer_type_name;',
        'return "object-pointer:" + fn.return_object_pointer_type_name;',
        "target.return_typecheck_family_symbol = source.return_typecheck_family_symbol;",
        "target.typecheck_family_symbol = source.typecheck_family_symbol;",
        "fn.return_sel_spelling = false;",
        "fn.return_sel_spelling = true;",
        "fn.return_typecheck_family_symbol.clear();",
        "fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);",
        "param.sel_spelling = false;",
        "param.sel_spelling = true;",
        "param.typecheck_family_symbol.clear();",
        "param.typecheck_family_symbol = BuildObjcTypecheckParamFamilySymbol(param);",
    ):
        assert marker in parser_source

    for marker in (
        "bool sel_spelling = false;",
        "bool return_sel_spelling = false;",
        "std::string typecheck_family_symbol;",
        "std::string return_typecheck_family_symbol;",
    ):
        assert marker in ast_source
