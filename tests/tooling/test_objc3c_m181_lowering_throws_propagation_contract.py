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


def test_m181_lowering_throws_propagation_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Throws propagation lowering artifact contract (M181-C001)",
        "kObjc3ThrowsPropagationLoweringLaneContract",
        "Objc3ThrowsPropagationLoweringContract",
        "IsValidObjc3ThrowsPropagationLoweringContract(...)",
        "Objc3ThrowsPropagationLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_throws_propagation_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_throws_propagation_lowering_surface",
        "lowering_throws_propagation.replay_key",
        "throws_propagation_lowering = throws_propagation_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m181_lowering_throws_propagation_contract.py -q",
    ):
        assert text in fragment


def test_m181_lowering_throws_propagation_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3ThrowsPropagationLoweringLaneContract",
        "struct Objc3ThrowsPropagationLoweringContract",
        "std::size_t throws_propagation_sites = 0;",
        "std::size_t import_edge_candidate_sites = 0;",
        "std::size_t namespace_segment_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t cache_invalidation_candidate_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3ThrowsPropagationLoweringContract(",
        "Objc3ThrowsPropagationLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3ThrowsPropagationLoweringContract(",
        "Objc3ThrowsPropagationLoweringReplayKey(",
        '"throws_propagation_sites="',
        '";cache_invalidation_candidate_sites="',
        '";normalized_sites="',
        "kObjc3ThrowsPropagationLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "BuildThrowsPropagationLoweringContract(",
        "IsValidObjc3ThrowsPropagationLoweringContract(",
        "throws_propagation_lowering_replay_key",
        '\\"deterministic_throws_propagation_lowering_handoff\\":',
        '\\"objc_throws_propagation_lowering_surface\\":{\\"throws_propagation_sites\\":',
        '\\"lowering_throws_propagation_replay_key\\":\\"',
        '\\"lowering_throws_propagation\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_throws_propagation_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_throws_propagation_replay_key;",
        "std::size_t throws_propagation_lowering_sites = 0;",
        "std::size_t throws_propagation_lowering_import_edge_candidate_sites = 0;",
        "std::size_t throws_propagation_lowering_namespace_segment_sites = 0;",
        "std::size_t throws_propagation_lowering_object_pointer_type_sites = 0;",
        "std::size_t throws_propagation_lowering_pointer_declarator_sites = 0;",
        "std::size_t throws_propagation_lowering_normalized_sites = 0;",
        "std::size_t throws_propagation_lowering_cache_invalidation_candidate_sites = 0;",
        "std::size_t throws_propagation_lowering_contract_violation_sites = 0;",
        "bool deterministic_throws_propagation_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; throws_propagation_lowering = "',
        "frontend_objc_throws_propagation_lowering_profile",
        "!objc3.objc_throws_propagation_lowering = !{!34}",
        "!34 = !{i64 ",
    ):
        assert marker in ir_source
