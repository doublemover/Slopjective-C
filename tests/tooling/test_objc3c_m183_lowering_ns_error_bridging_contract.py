from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m183_lowering_ns_error_bridging_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## NSError bridging lowering artifact contract (M183-C001)",
        "kObjc3NSErrorBridgingLoweringLaneContract",
        "Objc3NSErrorBridgingLoweringContract",
        "IsValidObjc3NSErrorBridgingLoweringContract(...)",
        "Objc3NSErrorBridgingLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.deterministic_ns_error_bridging_lowering_handoff",
        "frontend.pipeline.semantic_surface.objc_ns_error_bridging_lowering_surface",
        "lowering_ns_error_bridging.replay_key",
        "ns_error_bridging_lowering = ns_error_bridging_sites=<N>",
        "frontend_objc_ns_error_bridging_lowering_profile",
        "!objc3.objc_ns_error_bridging_lowering = !{!36}",
        "python -m pytest tests/tooling/test_objc3c_m183_lowering_ns_error_bridging_contract.py -q",
    ):
        assert text in fragment


def test_m183_lowering_ns_error_bridging_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3NSErrorBridgingLoweringLaneContract",
        "struct Objc3NSErrorBridgingLoweringContract",
        "std::size_t ns_error_bridging_sites = 0;",
        "std::size_t ns_error_parameter_sites = 0;",
        "std::size_t ns_error_out_parameter_sites = 0;",
        "std::size_t ns_error_bridge_path_sites = 0;",
        "std::size_t failable_call_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t bridge_boundary_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3NSErrorBridgingLoweringContract(",
        "std::string Objc3NSErrorBridgingLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3NSErrorBridgingLoweringContract(",
        "Objc3NSErrorBridgingLoweringReplayKey(",
        "contract.ns_error_out_parameter_sites > contract.ns_error_parameter_sites",
        "contract.ns_error_bridge_path_sites > contract.ns_error_out_parameter_sites",
        "contract.ns_error_bridge_path_sites > contract.failable_call_sites",
        "contract.normalized_sites + contract.bridge_boundary_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"ns_error_bridging_sites="',
        '";ns_error_parameter_sites="',
        '";ns_error_out_parameter_sites="',
        '";ns_error_bridge_path_sites="',
        '";failable_call_sites="',
        '";normalized_sites="',
        '";bridge_boundary_sites="',
        '";contract_violation_sites="',
        "kObjc3NSErrorBridgingLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_ns_error_bridging_replay_key;",
        "std::size_t ns_error_bridging_lowering_sites = 0;",
        "std::size_t ns_error_bridging_lowering_ns_error_parameter_sites = 0;",
        "std::size_t ns_error_bridging_lowering_ns_error_out_parameter_sites = 0;",
        "std::size_t ns_error_bridging_lowering_ns_error_bridge_path_sites = 0;",
        "std::size_t ns_error_bridging_lowering_failable_call_sites = 0;",
        "std::size_t ns_error_bridging_lowering_normalized_sites = 0;",
        "std::size_t ns_error_bridging_lowering_bridge_boundary_sites = 0;",
        "std::size_t ns_error_bridging_lowering_contract_violation_sites = 0;",
        "bool deterministic_ns_error_bridging_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; ns_error_bridging_lowering = "',
        "frontend_objc_ns_error_bridging_lowering_profile",
        "!objc3.objc_ns_error_bridging_lowering = !{!36}",
        "!36 = !{i64 ",
    ):
        assert marker in ir_source
