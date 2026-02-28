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


def test_m150_lowering_object_pointer_nullability_generics_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Object-pointer/nullability/generics lowering artifact contract (M150-C001)",
        "frontend.pipeline.sema_pass_manager.deterministic_object_pointer_nullability_generics_handoff",
        "frontend.pipeline.semantic_surface.objc_object_pointer_nullability_generics_surface",
        "!objc3.objc_object_pointer_nullability_generics = !{!5}",
        "python -m pytest tests/tooling/test_objc3c_m150_lowering_object_pointer_nullability_generics_contract.py -q",
    ):
        assert text in fragment


def test_m150_lowering_object_pointer_nullability_generics_markers_map_to_sources() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "struct Objc3FrontendObjectPointerNullabilityGenericsSummary",
        "Objc3FrontendObjectPointerNullabilityGenericsSummary object_pointer_nullability_generics_summary;",
    ):
        assert marker in pipeline_types

    for marker in (
        "AccumulateObjectPointerNullabilityGenericsTypeAnnotation(",
        "BuildObjectPointerNullabilityGenericsSummary(",
        "token.kind == Objc3SemaTokenKind::PointerDeclarator",
        "token.kind == Objc3SemaTokenKind::NullabilitySuffix",
        "result.object_pointer_nullability_generics_summary =",
    ):
        assert marker in pipeline_source

    for marker in (
        '<< ",\\"deterministic_object_pointer_nullability_generics_handoff\\":"',
        '<< ",\\"object_pointer_type_spellings\\":"',
        '<< ",\\"objc_object_pointer_nullability_generics_surface\\":{\\"object_pointer_type_spellings\\":"',
        "ir_frontend_metadata.object_pointer_type_spellings =",
        "ir_frontend_metadata.deterministic_object_pointer_nullability_generics_handoff =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::size_t object_pointer_type_spellings = 0;",
        "std::size_t pointer_declarator_entries = 0;",
        "std::size_t nullability_suffix_entries = 0;",
        "std::size_t generic_suffix_entries = 0;",
        "bool deterministic_object_pointer_nullability_generics_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        "frontend_objc_object_pointer_nullability_generics_profile",
        "!objc3.objc_object_pointer_nullability_generics = !{!5}",
        "frontend_metadata_.object_pointer_type_spellings",
        "frontend_metadata_.deterministic_object_pointer_nullability_generics_handoff",
    ):
        assert marker in ir_source
