from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m162_sema_retain_release_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M162 sema/type retain-release operation contract (M162-B001)",
        "BuildRetainReleaseOperationSummaryFromIntegrationSurface",
        "BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff",
        "retain_release_operation_ownership_qualified_sites_total",
        "retain_release_operation_contract_violation_sites_total",
        "deterministic_retain_release_operation_handoff",
        "python -m pytest tests/tooling/test_objc3c_m162_sema_retain_release_contract.py -q",
    ):
        assert text in fragment


def test_m162_sema_retain_release_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3RetainReleaseOperationSummary {",
        "std::size_t ownership_qualified_sites = 0;",
        "std::size_t retain_insertion_sites = 0;",
        "std::size_t release_insertion_sites = 0;",
        "std::size_t autorelease_insertion_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::vector<bool> param_ownership_insert_retain;",
        "std::vector<bool> param_ownership_insert_release;",
        "std::vector<bool> param_ownership_insert_autorelease;",
        "bool return_ownership_insert_retain = false;",
        "bool return_ownership_insert_release = false;",
        "bool return_ownership_insert_autorelease = false;",
        "bool ownership_insert_retain = false;",
        "bool ownership_insert_release = false;",
        "bool ownership_insert_autorelease = false;",
        "Objc3RetainReleaseOperationSummary retain_release_operation_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "retain_release_operation_ownership_qualified_sites_total",
        "retain_release_operation_retain_insertion_sites_total",
        "retain_release_operation_release_insertion_sites_total",
        "retain_release_operation_autorelease_insertion_sites_total",
        "retain_release_operation_contract_violation_sites_total",
        "deterministic_retain_release_operation_handoff",
        "surface.retain_release_operation_summary.ownership_qualified_sites ==",
        "surface.retain_release_operation_summary.contract_violation_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentRetainReleaseOperationSummary",
        "result.retain_release_operation_summary = result.integration_surface.retain_release_operation_summary;",
        "result.deterministic_retain_release_operation_handoff =",
        "result.parity_surface.retain_release_operation_summary =",
        "result.parity_surface.retain_release_operation_ownership_qualified_sites_total =",
        "result.parity_surface.deterministic_retain_release_operation_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildRetainReleaseOperationSummaryFromIntegrationSurface",
        "BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff",
        "surface.retain_release_operation_summary =",
        "handoff.retain_release_operation_summary =",
        "metadata.param_ownership_insert_retain = source.param_ownership_insert_retain;",
        "metadata.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;",
    ):
        assert marker in sema_passes
