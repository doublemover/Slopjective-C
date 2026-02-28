from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m175_frontend_generic_metadata_abi_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M175 frontend generic metadata emission and ABI checks parser/AST surface (M175-A001)",
        "BuildGenericMetadataAbiProfile(...)",
        "IsGenericMetadataAbiProfileNormalized(...)",
        "generic_metadata_abi_profile",
        "generic_metadata_abi_profile_is_normalized",
        "return_generic_metadata_abi_profile",
        "return_generic_metadata_abi_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m175_frontend_generic_metadata_abi_parser_contract.py -q",
    ):
        assert text in fragment


def test_m175_frontend_generic_metadata_abi_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool generic_metadata_abi_profile_is_normalized = false;",
        "std::string generic_metadata_abi_profile;",
        "bool return_generic_metadata_abi_profile_is_normalized = false;",
        "std::string return_generic_metadata_abi_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildGenericMetadataAbiProfile(",
        "IsGenericMetadataAbiProfileNormalized(",
        "CountTopLevelGenericArgumentSlots(",
        "param.generic_metadata_abi_profile =",
        "param.generic_metadata_abi_profile_is_normalized =",
        "fn.return_generic_metadata_abi_profile =",
        "fn.return_generic_metadata_abi_profile_is_normalized =",
        "target.generic_metadata_abi_profile =",
        "target.return_generic_metadata_abi_profile =",
    ):
        assert marker in parser_source
