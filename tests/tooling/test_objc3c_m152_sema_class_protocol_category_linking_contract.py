from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m152_sema_class_protocol_category_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M152 sema/type class-protocol-category semantic linking contract (M152-B001)",
        "Objc3ClassProtocolCategoryLinkingSummary",
        "class_protocol_category_linking_summary",
        "deterministic_class_protocol_category_linking_handoff",
        "python -m pytest tests/tooling/test_objc3c_m152_sema_class_protocol_category_linking_contract.py -q",
    ):
        assert text in fragment


def test_m152_sema_class_protocol_category_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ClassProtocolCategoryLinkingSummary",
        "std::size_t declared_interfaces = 0;",
        "std::size_t resolved_interfaces = 0;",
        "std::size_t declared_implementations = 0;",
        "std::size_t resolved_implementations = 0;",
        "std::size_t linked_implementation_symbols = 0;",
        "std::size_t protocol_composition_sites = 0;",
        "std::size_t category_composition_sites = 0;",
        "std::size_t invalid_protocol_composition_sites = 0;",
        "Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "bool deterministic_class_protocol_category_linking_handoff = false;",
        "Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;",
        "surface.class_protocol_category_linking_summary.declared_interfaces ==",
        "surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.class_protocol_category_linking_summary = result.integration_surface.class_protocol_category_linking_summary;",
        "result.deterministic_class_protocol_category_linking_handoff =",
        "result.parity_surface.class_protocol_category_linking_summary =",
        "result.parity_surface.deterministic_class_protocol_category_linking_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildClassProtocolCategoryLinkingSummary(",
        "surface.class_protocol_category_linking_summary =",
        "handoff.class_protocol_category_linking_summary =",
        "summary.invalid_protocol_composition_sites <= summary.total_composition_sites()",
    ):
        assert marker in sema_passes
