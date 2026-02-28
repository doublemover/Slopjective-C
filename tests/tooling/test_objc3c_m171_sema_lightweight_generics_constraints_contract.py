from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m171_sema_lightweight_generics_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M171 sema/type lightweight generics semantic constraints contract (M171-B001)",
        "Objc3LightweightGenericConstraintSummary",
        "BuildLightweightGenericConstraintSummaryFromTypeAnnotationSurfaceSummary",
        "lightweight_generic_constraint_sites_total",
        "deterministic_lightweight_generic_constraint_handoff",
        "python -m pytest tests/tooling/test_objc3c_m171_sema_lightweight_generics_constraints_contract.py -q",
    ):
        assert text in fragment


def test_m171_sema_lightweight_generics_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3LightweightGenericConstraintSummary {",
        "std::size_t generic_constraint_sites = 0;",
        "std::size_t generic_suffix_sites = 0;",
        "std::size_t object_pointer_type_sites = 0;",
        "std::size_t terminated_generic_suffix_sites = 0;",
        "std::size_t normalized_constraint_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "lightweight_generic_constraint_sites_total",
        "lightweight_generic_constraint_normalized_sites_total",
        "lightweight_generic_constraint_contract_violation_sites_total",
        "deterministic_lightweight_generic_constraint_handoff",
        "surface.lightweight_generic_constraint_summary.generic_constraint_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentLightweightGenericConstraintSummary",
        "result.lightweight_generic_constraint_summary =",
        "result.deterministic_lightweight_generic_constraint_handoff =",
        "result.parity_surface.lightweight_generic_constraint_summary =",
        "result.parity_surface.lightweight_generic_constraint_sites_total =",
        "result.parity_surface.deterministic_lightweight_generic_constraint_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildLightweightGenericConstraintSummaryFromTypeAnnotationSurfaceSummary(",
        "surface.lightweight_generic_constraint_summary =",
        "handoff.lightweight_generic_constraint_summary =",
        "handoff.lightweight_generic_constraint_summary.deterministic",
    ):
        assert marker in sema_passes
