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


def test_m161_lowering_ownership_qualifier_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Ownership-qualifier lowering artifact contract (M161-C001)",
        "kObjc3OwnershipQualifierLoweringLaneContract",
        "Objc3OwnershipQualifierLoweringContract",
        "IsValidObjc3OwnershipQualifierLoweringContract(...)",
        "Objc3OwnershipQualifierLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites",
        "frontend.pipeline.semantic_surface.objc_ownership_qualifier_lowering_surface",
        "lowering_ownership_qualifier.replay_key",
        "ownership_qualifier_lowering = ownership_qualifier_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m161_lowering_ownership_qualifier_contract.py -q",
    ):
        assert text in fragment


def test_m161_lowering_ownership_qualifier_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3OwnershipQualifierLoweringLaneContract",
        "struct Objc3OwnershipQualifierLoweringContract",
        "std::size_t ownership_qualifier_sites = 0;",
        "std::size_t invalid_ownership_qualifier_sites = 0;",
        "std::size_t object_pointer_type_annotation_sites = 0;",
        "IsValidObjc3OwnershipQualifierLoweringContract(",
        "Objc3OwnershipQualifierLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3OwnershipQualifierLoweringContract(",
        "Objc3OwnershipQualifierLoweringReplayKey(",
        '"ownership_qualifier_sites="',
        '";invalid_ownership_qualifier_sites="',
        '";object_pointer_type_annotation_sites="',
        '";lane_contract=" + kObjc3OwnershipQualifierLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildOwnershipQualifierLoweringContract(",
        "IsValidObjc3OwnershipQualifierLoweringContract(",
        "ownership_qualifier_lowering_replay_key",
        '\\"deterministic_ownership_qualifier_lowering_handoff\\":',
        '\\"objc_ownership_qualifier_lowering_surface\\":{\\"ownership_qualifier_sites\\":',
        '\\"lowering_ownership_qualifier\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_ownership_qualifier_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_ownership_qualifier_replay_key;",
        "std::size_t ownership_qualifier_lowering_ownership_qualifier_sites = 0;",
        "std::size_t ownership_qualifier_lowering_invalid_ownership_qualifier_sites = 0;",
        "std::size_t ownership_qualifier_lowering_object_pointer_type_annotation_sites = 0;",
        "bool deterministic_ownership_qualifier_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; ownership_qualifier_lowering = "',
        "frontend_objc_ownership_qualifier_lowering_profile",
        "!objc3.objc_ownership_qualifier_lowering = !{!14}",
        "!14 = !{i64 ",
    ):
        assert marker in ir_source
