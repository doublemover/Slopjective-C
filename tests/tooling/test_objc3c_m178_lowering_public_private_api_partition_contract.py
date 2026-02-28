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


def test_m178_lowering_public_private_api_partition_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Public-private API partition semantics lowering artifact contract (M178-C001)",
        "kObjc3PublicPrivateApiPartitionLoweringLaneContract",
        "Objc3PublicPrivateApiPartitionLoweringContract",
        "IsValidObjc3PublicPrivateApiPartitionLoweringContract(...)",
        "Objc3PublicPrivateApiPartitionLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_public_private_api_partition_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_public_private_api_partition_lowering_surface",
        "lowering_public_private_api_partition.replay_key",
        "public_private_api_partition_lowering = public_private_api_partition_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m178_lowering_public_private_api_partition_contract.py -q",
    ):
        assert text in fragment


def test_m178_lowering_public_private_api_partition_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3PublicPrivateApiPartitionLoweringLaneContract",
        "struct Objc3PublicPrivateApiPartitionLoweringContract",
        "std::size_t public_private_api_partition_sites = 0;",
        "std::size_t import_edge_candidate_sites = 0;",
        "std::size_t namespace_segment_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3PublicPrivateApiPartitionLoweringContract(",
        "Objc3PublicPrivateApiPartitionLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3PublicPrivateApiPartitionLoweringContract(",
        "Objc3PublicPrivateApiPartitionLoweringReplayKey(",
        '"public_private_api_partition_sites="',
        '";import_edge_candidate_sites="',
        '";normalized_sites="',
        "kObjc3PublicPrivateApiPartitionLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "BuildPublicPrivateApiPartitionLoweringContract(",
        "IsValidObjc3PublicPrivateApiPartitionLoweringContract(",
        "public_private_api_partition_lowering_replay_key",
        '\\"deterministic_public_private_api_partition_lowering_handoff\\":',
        '\\"objc_public_private_api_partition_lowering_surface\\":{\\"public_private_api_partition_sites\\":',
        '\\"lowering_public_private_api_partition_replay_key\\":\\"',
        "ir_frontend_metadata.lowering_public_private_api_partition_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_public_private_api_partition_replay_key;",
        "std::size_t public_private_api_partition_lowering_sites = 0;",
        "std::size_t public_private_api_partition_lowering_import_edge_candidate_sites = 0;",
        "std::size_t public_private_api_partition_lowering_namespace_segment_sites = 0;",
        "std::size_t public_private_api_partition_lowering_object_pointer_type_sites = 0;",
        "std::size_t public_private_api_partition_lowering_pointer_declarator_sites = 0;",
        "std::size_t public_private_api_partition_lowering_normalized_sites = 0;",
        "std::size_t public_private_api_partition_lowering_contract_violation_sites = 0;",
        "bool deterministic_public_private_api_partition_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; public_private_api_partition_lowering = "',
        "frontend_objc_public_private_api_partition_lowering_profile",
        "!objc3.objc_public_private_api_partition_lowering = !{!31}",
        "!31 = !{i64 ",
    ):
        assert marker in ir_source
