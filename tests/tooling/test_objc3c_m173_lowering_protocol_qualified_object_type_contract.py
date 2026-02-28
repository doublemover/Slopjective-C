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


def test_m173_lowering_protocol_qualified_object_type_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Protocol-qualified object type lowering artifact contract (M173-C001)",
        "kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract",
        "Objc3ProtocolQualifiedObjectTypeLoweringContract",
        "IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(...)",
        "Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_protocol_qualified_object_type_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_protocol_qualified_object_type_lowering_surface",
        "lowering_protocol_qualified_object_type.replay_key",
        "protocol_qualified_object_type_lowering = protocol_qualified_object_type_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m173_lowering_protocol_qualified_object_type_contract.py -q",
    ):
        assert text in fragment


def test_m173_lowering_protocol_qualified_object_type_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract",
        "struct Objc3ProtocolQualifiedObjectTypeLoweringContract",
        "std::size_t protocol_qualified_object_type_sites = 0;",
        "std::size_t protocol_composition_sites = 0;",
        "std::size_t normalized_protocol_composition_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(",
        "Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(",
        "Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(",
        '\"protocol_qualified_object_type_sites=\"',
        '\";protocol_composition_sites=\"',
        '\";normalized_protocol_composition_sites=\"',
        '\";lane_contract=\" + kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildProtocolQualifiedObjectTypeLoweringContract(",
        "IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(",
        "protocol_qualified_object_type_lowering_replay_key",
        '\\"deterministic_protocol_qualified_object_type_lowering_handoff\\":',
        '\\"objc_protocol_qualified_object_type_lowering_surface\\":{\\"protocol_qualified_object_type_sites\\":',
        '\\"lowering_protocol_qualified_object_type_replay_key\\":\\"',
        "ir_frontend_metadata.lowering_protocol_qualified_object_type_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_protocol_qualified_object_type_replay_key;",
        "std::size_t protocol_qualified_object_type_lowering_sites = 0;",
        "std::size_t protocol_qualified_object_type_lowering_protocol_composition_sites = 0;",
        "std::size_t protocol_qualified_object_type_lowering_object_pointer_type_sites = 0;",
        "std::size_t protocol_qualified_object_type_lowering_terminated_protocol_composition_sites = 0;",
        "std::size_t protocol_qualified_object_type_lowering_pointer_declarator_sites = 0;",
        "std::size_t protocol_qualified_object_type_lowering_normalized_protocol_composition_sites = 0;",
        "std::size_t protocol_qualified_object_type_lowering_contract_violation_sites = 0;",
        "bool deterministic_protocol_qualified_object_type_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; protocol_qualified_object_type_lowering = "',
        "frontend_objc_protocol_qualified_object_type_lowering_profile",
        "!objc3.objc_protocol_qualified_object_type_lowering = !{!26}",
        "!26 = !{i64 ",
    ):
        assert marker in ir_source
