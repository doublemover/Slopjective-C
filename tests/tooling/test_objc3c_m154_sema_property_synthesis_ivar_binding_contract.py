from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m154_sema_property_synthesis_ivar_binding_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M154 sema/type property synthesis and ivar binding semantics contract (M154-B001)",
        "Objc3PropertySynthesisIvarBindingSummary",
        "property_synthesis_ivar_binding_summary",
        "deterministic_property_synthesis_ivar_binding_handoff",
        "python -m pytest tests/tooling/test_objc3c_m154_sema_property_synthesis_ivar_binding_contract.py -q",
    ):
        assert text in fragment


def test_m154_sema_property_synthesis_ivar_binding_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3PropertySynthesisIvarBindingSummary",
        "std::size_t property_synthesis_sites = 0;",
        "std::size_t property_synthesis_explicit_ivar_bindings = 0;",
        "std::size_t property_synthesis_default_ivar_bindings = 0;",
        "std::size_t ivar_binding_sites = 0;",
        "std::size_t ivar_binding_resolved = 0;",
        "std::size_t ivar_binding_missing = 0;",
        "std::size_t ivar_binding_conflicts = 0;",
        "Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "property_synthesis_ivar_binding_property_synthesis_sites_total",
        "property_synthesis_ivar_binding_explicit_ivar_bindings_total",
        "property_synthesis_ivar_binding_default_ivar_bindings_total",
        "property_synthesis_ivar_binding_ivar_binding_sites_total",
        "property_synthesis_ivar_binding_ivar_binding_resolved_total",
        "property_synthesis_ivar_binding_ivar_binding_missing_total",
        "property_synthesis_ivar_binding_ivar_binding_conflicts_total",
        "deterministic_property_synthesis_ivar_binding_handoff",
        "Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.property_synthesis_ivar_binding_summary =",
        "result.deterministic_property_synthesis_ivar_binding_handoff =",
        "result.parity_surface.property_synthesis_ivar_binding_summary =",
        "result.parity_surface.property_synthesis_ivar_binding_property_synthesis_sites_total =",
        "result.parity_surface.deterministic_property_synthesis_ivar_binding_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildPropertySynthesisIvarBindingSummaryFromIntegrationSurface(",
        "BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(",
        "surface.property_synthesis_ivar_binding_summary =",
        "handoff.property_synthesis_ivar_binding_summary =",
        "handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites ==",
    ):
        assert marker in sema_passes
