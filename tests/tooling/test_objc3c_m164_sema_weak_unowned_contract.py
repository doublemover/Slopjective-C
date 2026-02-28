from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m164_sema_weak_unowned_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M164 sema/type weak-unowned semantics contract (M164-B001)",
        "Objc3WeakUnownedSemanticsSummary",
        "BuildWeakUnownedSemanticsSummaryFromIntegrationSurface",
        "BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff",
        "weak_unowned_semantics_ownership_candidate_sites_total",
        "weak_unowned_semantics_unowned_safe_reference_sites_total",
        "deterministic_weak_unowned_semantics_handoff",
        "python -m pytest tests/tooling/test_objc3c_m164_sema_weak_unowned_contract.py -q",
    ):
        assert text in fragment


def test_m164_sema_weak_unowned_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3WeakUnownedSemanticsSummary {",
        "std::size_t ownership_candidate_sites = 0;",
        "std::size_t weak_reference_sites = 0;",
        "std::size_t unowned_reference_sites = 0;",
        "std::size_t unowned_safe_reference_sites = 0;",
        "std::size_t weak_unowned_conflict_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::vector<bool> param_ownership_is_weak_reference;",
        "std::vector<bool> param_ownership_is_unowned_reference;",
        "std::vector<bool> param_ownership_is_unowned_safe_reference;",
        "bool return_ownership_is_weak_reference = false;",
        "bool return_ownership_is_unowned_reference = false;",
        "bool return_ownership_is_unowned_safe_reference = false;",
        "bool ownership_is_weak_reference = false;",
        "bool ownership_is_unowned_reference = false;",
        "bool ownership_is_unowned_safe_reference = false;",
        "bool is_unowned = false;",
        "bool has_weak_unowned_conflict = false;",
        "Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "weak_unowned_semantics_ownership_candidate_sites_total",
        "weak_unowned_semantics_weak_reference_sites_total",
        "weak_unowned_semantics_unowned_reference_sites_total",
        "weak_unowned_semantics_unowned_safe_reference_sites_total",
        "weak_unowned_semantics_conflict_sites_total",
        "weak_unowned_semantics_contract_violation_sites_total",
        "deterministic_weak_unowned_semantics_handoff",
        "surface.weak_unowned_semantics_summary.ownership_candidate_sites ==",
        "surface.weak_unowned_semantics_summary.unowned_safe_reference_sites <=",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentWeakUnownedSemanticsSummary",
        "result.weak_unowned_semantics_summary = result.integration_surface.weak_unowned_semantics_summary;",
        "result.deterministic_weak_unowned_semantics_handoff =",
        "result.parity_surface.weak_unowned_semantics_summary =",
        "result.parity_surface.weak_unowned_semantics_ownership_candidate_sites_total =",
        "result.parity_surface.deterministic_weak_unowned_semantics_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildWeakUnownedSemanticsSummaryFromIntegrationSurface",
        "BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff",
        "surface.weak_unowned_semantics_summary =",
        "handoff.weak_unowned_semantics_summary =",
        "info.param_ownership_is_weak_reference.push_back(param.ownership_is_weak_reference);",
        "info.return_ownership_is_unowned_reference = fn.return_ownership_is_unowned_reference;",
        "property_metadata.has_weak_unowned_conflict = source.has_weak_unowned_conflict;",
    ):
        assert marker in sema_passes
