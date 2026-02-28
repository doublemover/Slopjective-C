from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m187_frontend_await_suspension_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M187 frontend await lowering and suspension packetization",
        "IsAwaitKeywordSymbol(...)",
        "IsAwaitSuspensionPointSymbol(...)",
        "IsAwaitResumeSymbol(...)",
        "IsAwaitStateMachineSymbol(...)",
        "IsAwaitContinuationSymbol(...)",
        "struct Objc3AwaitSuspensionProfile",
        "BuildAwaitSuspensionProfile(...)",
        "IsAwaitSuspensionProfileNormalized(...)",
        "FinalizeAwaitSuspensionProfile(FunctionDecl &fn)",
        "FinalizeAwaitSuspensionProfile(Objc3MethodDecl &method)",
        "fn.await_suspension_sites = profile.await_suspension_sites;",
        "method.await_suspension_sites = profile.await_suspension_sites;",
        "bool await_suspension_profile_is_normalized = false;",
        "bool deterministic_await_suspension_handoff = false;",
        "std::string await_suspension_profile;",
        "python -m pytest tests/tooling/test_objc3c_m187_frontend_await_suspension_parser_contract.py -q",
    ):
        assert text in fragment


def test_m187_frontend_await_suspension_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool await_suspension_profile_is_normalized = false;",
        "bool deterministic_await_suspension_handoff = false;",
        "std::size_t await_suspension_sites = 0;",
        "std::size_t await_keyword_sites = 0;",
        "std::size_t await_suspension_point_sites = 0;",
        "std::size_t await_resume_sites = 0;",
        "std::size_t await_state_machine_sites = 0;",
        "std::size_t await_continuation_sites = 0;",
        "std::size_t await_suspension_normalized_sites = 0;",
        "std::size_t await_suspension_gate_blocked_sites = 0;",
        "std::size_t await_suspension_contract_violation_sites = 0;",
        "std::string await_suspension_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsAwaitKeywordSymbol(",
        "IsAwaitSuspensionPointSymbol(",
        "IsAwaitResumeSymbol(",
        "IsAwaitStateMachineSymbol(",
        "IsAwaitContinuationSymbol(",
        "CollectAwaitSuspensionExprSites(",
        "CollectAwaitSuspensionStmtSites(",
        "CountAwaitSuspensionSitesInBody(",
        "struct Objc3AwaitSuspensionProfile {",
        "BuildAwaitSuspensionProfile(",
        "IsAwaitSuspensionProfileNormalized(",
        "BuildAwaitSuspensionProfileFromCounts(",
        "BuildAwaitSuspensionProfileFromFunction(",
        "BuildAwaitSuspensionProfileFromOpaqueBody(",
        "FinalizeAwaitSuspensionProfile(FunctionDecl &fn)",
        "FinalizeAwaitSuspensionProfile(Objc3MethodDecl &method)",
        "target.await_suspension_profile = source.await_suspension_profile;",
        "fn.await_suspension_sites = profile.await_suspension_sites;",
        "method.await_suspension_sites = profile.await_suspension_sites;",
        "FinalizeAwaitSuspensionProfile(*fn);",
        "FinalizeAwaitSuspensionProfile(method);",
        "await-suspension:await_suspension_sites=",
        "profile.normalized_sites + profile.gate_blocked_sites !=",
    ):
        assert marker in parser_source
