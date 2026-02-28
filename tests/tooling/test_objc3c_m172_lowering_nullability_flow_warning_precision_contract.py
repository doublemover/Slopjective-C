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


def test_m172_lowering_nullability_flow_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Nullability-flow warning-precision lowering artifact contract (M172-C001)",
        "kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract",
        "Objc3NullabilityFlowWarningPrecisionLoweringContract",
        "IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(...)",
        "Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_nullability_flow_warning_precision_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_nullability_flow_warning_precision_lowering_surface",
        "lowering_nullability_flow_warning_precision.replay_key",
        "nullability_flow_warning_precision_lowering = nullability_flow_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m172_lowering_nullability_flow_warning_precision_contract.py -q",
    ):
        assert text in fragment


def test_m172_lowering_nullability_flow_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract",
        "struct Objc3NullabilityFlowWarningPrecisionLoweringContract",
        "std::size_t nullability_flow_sites = 0;",
        "std::size_t nullable_suffix_sites = 0;",
        "std::size_t nonnull_suffix_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(",
        "Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(",
        "Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(",
        '\"nullability_flow_sites=\"',
        '\";nullable_suffix_sites=\"',
        '\";nonnull_suffix_sites=\"',
        '\";normalized_sites=\"',
        '\";lane_contract=\" + kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildNullabilityFlowWarningPrecisionLoweringContract(",
        "IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(",
        "nullability_flow_warning_precision_lowering_replay_key",
        '\\"deterministic_nullability_flow_warning_precision_lowering_handoff\\":',
        '\\"objc_nullability_flow_warning_precision_lowering_surface\\":{\\"nullability_flow_sites\\":',
        '\\"lowering_nullability_flow_warning_precision_replay_key\\":\\"',
        "ir_frontend_metadata.lowering_nullability_flow_warning_precision_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_nullability_flow_warning_precision_replay_key;",
        "std::size_t nullability_flow_warning_precision_lowering_sites = 0;",
        "std::size_t nullability_flow_warning_precision_lowering_object_pointer_type_sites = 0;",
        "std::size_t nullability_flow_warning_precision_lowering_nullability_suffix_sites = 0;",
        "std::size_t nullability_flow_warning_precision_lowering_nullable_suffix_sites = 0;",
        "std::size_t nullability_flow_warning_precision_lowering_nonnull_suffix_sites = 0;",
        "std::size_t nullability_flow_warning_precision_lowering_normalized_sites = 0;",
        "std::size_t nullability_flow_warning_precision_lowering_contract_violation_sites = 0;",
        "bool deterministic_nullability_flow_warning_precision_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; nullability_flow_warning_precision_lowering = "',
        "frontend_objc_nullability_flow_warning_precision_lowering_profile",
        "!objc3.objc_nullability_flow_warning_precision_lowering = !{!25}",
        "!25 = !{i64 ",
    ):
        assert marker in ir_source
