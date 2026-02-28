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


def test_m160_lowering_runtime_shim_host_link_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Runtime-shim host-link artifact contract (M160-C001)",
        "kObjc3RuntimeShimHostLinkLaneContract",
        "Objc3RuntimeShimHostLinkContract",
        "IsValidObjc3RuntimeShimHostLinkContract(...)",
        "Objc3RuntimeShimHostLinkReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.runtime_shim_host_link_message_send_sites",
        "frontend.pipeline.semantic_surface.objc_runtime_shim_host_link_surface",
        "lowering_runtime_shim_host_link.replay_key",
        "runtime_shim_host_link_lowering = message_send_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m160_lowering_runtime_shim_host_link_contract.py -q",
    ):
        assert text in fragment


def test_m160_lowering_runtime_shim_host_link_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3RuntimeShimHostLinkLaneContract",
        "struct Objc3RuntimeShimHostLinkContract",
        "std::size_t runtime_shim_required_sites = 0;",
        "std::size_t runtime_shim_elided_sites = 0;",
        "std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;",
        "std::size_t runtime_dispatch_declaration_parameter_count = 0;",
        "std::size_t contract_violation_sites = 0;",
        "std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;",
        "bool default_runtime_dispatch_symbol_binding = true;",
        "IsValidObjc3RuntimeShimHostLinkContract(",
        "Objc3RuntimeShimHostLinkReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3RuntimeShimHostLinkContract(",
        "Objc3RuntimeShimHostLinkReplayKey(",
        '"message_send_sites="',
        '";runtime_dispatch_symbol="',
        '";lane_contract=" + kObjc3RuntimeShimHostLinkLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildRuntimeShimHostLinkContract(",
        "IsValidObjc3RuntimeShimHostLinkContract(",
        "runtime_shim_host_link_replay_key",
        '\\"deterministic_runtime_shim_host_link_handoff\\":',
        '\\"objc_runtime_shim_host_link_surface\\":{\\"message_send_sites\\":',
        '\\"lowering_runtime_shim_host_link\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_runtime_shim_host_link_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_runtime_shim_host_link_replay_key;",
        "std::size_t runtime_shim_host_link_message_send_sites = 0;",
        "std::size_t runtime_shim_host_link_required_sites = 0;",
        "std::size_t runtime_shim_host_link_elided_sites = 0;",
        "std::size_t runtime_shim_host_link_runtime_dispatch_arg_slots = 0;",
        "std::size_t runtime_shim_host_link_runtime_dispatch_declaration_parameter_count = 0;",
        "std::size_t runtime_shim_host_link_contract_violation_sites = 0;",
        "std::string runtime_shim_host_link_runtime_dispatch_symbol;",
        "bool runtime_shim_host_link_default_runtime_dispatch_symbol_binding = true;",
        "bool deterministic_runtime_shim_host_link_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; runtime_shim_host_link_lowering = "',
        "frontend_objc_runtime_shim_host_link_profile",
        "!objc3.objc_runtime_shim_host_link = !{!13}",
    ):
        assert marker in ir_source
