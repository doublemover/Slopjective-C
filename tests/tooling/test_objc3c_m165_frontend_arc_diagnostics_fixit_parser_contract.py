from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m165_frontend_arc_diagnostics_fixit_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M165 frontend ARC diagnostics/fix-it parser/AST surface",
        "BuildArcDiagnosticFixitProfile(...)",
        "param.ownership_arc_diagnostic_candidate",
        "fn.return_ownership_arc_fixit_hint",
        "property.ownership_arc_diagnostic_profile",
        "property.has_weak_unowned_conflict",
        "python -m pytest tests/tooling/test_objc3c_m165_frontend_arc_diagnostics_fixit_parser_contract.py -q",
    ):
        assert text in fragment


def test_m165_frontend_arc_diagnostics_fixit_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool ownership_arc_diagnostic_candidate = false;",
        "bool ownership_arc_fixit_available = false;",
        "std::string ownership_arc_diagnostic_profile;",
        "std::string ownership_arc_fixit_hint;",
        "bool return_ownership_arc_diagnostic_candidate = false;",
        "bool return_ownership_arc_fixit_available = false;",
        "std::string return_ownership_arc_diagnostic_profile;",
        "std::string return_ownership_arc_fixit_hint;",
    ):
        assert marker in ast_header

    for marker in (
        "struct Objc3ArcDiagnosticFixitProfile",
        "BuildArcDiagnosticFixitProfile(",
        'profile.diagnostic_profile = "arc-weak-unowned-conflict";',
        'profile.fixit_hint = "remove-weak-or-unowned-attribute";',
        "param.ownership_arc_diagnostic_candidate =",
        "param.ownership_arc_fixit_hint =",
        "fn.return_ownership_arc_diagnostic_profile =",
        "fn.return_ownership_arc_fixit_hint =",
        "property.ownership_arc_diagnostic_profile =",
        "property.ownership_arc_fixit_available =",
        "property.has_weak_unowned_conflict = property.is_weak && property.is_unowned;",
    ):
        assert marker in parser_source
