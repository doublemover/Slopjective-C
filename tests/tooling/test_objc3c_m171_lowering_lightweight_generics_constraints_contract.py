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


def test_m171_lowering_lightweight_generics_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Lightweight generics constraint lowering artifact contract (M171-C001)",
        "kObjc3LightweightGenericsConstraintLoweringLaneContract",
        "Objc3LightweightGenericsConstraintLoweringContract",
        "IsValidObjc3LightweightGenericsConstraintLoweringContract(...)",
        "Objc3LightweightGenericsConstraintLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_lightweight_generic_constraint_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_lightweight_generic_constraint_lowering_surface",
        "lowering_lightweight_generic_constraint.replay_key",
        "lightweight_generic_constraint_lowering = generic_constraint_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m171_lowering_lightweight_generics_constraints_contract.py -q",
    ):
        assert text in fragment


def test_m171_lowering_lightweight_generics_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3LightweightGenericsConstraintLoweringLaneContract",
        "struct Objc3LightweightGenericsConstraintLoweringContract",
        "std::size_t generic_suffix_sites = 0;",
        "std::size_t object_pointer_type_sites = 0;",
        "std::size_t terminated_generic_suffix_sites = 0;",
        "std::size_t pointer_declarator_sites = 0;",
        "std::size_t normalized_constraint_sites = 0;",
        "IsValidObjc3LightweightGenericsConstraintLoweringContract(",
        "Objc3LightweightGenericsConstraintLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3LightweightGenericsConstraintLoweringContract(",
        "Objc3LightweightGenericsConstraintLoweringReplayKey(",
        '\";generic_suffix_sites=\"',
        '\";object_pointer_type_sites=\"',
        '\";terminated_generic_suffix_sites=\"',
        '\";pointer_declarator_sites=\"',
        '\";normalized_constraint_sites=\"',
        '\";lane_contract=\" + kObjc3LightweightGenericsConstraintLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildLightweightGenericsConstraintLoweringContract(",
        "IsValidObjc3LightweightGenericsConstraintLoweringContract(",
        "lightweight_generic_constraint_lowering_replay_key",
        '\\"deterministic_lightweight_generic_constraint_lowering_handoff\\":',
        '\\"objc_lightweight_generic_constraint_lowering_surface\\":{\\"generic_constraint_sites\\":',
        '\\"lowering_lightweight_generic_constraint_replay_key\\":\\"',
        "ir_frontend_metadata.lowering_lightweight_generic_constraint_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_lightweight_generic_constraint_replay_key;",
        "std::size_t lightweight_generic_constraint_lowering_generic_constraint_sites = 0;",
        "std::size_t lightweight_generic_constraint_lowering_generic_suffix_sites = 0;",
        "std::size_t lightweight_generic_constraint_lowering_object_pointer_type_sites = 0;",
        "std::size_t lightweight_generic_constraint_lowering_terminated_generic_suffix_sites = 0;",
        "std::size_t lightweight_generic_constraint_lowering_pointer_declarator_sites = 0;",
        "std::size_t lightweight_generic_constraint_lowering_normalized_constraint_sites = 0;",
        "std::size_t lightweight_generic_constraint_lowering_contract_violation_sites = 0;",
        "bool deterministic_lightweight_generic_constraint_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; lightweight_generic_constraint_lowering = "',
        "frontend_objc_lightweight_generic_constraint_lowering_profile",
        "!objc3.objc_lightweight_generic_constraint_lowering = !{!24}",
        "!24 = !{i64 ",
    ):
        assert marker in ir_source
