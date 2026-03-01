import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


M185_HEADING = "M185 sema/type error diagnostics UX and recovery contract (M185-B001)"
M185_ANCHOR = '<a id="m185-sema-type-error-diagnostics-ux-recovery-contract-m185-b001"></a>'


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _extract_h2_section(doc: str, heading: str) -> str:
    match = re.search(
        rf"^## {re.escape(heading)}\n(?P<body>.*?)(?=^## |\Z)",
        doc,
        flags=re.MULTILINE | re.DOTALL,
    )
    assert match is not None, f"missing section heading: {heading}"
    return match.group(0)


def _assert_regex(text: str, pattern: str) -> None:
    assert re.search(pattern, text, flags=re.MULTILINE | re.DOTALL), pattern


def test_m185_sema_error_diagnostics_recovery_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M185_ANCHOR)}\s*\n## {re.escape(M185_HEADING)}",
    )

    section = _extract_h2_section(fragment, M185_HEADING)
    for marker in (
        "Objc3ErrorDiagnosticsRecoverySummary",
        "BuildErrorDiagnosticsRecoverySummaryFromProgramAst",
        "error_diagnostics_recovery_sites_total",
        "error_diagnostics_recovery_normalized_sites_total + error_diagnostics_recovery_gate_blocked_sites_total == error_diagnostics_recovery_sites_total",
        "deterministic_error_diagnostics_recovery_handoff",
        "python -m pytest tests/tooling/test_objc3c_m185_sema_error_diagnostics_recovery_contract.py -q",
    ):
        assert marker in section


def test_m185_sema_error_diagnostics_recovery_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ErrorDiagnosticsRecoverySummary {",
        "std::size_t error_diagnostics_recovery_sites = 0;",
        "std::size_t diagnostic_emit_sites = 0;",
        "std::size_t recovery_anchor_sites = 0;",
        "std::size_t recovery_boundary_sites = 0;",
        "std::size_t fail_closed_diagnostic_sites = 0;",
        "Objc3ErrorDiagnosticsRecoverySummary error_diagnostics_recovery_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "error_diagnostics_recovery_sites_total",
        "error_diagnostics_recovery_diagnostic_emit_sites_total",
        "error_diagnostics_recovery_recovery_anchor_sites_total",
        "error_diagnostics_recovery_recovery_boundary_sites_total",
        "error_diagnostics_recovery_fail_closed_diagnostic_sites_total",
        "error_diagnostics_recovery_normalized_sites_total",
        "error_diagnostics_recovery_gate_blocked_sites_total",
        "error_diagnostics_recovery_contract_violation_sites_total",
        "deterministic_error_diagnostics_recovery_handoff",
        "surface.error_diagnostics_recovery_summary",
    ):
        assert marker in sema_pass_manager_contract

    _assert_regex(
        sema_pass_manager_contract,
        r"surface\.error_diagnostics_recovery_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"surface\.error_diagnostics_recovery_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"surface\.error_diagnostics_recovery_summary\s*"
        r"\.error_diagnostics_recovery_sites",
    )

    for marker in (
        "IsEquivalentErrorDiagnosticsRecoverySummary",
        "result.error_diagnostics_recovery_summary =",
        "result.deterministic_error_diagnostics_recovery_handoff =",
        "result.parity_surface.error_diagnostics_recovery_summary =",
        "result.parity_surface.error_diagnostics_recovery_sites_total =",
        "result.parity_surface.deterministic_error_diagnostics_recovery_handoff =",
    ):
        assert marker in sema_pass_manager

    _assert_regex(
        sema_pass_manager,
        r"result\.type_metadata_handoff\.error_diagnostics_recovery_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.error_diagnostics_recovery_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\.error_diagnostics_recovery_summary\s*"
        r"\.error_diagnostics_recovery_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"result\.parity_surface\.error_diagnostics_recovery_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.error_diagnostics_recovery_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.parity_surface\.error_diagnostics_recovery_summary\s*"
        r"\.error_diagnostics_recovery_sites",
    )

    for marker in (
        "BuildErrorDiagnosticsRecoverySummaryFromProgramAst(",
        "surface.error_diagnostics_recovery_summary =",
        "handoff.error_diagnostics_recovery_summary =",
        "handoff.error_diagnostics_recovery_summary.deterministic",
    ):
        assert marker in sema_passes

    _assert_regex(
        sema_passes,
        r"is_partitioned\(\s*declaration\.error_diagnostics_recovery_normalized_sites,\s*"
        r"declaration\.error_diagnostics_recovery_gate_blocked_sites,\s*"
        r"declaration\.error_diagnostics_recovery_sites\s*\)",
    )
    _assert_regex(
        sema_passes,
        r"is_partitioned\(\s*summary\.normalized_sites,\s*summary\.gate_blocked_sites,\s*"
        r"summary\.error_diagnostics_recovery_sites\s*\)",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.error_diagnostics_recovery_summary\s*\.normalized_sites\s*\+\s*"
        r"handoff\.error_diagnostics_recovery_summary\s*\.gate_blocked_sites\s*==\s*"
        r"handoff\.error_diagnostics_recovery_summary\s*"
        r"\.error_diagnostics_recovery_sites",
    )
