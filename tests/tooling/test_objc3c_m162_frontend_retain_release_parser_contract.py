from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m162_frontend_retain_release_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M162 frontend retain/release ownership-operation profile surface",
        "BuildParamOwnershipOperationProfile(...)",
        "BuildReturnOwnershipOperationProfile(...)",
        "param.ownership_insert_retain",
        "fn.return_ownership_insert_release",
        "CopyMethodReturnTypeFromFunctionDecl(...)",
        "python -m pytest tests/tooling/test_objc3c_m162_frontend_retain_release_parser_contract.py -q",
    ):
        assert text in fragment


def test_m162_frontend_retain_release_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool ownership_insert_retain = false;",
        "bool ownership_insert_release = false;",
        "bool ownership_insert_autorelease = false;",
        "std::string ownership_operation_profile;",
        "bool return_ownership_insert_retain = false;",
        "bool return_ownership_insert_release = false;",
        "bool return_ownership_insert_autorelease = false;",
        "std::string return_ownership_operation_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "struct Objc3OwnershipOperationProfile",
        "BuildParamOwnershipOperationProfile(",
        "BuildReturnOwnershipOperationProfile(",
        'profile.profile = "param-retain-release";',
        'profile.profile = "return-retain-release-transfer";',
        "param.ownership_insert_retain =",
        "param.ownership_insert_release =",
        "param.ownership_insert_autorelease =",
        "param.ownership_operation_profile =",
        "fn.return_ownership_insert_retain =",
        "fn.return_ownership_insert_release =",
        "fn.return_ownership_insert_autorelease =",
        "fn.return_ownership_operation_profile =",
        "target.return_ownership_insert_retain = source.return_ownership_insert_retain;",
        "target.ownership_insert_release = source.ownership_insert_release;",
    ):
        assert marker in parser_source
