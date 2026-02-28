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


def test_m151_lowering_symbol_graph_scope_resolution_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Symbol-graph/scope-resolution lowering artifact contract (M151-C001)",
        "frontend.pipeline.sema_pass_manager.deterministic_symbol_graph_handoff",
        "frontend.pipeline.sema_pass_manager.deterministic_scope_resolution_handoff",
        "frontend.pipeline.semantic_surface.objc_symbol_graph_scope_resolution_surface",
        "!objc3.objc_symbol_graph_scope_resolution = !{!6}",
        "python -m pytest tests/tooling/test_objc3c_m151_lowering_symbol_graph_scope_resolution_contract.py -q",
    ):
        assert text in fragment


def test_m151_lowering_symbol_graph_scope_resolution_markers_map_to_sources() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "struct Objc3FrontendSymbolGraphScopeResolutionSummary",
        "bool deterministic_symbol_graph_handoff = true;",
        "bool deterministic_scope_resolution_handoff = true;",
        "std::string deterministic_handoff_key;",
        "Objc3FrontendSymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;",
    ):
        assert marker in pipeline_types

    for marker in (
        "BuildSymbolGraphScopeResolutionHandoffKey(",
        "BuildSymbolGraphScopeResolutionSummary(",
        "integration_surface.symbol_graph_scope_resolution_summary;",
        "type_metadata_handoff.symbol_graph_scope_resolution_summary;",
        "summary.deterministic_symbol_graph_handoff =",
        "summary.deterministic_scope_resolution_handoff =",
        "result.symbol_graph_scope_resolution_summary =",
    ):
        assert marker in pipeline_source

    for marker in (
        '<< ",\\"symbol_graph_global_symbol_nodes\\":"',
        '<< ",\\"deterministic_symbol_graph_handoff\\":"',
        '<< ",\\"symbol_graph_scope_resolution_handoff_key\\":\\""',
        '<< ",\\"objc_symbol_graph_scope_resolution_surface\\":{\\"global_symbol_nodes\\":"',
        "ir_frontend_metadata.global_symbol_nodes =",
        "ir_frontend_metadata.deterministic_symbol_graph_scope_resolution_handoff_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::size_t global_symbol_nodes = 0;",
        "std::size_t scope_frames_total = 0;",
        "std::size_t method_resolution_hits = 0;",
        "bool deterministic_symbol_graph_handoff = false;",
        "bool deterministic_scope_resolution_handoff = false;",
        "std::string deterministic_symbol_graph_scope_resolution_handoff_key;",
    ):
        assert marker in ir_header

    for marker in (
        "frontend_objc_symbol_graph_scope_resolution_profile",
        "!objc3.objc_symbol_graph_scope_resolution = !{!6}",
        "frontend_metadata_.global_symbol_nodes",
        "frontend_metadata_.deterministic_scope_resolution_handoff",
        "EscapeCStringLiteral(frontend_metadata_.deterministic_symbol_graph_scope_resolution_handoff_key)",
    ):
        assert marker in ir_source
