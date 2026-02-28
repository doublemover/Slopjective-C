from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
PIPELINE_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m149_lowering_property_attribute_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Property-attribute lowering artifact contract (M149-C001)",
        "frontend.pipeline.sema_pass_manager.deterministic_property_attribute_handoff",
        "frontend.pipeline.semantic_surface.objc_property_attribute_surface",
        "!objc3.objc_property_attribute = !{!4}",
        "python -m pytest tests/tooling/test_objc3c_m149_lowering_property_attribute_contract.py -q",
    ):
        assert text in fragment


def test_m149_lowering_property_attribute_markers_map_to_sources() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "struct Objc3FrontendPropertyAttributeSummary",
        "Objc3FrontendPropertyAttributeSummary property_attribute_summary;",
    ):
        assert marker in pipeline_types

    for marker in (
        "AccumulatePropertyAttributeSummary(",
        "BuildPropertyAttributeSummary(",
        "summary.property_accessor_modifier_entries += accessor_modifier_entries;",
        "summary.deterministic_property_attribute_handoff =",
        "result.property_attribute_summary =",
    ):
        assert marker in pipeline_source

    for marker in (
        '<< ",\\"deterministic_property_attribute_handoff\\":"',
        '<< ",\\"property_accessor_modifier_entries\\":"',
        '<< ",\\"objc_property_attribute_surface\\":{\\"property_declaration_entries\\":"',
        "ir_frontend_metadata.property_declaration_entries =",
        "ir_frontend_metadata.deterministic_property_attribute_handoff =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::size_t property_declaration_entries = 0;",
        "std::size_t property_attribute_entries = 0;",
        "std::size_t property_accessor_modifier_entries = 0;",
        "std::size_t property_setter_selector_entries = 0;",
        "bool deterministic_property_attribute_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        "frontend_objc_property_attribute_profile",
        "!objc3.objc_property_attribute = !{!4}",
        "frontend_metadata_.property_declaration_entries",
        "frontend_metadata_.deterministic_property_attribute_handoff",
    ):
        assert marker in ir_source
