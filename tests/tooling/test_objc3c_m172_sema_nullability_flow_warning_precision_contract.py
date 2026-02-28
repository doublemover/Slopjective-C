from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m172_sema_nullability_flow_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M172 sema/type nullability flow and warning precision contract (M172-B001)",
        "Objc3NullabilityFlowWarningPrecisionSummary",
        "BuildNullabilityFlowWarningPrecisionSummaryFromTypeAnnotationSurfaceSummary",
        "nullability_flow_sites_total",
        "deterministic_nullability_flow_warning_precision_handoff",
        "python -m pytest tests/tooling/test_objc3c_m172_sema_nullability_flow_warning_precision_contract.py -q",
    ):
        assert text in fragment


def test_m172_sema_nullability_flow_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3NullabilityFlowWarningPrecisionSummary {",
        "std::size_t nullability_flow_sites = 0;",
        "std::size_t nullability_suffix_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "nullability_flow_sites_total",
        "nullability_flow_normalized_sites_total",
        "nullability_flow_contract_violation_sites_total",
        "deterministic_nullability_flow_warning_precision_handoff",
        "surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentNullabilityFlowWarningPrecisionSummary",
        "result.nullability_flow_warning_precision_summary =",
        "result.deterministic_nullability_flow_warning_precision_handoff =",
        "result.parity_surface.nullability_flow_warning_precision_summary =",
        "result.parity_surface.nullability_flow_sites_total =",
        "result.parity_surface.deterministic_nullability_flow_warning_precision_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildNullabilityFlowWarningPrecisionSummaryFromTypeAnnotationSurfaceSummary(",
        "surface.nullability_flow_warning_precision_summary =",
        "handoff.nullability_flow_warning_precision_summary =",
        "handoff.nullability_flow_warning_precision_summary.deterministic",
    ):
        assert marker in sema_passes
