from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m179_lowering_incremental_module_cache_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Incremental module cache and invalidation lowering artifact contract (M179-C001)",
        "kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract",
        "Objc3IncrementalModuleCacheInvalidationLoweringContract",
        "IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(...)",
        "Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_incremental_module_cache_invalidation_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_incremental_module_cache_invalidation_lowering_surface",
        "lowering_incremental_module_cache_invalidation.replay_key",
        "incremental_module_cache_invalidation_lowering = incremental_module_cache_invalidation_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m179_lowering_incremental_module_cache_contract.py -q",
    ):
        assert text in fragment


def test_m179_lowering_incremental_module_cache_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract",
        "struct Objc3IncrementalModuleCacheInvalidationLoweringContract",
        "std::size_t incremental_module_cache_invalidation_sites = 0;",
        "std::size_t import_edge_candidate_sites = 0;",
        "std::size_t namespace_segment_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t cache_invalidation_candidate_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(",
        "Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(",
        "Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(",
        '"incremental_module_cache_invalidation_sites="',
        '";cache_invalidation_candidate_sites="',
        '";normalized_sites="',
        "kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "BuildIncrementalModuleCacheInvalidationLoweringContract(",
        "IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(",
        "incremental_module_cache_invalidation_lowering_replay_key",
        '\\"deterministic_incremental_module_cache_invalidation_lowering_handoff\\":',
        '\\"objc_incremental_module_cache_invalidation_lowering_surface\\":{\\"incremental_module_cache_invalidation_sites\\":',
        '\\"lowering_incremental_module_cache_invalidation_replay_key\\":\\"',
        "lowering_incremental_module_cache_invalidation_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_incremental_module_cache_invalidation_replay_key;",
        "std::size_t incremental_module_cache_invalidation_lowering_sites = 0;",
        "std::size_t incremental_module_cache_invalidation_lowering_import_edge_candidate_sites = 0;",
        "std::size_t incremental_module_cache_invalidation_lowering_namespace_segment_sites = 0;",
        "std::size_t incremental_module_cache_invalidation_lowering_object_pointer_type_sites = 0;",
        "std::size_t incremental_module_cache_invalidation_lowering_pointer_declarator_sites = 0;",
        "std::size_t incremental_module_cache_invalidation_lowering_normalized_sites = 0;",
        "std::size_t incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites = 0;",
        "std::size_t incremental_module_cache_invalidation_lowering_contract_violation_sites = 0;",
        "bool deterministic_incremental_module_cache_invalidation_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; incremental_module_cache_invalidation_lowering = "',
        "frontend_objc_incremental_module_cache_invalidation_lowering_profile",
        "!objc3.objc_incremental_module_cache_invalidation_lowering = !{!32}",
        "!32 = !{i64 ",
    ):
        assert marker in ir_source
