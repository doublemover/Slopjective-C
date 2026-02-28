from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m155_sema_id_class_sel_object_pointer_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M155 sema/type id/class/SEL/object-pointer type-checking contract (M155-B001)",
        "Objc3IdClassSelObjectPointerTypeCheckingSummary",
        "id_class_sel_object_pointer_type_checking_summary",
        "deterministic_id_class_sel_object_pointer_type_checking_handoff",
        "python -m pytest tests/tooling/test_objc3c_m155_sema_id_class_sel_object_pointer_typecheck_contract.py -q",
    ):
        assert text in fragment


def test_m155_sema_id_class_sel_object_pointer_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3IdClassSelObjectPointerTypeCheckingSummary",
        "std::size_t param_id_spelling_sites = 0;",
        "std::size_t param_class_spelling_sites = 0;",
        "std::size_t param_sel_spelling_sites = 0;",
        "std::size_t param_instancetype_spelling_sites = 0;",
        "std::size_t param_object_pointer_type_sites = 0;",
        "std::size_t return_id_spelling_sites = 0;",
        "std::size_t return_class_spelling_sites = 0;",
        "std::size_t return_sel_spelling_sites = 0;",
        "std::size_t return_instancetype_spelling_sites = 0;",
        "std::size_t return_object_pointer_type_sites = 0;",
        "std::size_t property_id_spelling_sites = 0;",
        "std::size_t property_class_spelling_sites = 0;",
        "std::size_t property_sel_spelling_sites = 0;",
        "std::size_t property_instancetype_spelling_sites = 0;",
        "std::size_t property_object_pointer_type_sites = 0;",
        "Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "id_class_sel_object_pointer_param_type_sites_total",
        "id_class_sel_object_pointer_param_id_spelling_sites_total",
        "id_class_sel_object_pointer_param_class_spelling_sites_total",
        "id_class_sel_object_pointer_param_sel_spelling_sites_total",
        "id_class_sel_object_pointer_return_type_sites_total",
        "id_class_sel_object_pointer_return_sel_spelling_sites_total",
        "id_class_sel_object_pointer_property_type_sites_total",
        "id_class_sel_object_pointer_property_sel_spelling_sites_total",
        "deterministic_id_class_sel_object_pointer_type_checking_handoff",
        "Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.id_class_sel_object_pointer_type_checking_summary =",
        "result.deterministic_id_class_sel_object_pointer_type_checking_handoff =",
        "result.parity_surface.id_class_sel_object_pointer_type_checking_summary =",
        "result.parity_surface.id_class_sel_object_pointer_param_type_sites_total =",
        "result.parity_surface.deterministic_id_class_sel_object_pointer_type_checking_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildIdClassSelObjectPointerTypeCheckingSummaryFromIntegrationSurface(",
        "BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(",
        "surface.id_class_sel_object_pointer_type_checking_summary =",
        "handoff.id_class_sel_object_pointer_type_checking_summary =",
        "handoff.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites",
    ):
        assert marker in sema_passes
