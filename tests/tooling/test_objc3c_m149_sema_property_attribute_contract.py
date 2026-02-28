from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m149_sema_property_attribute_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M149 sema/type @property attribute and accessor modifier contract (M149-B001)",
        "Objc3PropertyAttributeSummary",
        "property_attribute_summary",
        "has_accessor_selector_contract_violation",
        "deterministic_property_attribute_handoff",
        "python -m pytest tests/tooling/test_objc3c_m149_sema_property_attribute_contract.py -q",
    ):
        assert text in fragment


def test_m149_sema_property_attribute_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3PropertyAttributeSummary",
        "struct Objc3PropertyInfo",
        "property_attribute_summary",
        "attribute_names_lexicographic",
        "has_accessor_selector_contract_violation",
        "has_invalid_attribute_contract",
    ):
        assert marker in sema_contract

    for marker in (
        "property_attribute_entries_total",
        "property_attribute_contract_violations_total",
        "deterministic_property_attribute_handoff",
        "Objc3PropertyAttributeSummary property_attribute_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.deterministic_property_attribute_handoff =",
        "result.parity_surface.property_attribute_summary =",
        "result.parity_surface.property_attribute_entries_total =",
        "result.parity_surface.property_attribute_contract_violations_total =",
        "result.parity_surface.deterministic_property_attribute_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildPropertyInfo(",
        "BuildPropertyAttributeSummaryFromSurface",
        "type mismatch: unknown @property attribute",
        "type mismatch: duplicate @property attribute",
        "type mismatch: invalid @property getter selector",
        "type mismatch: incompatible property signature",
        "surface.property_attribute_summary = BuildPropertyAttributeSummaryFromSurface(surface);",
    ):
        assert marker in sema_passes
