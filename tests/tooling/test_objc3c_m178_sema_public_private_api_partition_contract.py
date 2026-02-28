from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m178_sema_public_private_api_partition_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M178 sema/type public-private API partition contract (M178-B001)",
        "Objc3PublicPrivateApiPartitionSummary",
        "BuildPublicPrivateApiPartitionSummaryFromNamespaceCollisionShadowingSummary",
        "public_private_api_partition_sites_total",
        "deterministic_public_private_api_partition_handoff",
        "python -m pytest tests/tooling/test_objc3c_m178_sema_public_private_api_partition_contract.py -q",
    ):
        assert text in fragment


def test_m178_sema_public_private_api_partition_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3PublicPrivateApiPartitionSummary {",
        "std::size_t public_private_api_partition_sites = 0;",
        "std::size_t namespace_segment_sites = 0;",
        "std::size_t import_edge_candidate_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "public_private_api_partition_sites_total",
        "public_private_api_partition_import_edge_candidate_sites_total",
        "public_private_api_partition_contract_violation_sites_total",
        "deterministic_public_private_api_partition_handoff",
        "surface.public_private_api_partition_summary.public_private_api_partition_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentPublicPrivateApiPartitionSummary",
        "result.public_private_api_partition_summary =",
        "result.deterministic_public_private_api_partition_handoff =",
        "result.parity_surface.public_private_api_partition_summary =",
        "result.parity_surface.public_private_api_partition_sites_total =",
        "result.parity_surface.deterministic_public_private_api_partition_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildPublicPrivateApiPartitionSummaryFromNamespaceCollisionShadowingSummary(",
        "surface.public_private_api_partition_summary =",
        "handoff.public_private_api_partition_summary =",
        "handoff.public_private_api_partition_summary.deterministic",
    ):
        assert marker in sema_passes
