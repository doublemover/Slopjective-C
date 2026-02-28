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


def test_m152_lowering_class_protocol_category_linking_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Class/protocol/category linking lowering artifact contract (M152-C001)",
        "frontend.pipeline.sema_pass_manager.deterministic_class_protocol_category_linking_handoff",
        "frontend.pipeline.semantic_surface.objc_class_protocol_category_linking_surface",
        "!objc3.objc_class_protocol_category_linking = !{!7}",
        "python -m pytest tests/tooling/test_objc3c_m152_lowering_class_protocol_category_linking_contract.py -q",
    ):
        assert text in fragment


def test_m152_lowering_class_protocol_category_linking_markers_map_to_sources() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "struct Objc3FrontendClassProtocolCategoryLinkingSummary",
        "std::size_t declared_class_interfaces = 0;",
        "std::size_t linked_category_method_symbols = 0;",
        "bool deterministic_class_protocol_category_linking_handoff = true;",
        "Objc3FrontendClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;",
    ):
        assert marker in pipeline_types

    for marker in (
        "BuildClassProtocolCategoryLinkingSummary(",
        "integration_surface.protocol_category_composition_summary;",
        "type_metadata_handoff.protocol_category_composition_summary;",
        "summary.deterministic_class_protocol_category_linking_handoff =",
        "result.class_protocol_category_linking_summary =",
    ):
        assert marker in pipeline_source

    for marker in (
        '<< ",\\"deterministic_class_protocol_category_linking_handoff\\":"',
        '<< ",\\"class_protocol_category_declared_class_interfaces\\":"',
        '<< ",\\"class_protocol_category_protocol_composition_sites\\":"',
        '<< ",\\"objc_class_protocol_category_linking_surface\\":{\\"declared_class_interfaces\\":"',
        "ir_frontend_metadata.declared_class_interfaces =",
        "ir_frontend_metadata.deterministic_class_protocol_category_linking_handoff =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::size_t declared_class_interfaces = 0;",
        "std::size_t declared_class_implementations = 0;",
        "std::size_t linked_class_method_symbols = 0;",
        "std::size_t invalid_protocol_composition_sites = 0;",
        "bool deterministic_class_protocol_category_linking_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        "frontend_objc_class_protocol_category_linking_profile",
        "!objc3.objc_class_protocol_category_linking = !{!7}",
        "frontend_metadata_.declared_class_interfaces",
        "frontend_metadata_.invalid_protocol_composition_sites",
        "frontend_metadata_.deterministic_class_protocol_category_linking_handoff",
    ):
        assert marker in ir_source
