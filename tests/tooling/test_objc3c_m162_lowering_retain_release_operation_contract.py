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


def test_m162_lowering_retain_release_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Retain-release operation lowering artifact contract (M162-C001)",
        "kObjc3RetainReleaseOperationLoweringLaneContract",
        "Objc3RetainReleaseOperationLoweringContract",
        "IsValidObjc3RetainReleaseOperationLoweringContract(...)",
        "Objc3RetainReleaseOperationLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.retain_release_operation_lowering_ownership_qualified_sites",
        "frontend.pipeline.semantic_surface.objc_retain_release_operation_lowering_surface",
        "lowering_retain_release_operation.replay_key",
        "retain_release_operation_lowering = ownership_qualified_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m162_lowering_retain_release_operation_contract.py -q",
    ):
        assert text in fragment


def test_m162_lowering_retain_release_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3RetainReleaseOperationLoweringLaneContract",
        "struct Objc3RetainReleaseOperationLoweringContract",
        "std::size_t ownership_qualified_sites = 0;",
        "std::size_t retain_insertion_sites = 0;",
        "std::size_t release_insertion_sites = 0;",
        "std::size_t autorelease_insertion_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3RetainReleaseOperationLoweringContract(",
        "Objc3RetainReleaseOperationLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3RetainReleaseOperationLoweringContract(",
        "Objc3RetainReleaseOperationLoweringReplayKey(",
        '"ownership_qualified_sites="',
        '";retain_insertion_sites="',
        '";release_insertion_sites="',
        '";autorelease_insertion_sites="',
        '";contract_violation_sites="',
        '";lane_contract=" + kObjc3RetainReleaseOperationLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildRetainReleaseOperationLoweringContract(",
        "IsValidObjc3RetainReleaseOperationLoweringContract(",
        "retain_release_operation_lowering_replay_key",
        '\\"deterministic_retain_release_operation_lowering_handoff\\":',
        '\\"objc_retain_release_operation_lowering_surface\\":{\\"ownership_qualified_sites\\":',
        '\\"lowering_retain_release_operation\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_retain_release_operation_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_retain_release_operation_replay_key;",
        "std::size_t retain_release_operation_lowering_ownership_qualified_sites = 0;",
        "std::size_t retain_release_operation_lowering_retain_insertion_sites = 0;",
        "std::size_t retain_release_operation_lowering_release_insertion_sites = 0;",
        "std::size_t retain_release_operation_lowering_autorelease_insertion_sites = 0;",
        "std::size_t retain_release_operation_lowering_contract_violation_sites = 0;",
        "bool deterministic_retain_release_operation_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; retain_release_operation_lowering = "',
        "frontend_objc_retain_release_operation_lowering_profile",
        "!objc3.objc_retain_release_operation_lowering = !{!15}",
        "!15 = !{i64 ",
    ):
        assert marker in ir_source
