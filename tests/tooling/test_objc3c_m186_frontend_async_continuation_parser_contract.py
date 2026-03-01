from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m186_frontend_async_continuation_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M186 frontend async grammar and continuation packetization",
        "IsAsyncKeywordSymbol(...)",
        "IsAsyncFunctionSymbol(...)",
        "IsContinuationAllocationSymbol(...)",
        "IsContinuationResumeSymbol(...)",
        "IsContinuationSuspendSymbol(...)",
        "IsAsyncStateMachineSymbol(...)",
        "struct Objc3AsyncContinuationProfile",
        "BuildAsyncContinuationProfile(...)",
        "IsAsyncContinuationProfileNormalized(...)",
        "FinalizeAsyncContinuationProfile(FunctionDecl &fn)",
        "FinalizeAsyncContinuationProfile(Objc3MethodDecl &method)",
        "fn.async_continuation_sites = profile.async_continuation_sites;",
        "method.async_continuation_sites = profile.async_continuation_sites;",
        "bool async_continuation_profile_is_normalized = false;",
        "bool deterministic_async_continuation_handoff = false;",
        "std::string async_continuation_profile;",
        "python -m pytest tests/tooling/test_objc3c_m186_frontend_async_continuation_parser_contract.py -q",
    ):
        assert text in fragment


def test_m186_frontend_async_continuation_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool async_continuation_profile_is_normalized = false;",
        "bool deterministic_async_continuation_handoff = false;",
        "std::size_t async_continuation_sites = 0;",
        "std::size_t async_keyword_sites = 0;",
        "std::size_t async_function_sites = 0;",
        "std::size_t continuation_allocation_sites = 0;",
        "std::size_t continuation_resume_sites = 0;",
        "std::size_t continuation_suspend_sites = 0;",
        "std::size_t async_state_machine_sites = 0;",
        "std::size_t async_continuation_normalized_sites = 0;",
        "std::size_t async_continuation_gate_blocked_sites = 0;",
        "std::size_t async_continuation_contract_violation_sites = 0;",
        "std::string async_continuation_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsAsyncKeywordSymbol(",
        "IsAsyncFunctionSymbol(",
        "IsContinuationAllocationSymbol(",
        "IsContinuationResumeSymbol(",
        "IsContinuationSuspendSymbol(",
        "IsAsyncStateMachineSymbol(",
        "CollectAsyncContinuationExprSites(",
        "CollectAsyncContinuationStmtSites(",
        "CountAsyncContinuationSitesInBody(",
        "struct Objc3AsyncContinuationProfile {",
        "BuildAsyncContinuationProfile(",
        "IsAsyncContinuationProfileNormalized(",
        "BuildAsyncContinuationProfileFromCounts(",
        "BuildAsyncContinuationProfileFromFunction(",
        "BuildAsyncContinuationProfileFromOpaqueBody(",
        "FinalizeAsyncContinuationProfile(FunctionDecl &fn)",
        "FinalizeAsyncContinuationProfile(Objc3MethodDecl &method)",
        "target.async_continuation_profile = source.async_continuation_profile;",
        "fn.async_continuation_sites = profile.async_continuation_sites;",
        "method.async_continuation_sites = profile.async_continuation_sites;",
        "FinalizeAsyncContinuationProfile(*fn);",
        "FinalizeAsyncContinuationProfile(method);",
        "async-continuation:async_continuation_sites=",
        "profile.normalized_sites + profile.gate_blocked_sites !=",
    ):
        assert marker in parser_source
