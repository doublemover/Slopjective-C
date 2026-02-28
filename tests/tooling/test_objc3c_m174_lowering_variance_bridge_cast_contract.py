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


def test_m174_lowering_variance_bridge_cast_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Variance/bridged-cast lowering artifact contract (M174-C001)",
        "kObjc3VarianceBridgeCastLoweringLaneContract",
        "Objc3VarianceBridgeCastLoweringContract",
        "IsValidObjc3VarianceBridgeCastLoweringContract(...)",
        "Objc3VarianceBridgeCastLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_variance_bridge_cast_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_variance_bridge_cast_lowering_surface",
        "lowering_variance_bridge_cast.replay_key",
        "variance_bridge_cast_lowering = variance_bridge_cast_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m174_lowering_variance_bridge_cast_contract.py -q",
    ):
        assert text in fragment


def test_m174_lowering_variance_bridge_cast_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3VarianceBridgeCastLoweringLaneContract",
        "struct Objc3VarianceBridgeCastLoweringContract",
        "std::size_t variance_bridge_cast_sites = 0;",
        "std::size_t protocol_composition_sites = 0;",
        "std::size_t ownership_qualifier_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3VarianceBridgeCastLoweringContract(",
        "Objc3VarianceBridgeCastLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3VarianceBridgeCastLoweringContract(",
        "Objc3VarianceBridgeCastLoweringReplayKey(",
        '\"variance_bridge_cast_sites=\"',
        '\";protocol_composition_sites=\"',
        '\";normalized_sites=\"',
        '\";lane_contract=\" + kObjc3VarianceBridgeCastLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildVarianceBridgeCastLoweringContract(",
        "IsValidObjc3VarianceBridgeCastLoweringContract(",
        "variance_bridge_cast_lowering_replay_key",
        '\\"deterministic_variance_bridge_cast_lowering_handoff\\":',
        '\\"objc_variance_bridge_cast_lowering_surface\\":{\\"variance_bridge_cast_sites\\":',
        '\\"lowering_variance_bridge_cast_replay_key\\":\\"',
        "ir_frontend_metadata.lowering_variance_bridge_cast_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_variance_bridge_cast_replay_key;",
        "std::size_t variance_bridge_cast_lowering_sites = 0;",
        "std::size_t variance_bridge_cast_lowering_protocol_composition_sites = 0;",
        "std::size_t variance_bridge_cast_lowering_ownership_qualifier_sites = 0;",
        "std::size_t variance_bridge_cast_lowering_object_pointer_type_sites = 0;",
        "std::size_t variance_bridge_cast_lowering_pointer_declarator_sites = 0;",
        "std::size_t variance_bridge_cast_lowering_normalized_sites = 0;",
        "std::size_t variance_bridge_cast_lowering_contract_violation_sites = 0;",
        "bool deterministic_variance_bridge_cast_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; variance_bridge_cast_lowering = "',
        "frontend_objc_variance_bridge_cast_lowering_profile",
        "!objc3.objc_variance_bridge_cast_lowering = !{!27}",
        "!27 = !{i64 ",
    ):
        assert marker in ir_source
