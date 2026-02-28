from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m189_frontend_task_runtime_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M189 frontend task-runtime interop and cancellation packetization",
        "IsTaskRuntimeHookSymbol(...)",
        "IsCancellationCheckSymbol(...)",
        "IsCancellationHandlerSymbol(...)",
        "IsSuspensionPointSymbol(...)",
        "struct Objc3TaskRuntimeCancellationProfile",
        "BuildTaskRuntimeCancellationProfile(...)",
        "IsTaskRuntimeCancellationProfileNormalized(...)",
        "FinalizeTaskRuntimeCancellationProfile(FunctionDecl &fn)",
        "FinalizeTaskRuntimeCancellationProfile(Objc3MethodDecl &method)",
        "fn.task_runtime_interop_sites = profile.task_runtime_interop_sites;",
        "method.task_runtime_interop_sites = profile.task_runtime_interop_sites;",
        "bool task_runtime_cancellation_profile_is_normalized = false;",
        "bool deterministic_task_runtime_cancellation_handoff = false;",
        "std::string task_runtime_cancellation_profile;",
        "python -m pytest tests/tooling/test_objc3c_m189_frontend_task_runtime_cancellation_parser_contract.py -q",
    ):
        assert text in fragment


def test_m189_frontend_task_runtime_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool task_runtime_cancellation_profile_is_normalized = false;",
        "bool deterministic_task_runtime_cancellation_handoff = false;",
        "std::size_t task_runtime_interop_sites = 0;",
        "std::size_t runtime_hook_sites = 0;",
        "std::size_t cancellation_check_sites = 0;",
        "std::size_t cancellation_handler_sites = 0;",
        "std::size_t suspension_point_sites = 0;",
        "std::size_t cancellation_propagation_sites = 0;",
        "std::size_t task_runtime_normalized_sites = 0;",
        "std::size_t task_runtime_gate_blocked_sites = 0;",
        "std::size_t task_runtime_contract_violation_sites = 0;",
        "std::string task_runtime_cancellation_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsTaskRuntimeHookSymbol(",
        "IsCancellationCheckSymbol(",
        "IsCancellationHandlerSymbol(",
        "IsSuspensionPointSymbol(",
        "CollectTaskRuntimeCancellationExprSites(",
        "CollectTaskRuntimeCancellationStmtSites(",
        "CountTaskRuntimeCancellationSitesInBody(",
        "struct Objc3TaskRuntimeCancellationProfile {",
        "BuildTaskRuntimeCancellationProfile(",
        "IsTaskRuntimeCancellationProfileNormalized(",
        "BuildTaskRuntimeCancellationProfileFromCounts(",
        "BuildTaskRuntimeCancellationProfileFromFunction(",
        "BuildTaskRuntimeCancellationProfileFromOpaqueBody(",
        "FinalizeTaskRuntimeCancellationProfile(FunctionDecl &fn)",
        "FinalizeTaskRuntimeCancellationProfile(Objc3MethodDecl &method)",
        "target.task_runtime_cancellation_profile =",
        "fn.task_runtime_interop_sites = profile.task_runtime_interop_sites;",
        "method.task_runtime_interop_sites = profile.task_runtime_interop_sites;",
        "FinalizeTaskRuntimeCancellationProfile(*fn);",
        "FinalizeTaskRuntimeCancellationProfile(method);",
        "task-runtime-cancellation:task_runtime_interop_sites=",
        "profile.normalized_sites + profile.gate_blocked_sites !=",
    ):
        assert marker in parser_source
