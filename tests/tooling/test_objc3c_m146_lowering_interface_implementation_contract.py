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


def test_m146_lowering_interface_implementation_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Interface/implementation lowering artifact contract (M146-C001)",
        "frontend.pipeline.sema_pass_manager.deterministic_interface_implementation_handoff",
        "frontend.pipeline.semantic_surface.objc_interface_implementation_surface",
        "!objc3.objc_interface_implementation = !{!1}",
        "python -m pytest tests/tooling/test_objc3c_m146_lowering_interface_implementation_contract.py -q",
    ):
        assert text in fragment


def test_m146_lowering_interface_implementation_markers_map_to_sources() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    assert "Objc3SemanticTypeMetadataHandoff sema_type_metadata_handoff;" in pipeline_types
    assert "result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);" in pipeline_source

    for marker in (
        'manifest << "  \\\"interfaces\\\": [\\n";',
        'manifest << "  \\\"implementations\\\": [\\n";',
        '<< ",\\\"deterministic_interface_implementation_handoff\\\":"',
        '<< ",\\\"declared_interfaces\\\":" << program.interfaces.size()',
        '<< ",\\\"declared_implementations\\\":" << program.implementations.size()',
        'ir_frontend_metadata.declared_interfaces = interface_implementation_summary.declared_interfaces;',
        'ir_frontend_metadata.linked_implementation_symbols = interface_implementation_summary.linked_implementation_symbols;',
    ):
        assert marker in artifacts_source

    for marker in (
        "std::size_t declared_interfaces = 0;",
        "std::size_t declared_implementations = 0;",
        "std::size_t linked_implementation_symbols = 0;",
        "bool deterministic_interface_implementation_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        "frontend_objc_interface_implementation_profile",
        "!objc3.objc_interface_implementation = !{!1}",
        "frontend_metadata_.declared_interfaces",
        "frontend_metadata_.linked_implementation_symbols",
    ):
        assert marker in ir_source
