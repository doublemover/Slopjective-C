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


def test_m156_lowering_message_send_selector_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Message-send selector-lowering artifact contract (M156-C001)",
        "kObjc3MessageSendSelectorLoweringLaneContract",
        "Objc3MessageSendSelectorLoweringContract",
        "IsValidObjc3MessageSendSelectorLoweringContract(...)",
        "Objc3MessageSendSelectorLoweringReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.message_send_selector_lowering_sites",
        "frontend.pipeline.semantic_surface.objc_message_send_selector_lowering_surface",
        "lowering_message_send_selector_lowering.replay_key",
        "message_send_selector_lowering = message_send_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m156_lowering_message_send_selector_lowering_contract.py -q",
    ):
        assert text in fragment


def test_m156_lowering_message_send_selector_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3MessageSendSelectorLoweringLaneContract",
        "struct Objc3MessageSendSelectorLoweringContract",
        "std::size_t message_send_sites = 0;",
        "std::size_t unary_selector_sites = 0;",
        "std::size_t keyword_selector_sites = 0;",
        "std::size_t selector_piece_sites = 0;",
        "std::size_t argument_expression_sites = 0;",
        "std::size_t receiver_expression_sites = 0;",
        "std::size_t selector_literal_entries = 0;",
        "std::size_t selector_literal_characters = 0;",
        "IsValidObjc3MessageSendSelectorLoweringContract(",
        "Objc3MessageSendSelectorLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3MessageSendSelectorLoweringContract(",
        "Objc3MessageSendSelectorLoweringReplayKey(",
        '"message_send_sites="',
        '";selector_literal_characters="',
        '";lane_contract=" + kObjc3MessageSendSelectorLoweringLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildMessageSendSelectorLoweringContract(",
        "IsValidObjc3MessageSendSelectorLoweringContract(",
        "message_send_selector_lowering_replay_key",
        '\\"deterministic_message_send_selector_lowering_handoff\\":',
        '\\"objc_message_send_selector_lowering_surface\\":{\\"message_send_sites\\":',
        '\\"lowering_message_send_selector_lowering\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_message_send_selector_lowering_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_message_send_selector_lowering_replay_key;",
        "std::size_t message_send_selector_lowering_sites = 0;",
        "std::size_t message_send_selector_lowering_unary_sites = 0;",
        "std::size_t message_send_selector_lowering_keyword_sites = 0;",
        "std::size_t message_send_selector_lowering_selector_piece_sites = 0;",
        "std::size_t message_send_selector_lowering_argument_expression_sites = 0;",
        "std::size_t message_send_selector_lowering_receiver_sites = 0;",
        "std::size_t message_send_selector_lowering_selector_literal_entries = 0;",
        "std::size_t message_send_selector_lowering_selector_literal_characters = 0;",
        "bool deterministic_message_send_selector_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; message_send_selector_lowering = "',
        "frontend_objc_message_send_selector_lowering_profile",
        "!objc3.objc_message_send_selector_lowering = !{!9}",
    ):
        assert marker in ir_source
