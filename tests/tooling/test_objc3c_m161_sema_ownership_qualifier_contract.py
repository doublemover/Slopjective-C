from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m161_sema_ownership_qualifier_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M161 sema/type ownership qualifier contract (M161-B001)",
        "SupportsOwnershipQualifierParamTypeSuffix",
        "HasInvalidOwnershipQualifierParamTypeSuffix",
        "ownership_qualifier_sites",
        "invalid_ownership_qualifier_sites",
        "type_annotation_ownership_qualifier_sites_total",
        "type_annotation_invalid_ownership_qualifier_sites_total",
        "python -m pytest tests/tooling/test_objc3c_m161_sema_ownership_qualifier_contract.py -q",
    ):
        assert text in fragment


def test_m161_sema_ownership_qualifier_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "std::size_t ownership_qualifier_sites = 0;",
        "std::size_t invalid_ownership_qualifier_sites = 0;",
        "std::vector<bool> param_has_ownership_qualifier;",
        "std::vector<bool> param_has_invalid_ownership_qualifier;",
        "bool return_has_ownership_qualifier = false;",
        "bool return_has_invalid_ownership_qualifier = false;",
        "bool has_ownership_qualifier = false;",
        "bool has_invalid_ownership_qualifier = false;",
    ):
        assert marker in sema_contract

    for marker in (
        "type_annotation_ownership_qualifier_sites_total",
        "type_annotation_invalid_ownership_qualifier_sites_total",
        "surface.type_annotation_surface_summary.ownership_qualifier_sites ==",
        "surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "lhs.ownership_qualifier_sites == rhs.ownership_qualifier_sites",
        "lhs.invalid_ownership_qualifier_sites == rhs.invalid_ownership_qualifier_sites",
        "result.parity_surface.type_annotation_ownership_qualifier_sites_total =",
        "result.parity_surface.type_annotation_invalid_ownership_qualifier_sites_total =",
        "result.parity_surface.type_annotation_surface_summary.ownership_qualifier_sites ==",
        "result.parity_surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "SupportsOwnershipQualifierParamTypeSuffix",
        "SupportsOwnershipQualifierReturnTypeSuffix",
        "SupportsOwnershipQualifierPropertyTypeSuffix",
        "HasInvalidOwnershipQualifierParamTypeSuffix",
        "HasInvalidOwnershipQualifierReturnTypeSuffix",
        "HasInvalidOwnershipQualifierPropertyTypeSuffix",
        "param.ownership_qualifier_tokens",
        "fn.return_ownership_qualifier_tokens",
        "method.return_ownership_qualifier_tokens",
        "property.ownership_qualifier_tokens",
        "summary.ownership_qualifier_sites",
        "summary.invalid_ownership_qualifier_sites",
    ):
        assert marker in sema_passes
