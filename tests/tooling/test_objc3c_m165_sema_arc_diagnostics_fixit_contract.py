from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m165_sema_arc_diagnostics_fixit_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M165 sema/type ARC diagnostics-fixit contract (M165-B001)",
        "Objc3ArcDiagnosticsFixitSummary",
        "BuildArcDiagnosticsFixitSummaryFromIntegrationSurface",
        "BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff",
        "ownership_arc_diagnostic_candidate_sites_total",
        "ownership_arc_weak_unowned_conflict_diagnostic_sites_total",
        "deterministic_arc_diagnostics_fixit_handoff",
        "python -m pytest tests/tooling/test_objc3c_m165_sema_arc_diagnostics_fixit_contract.py -q",
    ):
        assert text in fragment


def test_m165_sema_arc_diagnostics_fixit_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ArcDiagnosticsFixitSummary {",
        "std::size_t ownership_arc_diagnostic_candidate_sites = 0;",
        "std::size_t ownership_arc_fixit_available_sites = 0;",
        "std::size_t ownership_arc_profiled_sites = 0;",
        "std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;",
        "std::size_t ownership_arc_empty_fixit_hint_sites = 0;",
        "std::vector<bool> param_ownership_arc_diagnostic_candidate;",
        "std::vector<bool> param_ownership_arc_fixit_available;",
        "std::vector<std::string> param_ownership_arc_diagnostic_profile;",
        "std::vector<std::string> param_ownership_arc_fixit_hint;",
        "bool return_ownership_arc_diagnostic_candidate = false;",
        "bool return_ownership_arc_fixit_available = false;",
        "std::string return_ownership_arc_diagnostic_profile;",
        "std::string return_ownership_arc_fixit_hint;",
        "bool ownership_arc_diagnostic_candidate = false;",
        "bool ownership_arc_fixit_available = false;",
        "std::string ownership_arc_diagnostic_profile;",
        "std::string ownership_arc_fixit_hint;",
        "Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "ownership_arc_diagnostic_candidate_sites_total",
        "ownership_arc_fixit_available_sites_total",
        "ownership_arc_profiled_sites_total",
        "ownership_arc_weak_unowned_conflict_diagnostic_sites_total",
        "ownership_arc_empty_fixit_hint_sites_total",
        "ownership_arc_contract_violation_sites_total",
        "deterministic_arc_diagnostics_fixit_handoff",
        "surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentArcDiagnosticsFixitSummary",
        "result.arc_diagnostics_fixit_summary = result.integration_surface.arc_diagnostics_fixit_summary;",
        "result.deterministic_arc_diagnostics_fixit_handoff =",
        "result.parity_surface.arc_diagnostics_fixit_summary =",
        "result.parity_surface.ownership_arc_diagnostic_candidate_sites_total =",
        "result.parity_surface.deterministic_arc_diagnostics_fixit_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildArcDiagnosticsFixitSummaryFromIntegrationSurface",
        "BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff",
        "surface.arc_diagnostics_fixit_summary =",
        "handoff.arc_diagnostics_fixit_summary =",
        "metadata.param_ownership_arc_diagnostic_candidate = source.param_ownership_arc_diagnostic_candidate;",
        "method_metadata.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;",
        "property_metadata.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;",
    ):
        assert marker in sema_passes
