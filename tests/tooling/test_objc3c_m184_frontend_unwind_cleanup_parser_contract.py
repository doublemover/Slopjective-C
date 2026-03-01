from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m184_frontend_unwind_cleanup_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M184 frontend unwind safety and cleanup emission packetization",
        "IsExceptionalExitSymbol(...)",
        "IsCleanupActionSymbol(...)",
        "IsCleanupScopeSymbol(...)",
        "IsCleanupResumeSymbol(...)",
        "struct Objc3UnwindCleanupProfile",
        "BuildUnwindCleanupProfile(...)",
        "IsUnwindCleanupProfileNormalized(...)",
        "FinalizeUnwindCleanupProfile(FunctionDecl &fn)",
        "FinalizeUnwindCleanupProfile(Objc3MethodDecl &method)",
        "fn.unwind_cleanup_sites = profile.unwind_cleanup_sites;",
        "method.unwind_cleanup_sites = profile.unwind_cleanup_sites;",
        "bool unwind_cleanup_profile_is_normalized = false;",
        "bool deterministic_unwind_cleanup_handoff = false;",
        "std::string unwind_cleanup_profile;",
        "python -m pytest tests/tooling/test_objc3c_m184_frontend_unwind_cleanup_parser_contract.py -q",
    ):
        assert text in fragment


def test_m184_frontend_unwind_cleanup_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool unwind_cleanup_profile_is_normalized = false;",
        "bool deterministic_unwind_cleanup_handoff = false;",
        "std::size_t unwind_cleanup_sites = 0;",
        "std::size_t exceptional_exit_sites = 0;",
        "std::size_t cleanup_action_sites = 0;",
        "std::size_t cleanup_scope_sites = 0;",
        "std::size_t cleanup_resume_sites = 0;",
        "std::size_t unwind_cleanup_normalized_sites = 0;",
        "std::size_t unwind_cleanup_fail_closed_sites = 0;",
        "std::size_t unwind_cleanup_contract_violation_sites = 0;",
        "std::string unwind_cleanup_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsExceptionalExitSymbol(",
        "IsCleanupActionSymbol(",
        "IsCleanupScopeSymbol(",
        "IsCleanupResumeSymbol(",
        "CollectUnwindCleanupExprSites(",
        "CollectUnwindCleanupStmtSites(",
        "CountUnwindCleanupSitesInBody(",
        "struct Objc3UnwindCleanupProfile {",
        "BuildUnwindCleanupProfile(",
        "IsUnwindCleanupProfileNormalized(",
        "BuildUnwindCleanupProfileFromCounts(",
        "BuildUnwindCleanupProfileFromFunction(",
        "BuildUnwindCleanupProfileFromOpaqueBody(",
        "FinalizeUnwindCleanupProfile(FunctionDecl &fn)",
        "FinalizeUnwindCleanupProfile(Objc3MethodDecl &method)",
        "target.unwind_cleanup_profile = source.unwind_cleanup_profile;",
        "fn.unwind_cleanup_sites = profile.unwind_cleanup_sites;",
        "method.unwind_cleanup_sites = profile.unwind_cleanup_sites;",
        "FinalizeUnwindCleanupProfile(*fn);",
        "FinalizeUnwindCleanupProfile(method);",
        "unwind-cleanup:unwind_cleanup_sites=",
        "profile.normalized_sites + profile.fail_closed_sites !=",
    ):
        assert marker in parser_source
