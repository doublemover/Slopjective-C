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


def test_m158_lowering_nil_receiver_semantics_foldability_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Nil-receiver semantics/foldability artifact contract (M158-C001)",
        "kObjc3NilReceiverSemanticsFoldabilityLaneContract",
        "Objc3NilReceiverSemanticsFoldabilityContract",
        "IsValidObjc3NilReceiverSemanticsFoldabilityContract(...)",
        "Objc3NilReceiverSemanticsFoldabilityReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.nil_receiver_semantics_foldability_message_send_sites",
        "frontend.pipeline.semantic_surface.objc_nil_receiver_semantics_foldability_surface",
        "lowering_nil_receiver_semantics_foldability.replay_key",
        "nil_receiver_semantics_foldability_lowering = message_send_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m158_lowering_nil_receiver_semantics_foldability_contract.py -q",
    ):
        assert text in fragment


def test_m158_lowering_nil_receiver_semantics_foldability_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3NilReceiverSemanticsFoldabilityLaneContract",
        "struct Objc3NilReceiverSemanticsFoldabilityContract",
        "std::size_t receiver_nil_literal_sites = 0;",
        "std::size_t nil_receiver_semantics_enabled_sites = 0;",
        "std::size_t nil_receiver_foldable_sites = 0;",
        "std::size_t nil_receiver_runtime_dispatch_required_sites = 0;",
        "std::size_t non_nil_receiver_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3NilReceiverSemanticsFoldabilityContract(",
        "Objc3NilReceiverSemanticsFoldabilityReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3NilReceiverSemanticsFoldabilityContract(",
        "Objc3NilReceiverSemanticsFoldabilityReplayKey(",
        '"message_send_sites="',
        '";contract_violation_sites="',
        '";lane_contract=" + kObjc3NilReceiverSemanticsFoldabilityLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildNilReceiverSemanticsFoldabilityContract(",
        "IsValidObjc3NilReceiverSemanticsFoldabilityContract(",
        "nil_receiver_semantics_foldability_replay_key",
        '\\"deterministic_nil_receiver_semantics_foldability_handoff\\":',
        '\\"objc_nil_receiver_semantics_foldability_surface\\":{\\"message_send_sites\\":',
        '\\"lowering_nil_receiver_semantics_foldability\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_nil_receiver_semantics_foldability_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_nil_receiver_semantics_foldability_replay_key;",
        "std::size_t nil_receiver_semantics_foldability_message_send_sites = 0;",
        "std::size_t nil_receiver_semantics_foldability_receiver_nil_literal_sites = 0;",
        "std::size_t nil_receiver_semantics_foldability_enabled_sites = 0;",
        "std::size_t nil_receiver_semantics_foldability_foldable_sites = 0;",
        "std::size_t nil_receiver_semantics_foldability_runtime_dispatch_required_sites = 0;",
        "std::size_t nil_receiver_semantics_foldability_non_nil_receiver_sites = 0;",
        "std::size_t nil_receiver_semantics_foldability_contract_violation_sites = 0;",
        "bool deterministic_nil_receiver_semantics_foldability_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; nil_receiver_semantics_foldability_lowering = "',
        "frontend_objc_nil_receiver_semantics_foldability_profile",
        "!objc3.objc_nil_receiver_semantics_foldability = !{!11}",
    ):
        assert marker in ir_source
