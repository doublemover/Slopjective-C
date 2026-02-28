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


def test_m175_lowering_generic_metadata_abi_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Generic metadata emission and ABI checks lowering artifact contract (M175-C001)",
        "kObjc3GenericMetadataAbiLoweringLaneContract",
        "Objc3GenericMetadataAbiLoweringContract",
        "IsValidObjc3GenericMetadataAbiLoweringContract(...)",
        "Objc3GenericMetadataAbiLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_generic_metadata_abi_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_generic_metadata_abi_lowering_surface",
        "lowering_generic_metadata_abi.replay_key",
        "generic_metadata_abi_lowering = generic_metadata_abi_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m175_lowering_generic_metadata_abi_contract.py -q",
    ):
        assert text in fragment


def test_m175_lowering_generic_metadata_abi_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3GenericMetadataAbiLoweringLaneContract",
        "struct Objc3GenericMetadataAbiLoweringContract",
        "std::size_t generic_metadata_abi_sites = 0;",
        "std::size_t generic_suffix_sites = 0;",
        "std::size_t protocol_composition_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3GenericMetadataAbiLoweringContract(",
        "Objc3GenericMetadataAbiLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3GenericMetadataAbiLoweringContract(",
        "Objc3GenericMetadataAbiLoweringReplayKey(",
        '"generic_metadata_abi_sites="',
        '";generic_suffix_sites="',
        '";normalized_sites="',
        '";lane_contract=" + kObjc3GenericMetadataAbiLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildGenericMetadataAbiLoweringContract(",
        "IsValidObjc3GenericMetadataAbiLoweringContract(",
        "generic_metadata_abi_lowering_replay_key",
        '\\"deterministic_generic_metadata_abi_lowering_handoff\\":',
        '\\"objc_generic_metadata_abi_lowering_surface\\":{\\"generic_metadata_abi_sites\\":',
        '\\"lowering_generic_metadata_abi_replay_key\\":\\"',
        "ir_frontend_metadata.lowering_generic_metadata_abi_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_generic_metadata_abi_replay_key;",
        "std::size_t generic_metadata_abi_lowering_sites = 0;",
        "std::size_t generic_metadata_abi_lowering_generic_suffix_sites = 0;",
        "std::size_t generic_metadata_abi_lowering_protocol_composition_sites = 0;",
        "std::size_t generic_metadata_abi_lowering_ownership_qualifier_sites = 0;",
        "std::size_t generic_metadata_abi_lowering_object_pointer_type_sites = 0;",
        "std::size_t generic_metadata_abi_lowering_pointer_declarator_sites = 0;",
        "std::size_t generic_metadata_abi_lowering_normalized_sites = 0;",
        "std::size_t generic_metadata_abi_lowering_contract_violation_sites = 0;",
        "bool deterministic_generic_metadata_abi_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; generic_metadata_abi_lowering = "',
        "frontend_objc_generic_metadata_abi_lowering_profile",
        "!objc3.objc_generic_metadata_abi_lowering = !{!28}",
        "!28 = !{i64 ",
    ):
        assert marker in ir_source
