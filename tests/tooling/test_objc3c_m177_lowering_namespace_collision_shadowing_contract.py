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


def test_m177_lowering_namespace_collision_shadowing_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Namespace collision and shadowing diagnostics lowering artifact contract (M177-C001)",
        "kObjc3NamespaceCollisionShadowingLoweringLaneContract",
        "Objc3NamespaceCollisionShadowingLoweringContract",
        "IsValidObjc3NamespaceCollisionShadowingLoweringContract(...)",
        "Objc3NamespaceCollisionShadowingLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_namespace_collision_shadowing_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_namespace_collision_shadowing_lowering_surface",
        "lowering_namespace_collision_shadowing.replay_key",
        "namespace_collision_shadowing_lowering = namespace_collision_shadowing_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m177_lowering_namespace_collision_shadowing_contract.py -q",
    ):
        assert text in fragment


def test_m177_lowering_namespace_collision_shadowing_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3NamespaceCollisionShadowingLoweringLaneContract",
        "struct Objc3NamespaceCollisionShadowingLoweringContract",
        "std::size_t namespace_collision_shadowing_sites = 0;",
        "std::size_t import_edge_candidate_sites = 0;",
        "std::size_t namespace_segment_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3NamespaceCollisionShadowingLoweringContract(",
        "Objc3NamespaceCollisionShadowingLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3NamespaceCollisionShadowingLoweringContract(",
        "Objc3NamespaceCollisionShadowingLoweringReplayKey(",
        '"namespace_collision_shadowing_sites="',
        '";import_edge_candidate_sites="',
        '";normalized_sites="',
        "kObjc3NamespaceCollisionShadowingLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "BuildNamespaceCollisionShadowingLoweringContract(",
        "IsValidObjc3NamespaceCollisionShadowingLoweringContract(",
        "namespace_collision_shadowing_lowering_replay_key",
        '\\"deterministic_namespace_collision_shadowing_lowering_handoff\\":',
        '\\"objc_namespace_collision_shadowing_lowering_surface\\":{\\"namespace_collision_shadowing_sites\\":',
        '\\"lowering_namespace_collision_shadowing_replay_key\\":\\"',
        "ir_frontend_metadata.lowering_namespace_collision_shadowing_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_namespace_collision_shadowing_replay_key;",
        "std::size_t namespace_collision_shadowing_lowering_sites = 0;",
        "std::size_t namespace_collision_shadowing_lowering_import_edge_candidate_sites = 0;",
        "std::size_t namespace_collision_shadowing_lowering_namespace_segment_sites = 0;",
        "std::size_t namespace_collision_shadowing_lowering_object_pointer_type_sites = 0;",
        "std::size_t namespace_collision_shadowing_lowering_pointer_declarator_sites = 0;",
        "std::size_t namespace_collision_shadowing_lowering_normalized_sites = 0;",
        "std::size_t namespace_collision_shadowing_lowering_contract_violation_sites = 0;",
        "bool deterministic_namespace_collision_shadowing_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; namespace_collision_shadowing_lowering = "',
        "frontend_objc_namespace_collision_shadowing_lowering_profile",
        "!objc3.objc_namespace_collision_shadowing_lowering = !{!30}",
        "!30 = !{i64 ",
    ):
        assert marker in ir_source
