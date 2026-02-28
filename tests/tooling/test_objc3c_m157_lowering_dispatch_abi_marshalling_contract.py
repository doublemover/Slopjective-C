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


def test_m157_lowering_dispatch_abi_marshalling_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Dispatch ABI marshalling artifact contract (M157-C001)",
        "kObjc3DispatchAbiMarshallingLaneContract",
        "Objc3DispatchAbiMarshallingContract",
        "IsValidObjc3DispatchAbiMarshallingContract(...)",
        "Objc3DispatchAbiMarshallingReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.dispatch_abi_marshalling_message_send_sites",
        "frontend.pipeline.semantic_surface.objc_dispatch_abi_marshalling_surface",
        "lowering_dispatch_abi_marshalling.replay_key",
        "dispatch_abi_marshalling_lowering = message_send_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m157_lowering_dispatch_abi_marshalling_contract.py -q",
    ):
        assert text in fragment


def test_m157_lowering_dispatch_abi_marshalling_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3DispatchAbiMarshallingLaneContract",
        "struct Objc3DispatchAbiMarshallingContract",
        "std::size_t message_send_sites = 0;",
        "std::size_t receiver_slots_marshaled = 0;",
        "std::size_t selector_slots_marshaled = 0;",
        "std::size_t argument_value_slots_marshaled = 0;",
        "std::size_t argument_padding_slots_marshaled = 0;",
        "std::size_t argument_total_slots_marshaled = 0;",
        "std::size_t total_marshaled_slots = 0;",
        "std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;",
        "IsValidObjc3DispatchAbiMarshallingContract(",
        "Objc3DispatchAbiMarshallingReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3DispatchAbiMarshallingContract(",
        "Objc3DispatchAbiMarshallingReplayKey(",
        '"message_send_sites="',
        '";runtime_dispatch_arg_slots="',
        '";lane_contract=" + kObjc3DispatchAbiMarshallingLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildDispatchAbiMarshallingContract(",
        "IsValidObjc3DispatchAbiMarshallingContract(",
        "dispatch_abi_marshalling_replay_key",
        '\\"deterministic_dispatch_abi_marshalling_handoff\\":',
        '\\"objc_dispatch_abi_marshalling_surface\\":{\\"message_send_sites\\":',
        '\\"lowering_dispatch_abi_marshalling\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_dispatch_abi_marshalling_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_dispatch_abi_marshalling_replay_key;",
        "std::size_t dispatch_abi_marshalling_message_send_sites = 0;",
        "std::size_t dispatch_abi_marshalling_receiver_slots_marshaled = 0;",
        "std::size_t dispatch_abi_marshalling_selector_slots_marshaled = 0;",
        "std::size_t dispatch_abi_marshalling_argument_value_slots_marshaled = 0;",
        "std::size_t dispatch_abi_marshalling_argument_padding_slots_marshaled = 0;",
        "std::size_t dispatch_abi_marshalling_argument_total_slots_marshaled = 0;",
        "std::size_t dispatch_abi_marshalling_total_marshaled_slots = 0;",
        "std::size_t dispatch_abi_marshalling_runtime_dispatch_arg_slots = 0;",
        "bool deterministic_dispatch_abi_marshalling_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; dispatch_abi_marshalling_lowering = "',
        "frontend_objc_dispatch_abi_marshalling_profile",
        "!objc3.objc_dispatch_abi_marshalling = !{!10}",
    ):
        assert marker in ir_source
