from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m153_sema_method_lookup_override_conflict_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M153 sema/type method lookup, override, and conflict semantics contract (M153-B001)",
        "Objc3MethodLookupOverrideConflictSummary",
        "method_lookup_override_conflict_summary",
        "deterministic_method_lookup_override_conflict_handoff",
        "python -m pytest tests/tooling/test_objc3c_m153_sema_method_lookup_override_conflict_contract.py -q",
    ):
        assert text in fragment


def test_m153_sema_method_lookup_override_conflict_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3MethodLookupOverrideConflictSummary",
        "std::size_t method_lookup_sites = 0;",
        "std::size_t method_lookup_hits = 0;",
        "std::size_t method_lookup_misses = 0;",
        "std::size_t override_lookup_sites = 0;",
        "std::size_t override_lookup_hits = 0;",
        "std::size_t override_lookup_misses = 0;",
        "std::size_t override_conflicts = 0;",
        "std::size_t unresolved_base_interfaces = 0;",
        "Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "method_lookup_override_conflict_lookup_sites_total",
        "method_lookup_override_conflict_lookup_hits_total",
        "method_lookup_override_conflict_lookup_misses_total",
        "method_lookup_override_conflict_override_sites_total",
        "method_lookup_override_conflict_override_hits_total",
        "method_lookup_override_conflict_override_misses_total",
        "method_lookup_override_conflict_override_conflicts_total",
        "method_lookup_override_conflict_unresolved_base_interfaces_total",
        "deterministic_method_lookup_override_conflict_handoff",
        "Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.method_lookup_override_conflict_summary =",
        "result.deterministic_method_lookup_override_conflict_handoff =",
        "result.parity_surface.method_lookup_override_conflict_summary =",
        "result.parity_surface.method_lookup_override_conflict_lookup_sites_total =",
        "result.parity_surface.deterministic_method_lookup_override_conflict_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildMethodLookupOverrideConflictSummaryFromIntegrationSurface(",
        "BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(",
        "surface.method_lookup_override_conflict_summary =",
        "handoff.method_lookup_override_conflict_summary =",
        "handoff.method_lookup_override_conflict_summary.override_conflicts <=",
    ):
        assert marker in sema_passes
