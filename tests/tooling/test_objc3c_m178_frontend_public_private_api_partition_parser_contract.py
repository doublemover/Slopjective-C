from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m178_frontend_public_private_api_partition_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M178 frontend public/private API partition parser/AST surface (M178-A001)",
        "BuildPublicPrivateApiPartitionProfile(...)",
        "IsPublicPrivateApiPartitionProfileNormalized(...)",
        "public_private_api_partition_profile",
        "public_private_api_partition_profile_is_normalized",
        "return_public_private_api_partition_profile",
        "return_public_private_api_partition_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m178_frontend_public_private_api_partition_parser_contract.py -q",
    ):
        assert text in fragment


def test_m178_frontend_public_private_api_partition_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool public_private_api_partition_profile_is_normalized = false;",
        "std::string public_private_api_partition_profile;",
        "bool return_public_private_api_partition_profile_is_normalized = false;",
        "std::string return_public_private_api_partition_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildPublicPrivateApiPartitionProfile(",
        "IsPublicPrivateApiPartitionProfileNormalized(",
        "CountNamespaceSegments(",
        "param.public_private_api_partition_profile =",
        "param.public_private_api_partition_profile_is_normalized =",
        "fn.return_public_private_api_partition_profile =",
        "fn.return_public_private_api_partition_profile_is_normalized =",
        "target.public_private_api_partition_profile =",
        "target.return_public_private_api_partition_profile =",
    ):
        assert marker in parser_source
