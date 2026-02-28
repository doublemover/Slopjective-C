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


def test_m148_lowering_selector_normalization_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Selector-normalization lowering artifact contract (M148-C001)",
        "frontend.pipeline.sema_pass_manager.deterministic_selector_normalization_handoff",
        "frontend.pipeline.semantic_surface.objc_selector_normalization_surface",
        "!objc3.objc_selector_normalization = !{!3}",
        "python -m pytest tests/tooling/test_objc3c_m148_lowering_selector_normalization_contract.py -q",
    ):
        assert text in fragment


def test_m148_lowering_selector_normalization_markers_map_to_sources() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "struct Objc3FrontendSelectorNormalizationSummary",
        "Objc3FrontendSelectorNormalizationSummary selector_normalization_summary;",
    ):
        assert marker in pipeline_types

    for marker in (
        "BuildSelectorNormalizationSummary(",
        "summary.selector_piece_entries += method.selector_pieces.size();",
        "summary.deterministic_selector_normalization_handoff =",
        "result.selector_normalization_summary =",
    ):
        assert marker in pipeline_source

    for marker in (
        '<< ",\\"deterministic_selector_normalization_handoff\\":"',
        '<< ",\\"selector_method_declaration_entries\\":"',
        '<< ",\\"objc_selector_normalization_surface\\":{\\"method_declaration_entries\\":"',
        "ir_frontend_metadata.selector_method_declaration_entries =",
        "ir_frontend_metadata.deterministic_selector_normalization_handoff =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::size_t selector_method_declaration_entries = 0;",
        "std::size_t selector_normalized_method_declarations = 0;",
        "std::size_t selector_piece_entries = 0;",
        "std::size_t selector_piece_parameter_links = 0;",
        "bool deterministic_selector_normalization_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        "frontend_objc_selector_normalization_profile",
        "!objc3.objc_selector_normalization = !{!3}",
        "frontend_metadata_.selector_piece_entries",
        "frontend_metadata_.deterministic_selector_normalization_handoff",
    ):
        assert marker in ir_source
