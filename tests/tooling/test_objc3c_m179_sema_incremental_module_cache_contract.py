from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m179_sema_incremental_module_cache_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M179 sema/type incremental module cache and invalidation contract (M179-B001)",
        "Objc3IncrementalModuleCacheInvalidationSummary",
        "BuildIncrementalModuleCacheInvalidationSummaryFromPublicPrivateApiPartitionSummary",
        "incremental_module_cache_invalidation_sites_total",
        "deterministic_incremental_module_cache_invalidation_handoff",
        "python -m pytest tests/tooling/test_objc3c_m179_sema_incremental_module_cache_contract.py -q",
    ):
        assert text in fragment


def test_m179_sema_incremental_module_cache_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3IncrementalModuleCacheInvalidationSummary {",
        "std::size_t incremental_module_cache_invalidation_sites = 0;",
        "std::size_t cache_invalidation_candidate_sites = 0;",
        "Objc3IncrementalModuleCacheInvalidationSummary incremental_module_cache_invalidation_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "incremental_module_cache_invalidation_sites_total",
        "incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total",
        "deterministic_incremental_module_cache_invalidation_handoff",
        "surface.incremental_module_cache_invalidation_summary.incremental_module_cache_invalidation_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentIncrementalModuleCacheInvalidationSummary",
        "result.incremental_module_cache_invalidation_summary =",
        "result.deterministic_incremental_module_cache_invalidation_handoff =",
        "result.parity_surface.incremental_module_cache_invalidation_summary =",
        "result.parity_surface.incremental_module_cache_invalidation_sites_total =",
        "result.parity_surface.deterministic_incremental_module_cache_invalidation_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildIncrementalModuleCacheInvalidationSummaryFromPublicPrivateApiPartitionSummary(",
        "surface.incremental_module_cache_invalidation_summary =",
        "handoff.incremental_module_cache_invalidation_summary =",
        "handoff.incremental_module_cache_invalidation_summary.deterministic",
    ):
        assert marker in sema_passes
