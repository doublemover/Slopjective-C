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


def test_m147_lowering_protocol_category_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Protocol/category lowering artifact contract (M147-C001)",
        "frontend.pipeline.sema_pass_manager.deterministic_protocol_category_handoff",
        "frontend.pipeline.semantic_surface.objc_protocol_category_surface",
        "!objc3.objc_protocol_category = !{!2}",
        "python -m pytest tests/tooling/test_objc3c_m147_lowering_protocol_category_contract.py -q",
    ):
        assert text in fragment


def test_m147_lowering_protocol_category_markers_map_to_sources() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    assert "Objc3FrontendProtocolCategorySummary protocol_category_summary;" in pipeline_types

    for marker in (
        "BuildProtocolCategorySummary(",
        "summary.declared_protocols = CountProtocols(program);",
        "summary.deterministic_protocol_category_handoff =",
        "result.protocol_category_summary =",
    ):
        assert marker in pipeline_source

    for marker in (
        '<< ",\\"deterministic_protocol_category_handoff\\":"',
        '<< ",\\"declared_protocols\\":" << protocol_category_summary.declared_protocols',
        '<< ",\\"objc_protocol_category_surface\\":{\\"protocol_method_symbols\\":"',
        'manifest << "  \\"protocols\\": [\\n";',
        'manifest << "  \\"categories\\": [\\n";',
        "ir_frontend_metadata.declared_protocols = protocol_category_summary.declared_protocols;",
        "ir_frontend_metadata.deterministic_protocol_category_handoff =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::size_t declared_protocols = 0;",
        "std::size_t declared_categories = 0;",
        "std::size_t linked_category_symbols = 0;",
        "bool deterministic_protocol_category_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        "frontend_objc_protocol_category_profile",
        "!objc3.objc_protocol_category = !{!2}",
        "frontend_metadata_.declared_protocols",
        "frontend_metadata_.linked_category_symbols",
    ):
        assert marker in ir_source
