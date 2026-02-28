from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m174_frontend_variance_bridge_cast_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M174 frontend variance and bridged-cast parser/AST surface (M174-A001)",
        "BuildVarianceBridgeCastProfile(...)",
        "IsVarianceBridgeCastProfileNormalized(...)",
        "variance_bridge_cast_profile",
        "variance_bridge_cast_profile_is_normalized",
        "return_variance_bridge_cast_profile",
        "return_variance_bridge_cast_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m174_frontend_variance_bridge_cast_parser_contract.py -q",
    ):
        assert text in fragment


def test_m174_frontend_variance_bridge_cast_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool variance_bridge_cast_profile_is_normalized = false;",
        "std::string variance_bridge_cast_profile;",
        "bool return_variance_bridge_cast_profile_is_normalized = false;",
        "std::string return_variance_bridge_cast_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildVarianceBridgeCastProfile(",
        "IsVarianceBridgeCastProfileNormalized(",
        "CountMarkerOccurrences(",
        "param.variance_bridge_cast_profile =",
        "param.variance_bridge_cast_profile_is_normalized =",
        "fn.return_variance_bridge_cast_profile =",
        "fn.return_variance_bridge_cast_profile_is_normalized =",
        "target.variance_bridge_cast_profile =",
        "target.return_variance_bridge_cast_profile =",
    ):
        assert marker in parser_source
