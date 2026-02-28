from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m175_sema_generic_metadata_abi_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M175 sema/type generic metadata emission and ABI checks contract (M175-B001)",
        "Objc3GenericMetadataAbiSummary",
        "BuildGenericMetadataAbiSummaryFromTypeAnnotationAndConstraintSummaries",
        "generic_metadata_abi_sites_total",
        "deterministic_generic_metadata_abi_handoff",
        "python -m pytest tests/tooling/test_objc3c_m175_sema_generic_metadata_abi_contract.py -q",
    ):
        assert text in fragment


def test_m175_sema_generic_metadata_abi_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3GenericMetadataAbiSummary {",
        "std::size_t generic_metadata_abi_sites = 0;",
        "std::size_t generic_suffix_sites = 0;",
        "std::size_t protocol_composition_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "generic_metadata_abi_sites_total",
        "generic_metadata_abi_generic_suffix_sites_total",
        "generic_metadata_abi_contract_violation_sites_total",
        "deterministic_generic_metadata_abi_handoff",
        "surface.generic_metadata_abi_summary.generic_metadata_abi_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentGenericMetadataAbiSummary",
        "result.generic_metadata_abi_summary =",
        "result.deterministic_generic_metadata_abi_handoff =",
        "result.parity_surface.generic_metadata_abi_summary =",
        "result.parity_surface.generic_metadata_abi_sites_total =",
        "result.parity_surface.deterministic_generic_metadata_abi_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildGenericMetadataAbiSummaryFromTypeAnnotationAndConstraintSummaries(",
        "surface.generic_metadata_abi_summary =",
        "handoff.generic_metadata_abi_summary =",
        "handoff.generic_metadata_abi_summary.deterministic",
    ):
        assert marker in sema_passes
