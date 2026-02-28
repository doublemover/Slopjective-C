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


def test_m159_lowering_super_dispatch_method_family_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Super-dispatch/method-family artifact contract (M159-C001)",
        "kObjc3SuperDispatchMethodFamilyLaneContract",
        "Objc3SuperDispatchMethodFamilyContract",
        "IsValidObjc3SuperDispatchMethodFamilyContract(...)",
        "Objc3SuperDispatchMethodFamilyReplayKey(...)",
        "frontend.pipeline.sema_pass_manager.super_dispatch_method_family_message_send_sites",
        "frontend.pipeline.semantic_surface.objc_super_dispatch_method_family_surface",
        "lowering_super_dispatch_method_family.replay_key",
        "super_dispatch_method_family_lowering = message_send_sites=<N>",
        "python -m pytest tests/tooling/test_objc3c_m159_lowering_super_dispatch_method_family_contract.py -q",
    ):
        assert text in fragment


def test_m159_lowering_super_dispatch_method_family_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3SuperDispatchMethodFamilyLaneContract",
        "struct Objc3SuperDispatchMethodFamilyContract",
        "std::size_t receiver_super_identifier_sites = 0;",
        "std::size_t super_dispatch_enabled_sites = 0;",
        "std::size_t super_dispatch_requires_class_context_sites = 0;",
        "std::size_t method_family_init_sites = 0;",
        "std::size_t method_family_copy_sites = 0;",
        "std::size_t method_family_mutable_copy_sites = 0;",
        "std::size_t method_family_new_sites = 0;",
        "std::size_t method_family_none_sites = 0;",
        "std::size_t method_family_returns_retained_result_sites = 0;",
        "std::size_t method_family_returns_related_result_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "IsValidObjc3SuperDispatchMethodFamilyContract(",
        "Objc3SuperDispatchMethodFamilyReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3SuperDispatchMethodFamilyContract(",
        "Objc3SuperDispatchMethodFamilyReplayKey(",
        '"message_send_sites="',
        '";contract_violation_sites="',
        '";lane_contract=" + kObjc3SuperDispatchMethodFamilyLaneContract',
    ):
        assert marker in source

    for marker in (
        "BuildSuperDispatchMethodFamilyContract(",
        "IsValidObjc3SuperDispatchMethodFamilyContract(",
        "super_dispatch_method_family_replay_key",
        '\\"deterministic_super_dispatch_method_family_handoff\\":',
        '\\"objc_super_dispatch_method_family_surface\\":{\\"message_send_sites\\":',
        '\\"lowering_super_dispatch_method_family\\":{\\"replay_key\\":\\"',
        "ir_frontend_metadata.lowering_super_dispatch_method_family_replay_key =",
    ):
        assert marker in artifacts_source

    for marker in (
        "std::string lowering_super_dispatch_method_family_replay_key;",
        "std::size_t super_dispatch_method_family_message_send_sites = 0;",
        "std::size_t super_dispatch_method_family_receiver_super_identifier_sites = 0;",
        "std::size_t super_dispatch_method_family_enabled_sites = 0;",
        "std::size_t super_dispatch_method_family_requires_class_context_sites = 0;",
        "std::size_t super_dispatch_method_family_init_sites = 0;",
        "std::size_t super_dispatch_method_family_copy_sites = 0;",
        "std::size_t super_dispatch_method_family_mutable_copy_sites = 0;",
        "std::size_t super_dispatch_method_family_new_sites = 0;",
        "std::size_t super_dispatch_method_family_none_sites = 0;",
        "std::size_t super_dispatch_method_family_returns_retained_result_sites = 0;",
        "std::size_t super_dispatch_method_family_returns_related_result_sites = 0;",
        "std::size_t super_dispatch_method_family_contract_violation_sites = 0;",
        "bool deterministic_super_dispatch_method_family_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; super_dispatch_method_family_lowering = "',
        "frontend_objc_super_dispatch_method_family_profile",
        "!objc3.objc_super_dispatch_method_family = !{!12}",
    ):
        assert marker in ir_source
