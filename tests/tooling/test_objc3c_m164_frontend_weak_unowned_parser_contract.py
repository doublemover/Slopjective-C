from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m164_frontend_weak_unowned_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M164 frontend weak/unowned parser/AST surface",
        "BuildWeakUnownedLifetimeProfile(...)",
        "BuildPropertyWeakUnownedLifetimeProfile(...)",
        "param.ownership_is_weak_reference",
        "fn.return_ownership_is_unowned_reference",
        "property.is_unowned",
        "property.has_weak_unowned_conflict",
        "python -m pytest tests/tooling/test_objc3c_m164_frontend_weak_unowned_parser_contract.py -q",
    ):
        assert text in fragment


def test_m164_frontend_weak_unowned_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool ownership_is_weak_reference = false;",
        "bool ownership_is_unowned_reference = false;",
        "bool ownership_is_unowned_safe_reference = false;",
        "std::string ownership_lifetime_profile;",
        "std::string ownership_runtime_hook_profile;",
        "bool return_ownership_is_weak_reference = false;",
        "bool return_ownership_is_unowned_reference = false;",
        "bool return_ownership_is_unowned_safe_reference = false;",
        "std::string return_ownership_lifetime_profile;",
        "std::string return_ownership_runtime_hook_profile;",
        "bool is_unowned = false;",
        "bool has_weak_unowned_conflict = false;",
    ):
        assert marker in ast_header

    for marker in (
        "struct Objc3WeakUnownedLifetimeProfile",
        "BuildWeakUnownedLifetimeProfile(",
        "BuildPropertyWeakUnownedLifetimeProfile(",
        'profile.lifetime_profile = "weak";',
        'profile.lifetime_profile = prefer_safe_unowned ? "unowned-safe" : "unowned-unsafe";',
        "property.is_unowned = true;",
        "param.ownership_is_weak_reference =",
        "param.ownership_is_unowned_reference =",
        "fn.return_ownership_is_unowned_reference =",
        "property.ownership_is_unowned_reference =",
        "property.has_weak_unowned_conflict = property.is_weak && property.is_unowned;",
    ):
        assert marker in parser_source
