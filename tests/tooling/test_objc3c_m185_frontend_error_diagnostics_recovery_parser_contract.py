from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m185_frontend_error_diagnostics_recovery_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M185 frontend error diagnostics UX and recovery packetization",
        "IsErrorDiagnosticSymbol(...)",
        "IsRecoveryAnchorSymbol(...)",
        "IsRecoveryBoundarySymbol(...)",
        "IsFailClosedDiagnosticSymbol(...)",
        "struct Objc3ErrorDiagnosticsRecoveryProfile",
        "BuildErrorDiagnosticsRecoveryProfile(...)",
        "IsErrorDiagnosticsRecoveryProfileNormalized(...)",
        "FinalizeErrorDiagnosticsRecoveryProfile(FunctionDecl &fn)",
        "FinalizeErrorDiagnosticsRecoveryProfile(Objc3MethodDecl &method)",
        "fn.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;",
        "method.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;",
        "bool error_diagnostics_recovery_profile_is_normalized = false;",
        "bool deterministic_error_diagnostics_recovery_handoff = false;",
        "std::string error_diagnostics_recovery_profile;",
        "python -m pytest tests/tooling/test_objc3c_m185_frontend_error_diagnostics_recovery_parser_contract.py -q",
    ):
        assert text in fragment


def test_m185_frontend_error_diagnostics_recovery_markers_map_to_sources() -> None:
    ast_header = _read(AST_HEADER)
    parser_source = _read(PARSER_SOURCE)

    for marker in (
        "bool error_diagnostics_recovery_profile_is_normalized = false;",
        "bool deterministic_error_diagnostics_recovery_handoff = false;",
        "std::size_t error_diagnostics_recovery_sites = 0;",
        "std::size_t diagnostic_emit_sites = 0;",
        "std::size_t recovery_anchor_sites = 0;",
        "std::size_t recovery_boundary_sites = 0;",
        "std::size_t fail_closed_diagnostic_sites = 0;",
        "std::size_t error_diagnostics_recovery_normalized_sites = 0;",
        "std::size_t error_diagnostics_recovery_gate_blocked_sites = 0;",
        "std::size_t error_diagnostics_recovery_contract_violation_sites = 0;",
        "std::string error_diagnostics_recovery_profile;",
    ):
        assert marker in ast_header

    for marker in (
        "IsErrorDiagnosticSymbol(",
        "IsRecoveryAnchorSymbol(",
        "IsRecoveryBoundarySymbol(",
        "IsFailClosedDiagnosticSymbol(",
        "CollectErrorDiagnosticsRecoveryExprSites(",
        "CollectErrorDiagnosticsRecoveryStmtSites(",
        "CountErrorDiagnosticsRecoverySitesInBody(",
        "struct Objc3ErrorDiagnosticsRecoveryProfile {",
        "BuildErrorDiagnosticsRecoveryProfile(",
        "IsErrorDiagnosticsRecoveryProfileNormalized(",
        "BuildErrorDiagnosticsRecoveryProfileFromCounts(",
        "BuildErrorDiagnosticsRecoveryProfileFromFunction(",
        "BuildErrorDiagnosticsRecoveryProfileFromOpaqueBody(",
        "FinalizeErrorDiagnosticsRecoveryProfile(FunctionDecl &fn)",
        "FinalizeErrorDiagnosticsRecoveryProfile(Objc3MethodDecl &method)",
        "target.error_diagnostics_recovery_profile =",
        "fn.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;",
        "method.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;",
        "FinalizeErrorDiagnosticsRecoveryProfile(*fn);",
        "FinalizeErrorDiagnosticsRecoveryProfile(method);",
        "error-diagnostics-recovery:error_diagnostics_recovery_sites=",
        "profile.normalized_sites + profile.gate_blocked_sites !=",
    ):
        assert marker in parser_source
