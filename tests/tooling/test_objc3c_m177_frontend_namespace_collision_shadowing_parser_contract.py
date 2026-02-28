from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m177_frontend_namespace_collision_shadowing_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M177 frontend namespace collision and shadowing diagnostics parser/AST surface (M177-A001)",
        "BuildNamespaceCollisionShadowingProfile(...)",
        "IsNamespaceCollisionShadowingProfileNormalized(...)",
        "namespace_collision_shadowing_profile",
        "namespace_collision_shadowing_profile_is_normalized",
        "return_namespace_collision_shadowing_profile",
        "return_namespace_collision_shadowing_profile_is_normalized",
        "python -m pytest tests/tooling/test_objc3c_m177_frontend_namespace_collision_shadowing_parser_contract.py -q",
    ):
        assert text in fragment


def test_m177_frontend_namespace_collision_shadowing_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool namespace_collision_shadowing_profile_is_normalized = false;",
        "std::string namespace_collision_shadowing_profile;",
        "bool return_namespace_collision_shadowing_profile_is_normalized = false;",
        "std::string return_namespace_collision_shadowing_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "BuildNamespaceCollisionShadowingProfile(",
        "IsNamespaceCollisionShadowingProfileNormalized(",
        "CountNamespaceSegments(",
        "param.namespace_collision_shadowing_profile =",
        "param.namespace_collision_shadowing_profile_is_normalized =",
        "fn.return_namespace_collision_shadowing_profile =",
        "fn.return_namespace_collision_shadowing_profile_is_normalized =",
        "target.namespace_collision_shadowing_profile =",
        "target.return_namespace_collision_shadowing_profile =",
    ):
        assert marker in parser_source
