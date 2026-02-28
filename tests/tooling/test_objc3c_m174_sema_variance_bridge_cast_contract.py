from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m174_sema_variance_bridge_cast_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M174 sema/type variance and bridged-cast contract (M174-B001)",
        "Objc3VarianceBridgeCastSummary",
        "BuildVarianceBridgeCastSummaryFromTypeAnnotationAndProtocolSummary",
        "variance_bridge_cast_sites_total",
        "deterministic_variance_bridge_cast_handoff",
        "python -m pytest tests/tooling/test_objc3c_m174_sema_variance_bridge_cast_contract.py -q",
    ):
        assert text in fragment


def test_m174_sema_variance_bridge_cast_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3VarianceBridgeCastSummary {",
        "std::size_t variance_bridge_cast_sites = 0;",
        "std::size_t protocol_composition_sites = 0;",
        "std::size_t ownership_qualifier_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "variance_bridge_cast_sites_total",
        "variance_bridge_cast_protocol_composition_sites_total",
        "variance_bridge_cast_contract_violation_sites_total",
        "deterministic_variance_bridge_cast_handoff",
        "surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentVarianceBridgeCastSummary",
        "result.variance_bridge_cast_summary =",
        "result.deterministic_variance_bridge_cast_handoff =",
        "result.parity_surface.variance_bridge_cast_summary =",
        "result.parity_surface.variance_bridge_cast_sites_total =",
        "result.parity_surface.deterministic_variance_bridge_cast_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildVarianceBridgeCastSummaryFromTypeAnnotationAndProtocolSummary(",
        "surface.variance_bridge_cast_summary =",
        "handoff.variance_bridge_cast_summary =",
        "handoff.variance_bridge_cast_summary.deterministic",
    ):
        assert marker in sema_passes
