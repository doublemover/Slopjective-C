from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m150_sema_object_pointer_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M150 sema/type object pointer, nullability, lightweight generic annotation contract (M150-B001)",
        "Objc3TypeAnnotationSurfaceSummary",
        "type_annotation_surface_summary",
        "deterministic_type_annotation_surface_handoff",
        "python -m pytest tests/tooling/test_objc3c_m150_sema_object_pointer_nullability_generics_contract.py -q",
    ):
        assert text in fragment


def test_m150_sema_object_pointer_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3TypeAnnotationSurfaceSummary",
        "std::vector<bool> param_has_generic_suffix;",
        "std::vector<bool> param_has_pointer_declarator;",
        "std::vector<bool> param_has_nullability_suffix;",
        "std::vector<bool> param_object_pointer_type_spelling;",
        "std::vector<bool> param_has_invalid_generic_suffix;",
        "std::vector<bool> param_has_invalid_pointer_declarator;",
        "std::vector<bool> param_has_invalid_nullability_suffix;",
        "bool return_has_generic_suffix = false;",
        "bool return_has_pointer_declarator = false;",
        "bool return_has_nullability_suffix = false;",
        "bool return_object_pointer_type_spelling = false;",
        "bool has_generic_suffix = false;",
        "bool has_pointer_declarator = false;",
        "bool has_nullability_suffix = false;",
        "bool object_pointer_type_spelling = false;",
    ):
        assert marker in sema_contract

    for marker in (
        "type_annotation_generic_suffix_sites_total",
        "type_annotation_pointer_declarator_sites_total",
        "type_annotation_nullability_suffix_sites_total",
        "type_annotation_object_pointer_type_sites_total",
        "type_annotation_invalid_generic_suffix_sites_total",
        "type_annotation_invalid_pointer_declarator_sites_total",
        "type_annotation_invalid_nullability_suffix_sites_total",
        "deterministic_type_annotation_surface_handoff",
        "Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.type_annotation_surface_summary = result.integration_surface.type_annotation_surface_summary;",
        "result.deterministic_type_annotation_surface_handoff =",
        "result.parity_surface.type_annotation_surface_summary =",
        "result.parity_surface.type_annotation_generic_suffix_sites_total =",
        "result.parity_surface.type_annotation_pointer_declarator_sites_total =",
        "result.parity_surface.type_annotation_nullability_suffix_sites_total =",
        "result.parity_surface.type_annotation_object_pointer_type_sites_total =",
        "result.parity_surface.type_annotation_invalid_generic_suffix_sites_total =",
        "result.parity_surface.type_annotation_invalid_pointer_declarator_sites_total =",
        "result.parity_surface.type_annotation_invalid_nullability_suffix_sites_total =",
        "result.parity_surface.deterministic_type_annotation_surface_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildTypeAnnotationSurfaceSummaryFromIntegrationSurface(",
        "surface.type_annotation_surface_summary = BuildTypeAnnotationSurfaceSummaryFromIntegrationSurface(surface);",
        "handoff.type_annotation_surface_summary = Objc3TypeAnnotationSurfaceSummary{};",
        "type mismatch: generic parameter type suffix",
        "type mismatch: nullability parameter type suffix",
        "metadata.param_has_generic_suffix = source.param_has_generic_suffix;",
        "metadata.param_has_pointer_declarator = source.param_has_pointer_declarator;",
        "metadata.param_has_nullability_suffix = source.param_has_nullability_suffix;",
        "metadata.param_object_pointer_type_spelling = source.param_object_pointer_type_spelling;",
        "metadata.return_has_invalid_nullability_suffix = source.return_has_invalid_nullability_suffix;",
        "property_metadata.object_pointer_type_spelling = source.object_pointer_type_spelling;",
    ):
        assert marker in sema_passes
