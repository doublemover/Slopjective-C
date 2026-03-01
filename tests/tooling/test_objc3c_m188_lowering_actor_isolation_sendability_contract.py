from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m188_lowering_actor_isolation_sendability_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Actor isolation and sendability lowering artifact contract (M188-C001)",
        "kObjc3ActorIsolationSendabilityLoweringLaneContract",
        "Objc3ActorIsolationSendabilityLoweringContract",
        "IsValidObjc3ActorIsolationSendabilityLoweringContract(...)",
        "Objc3ActorIsolationSendabilityLoweringReplayKey(...)",
        "actor_isolation_sendability_lowering = actor_isolation_sites=<N>",
        "frontend_objc_actor_isolation_sendability_lowering_profile",
        "!objc3.objc_actor_isolation_sendability_lowering = !{!41}",
        "python -m pytest tests/tooling/test_objc3c_m188_lowering_actor_isolation_sendability_contract.py -q",
    ):
        assert text in fragment


def test_m188_lowering_actor_isolation_sendability_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3ActorIsolationSendabilityLoweringLaneContract",
        "struct Objc3ActorIsolationSendabilityLoweringContract",
        "std::size_t actor_isolation_sites = 0;",
        "std::size_t sendability_check_sites = 0;",
        "std::size_t cross_actor_hop_sites = 0;",
        "std::size_t non_sendable_capture_sites = 0;",
        "std::size_t sendable_transfer_sites = 0;",
        "std::size_t isolation_boundary_sites = 0;",
        "std::size_t guard_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3ActorIsolationSendabilityLoweringContract(",
        "std::string Objc3ActorIsolationSendabilityLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3ActorIsolationSendabilityLoweringContract(",
        "Objc3ActorIsolationSendabilityLoweringReplayKey(",
        "contract.isolation_boundary_sites + contract.guard_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"actor_isolation_sites="',
        '";sendability_check_sites="',
        '";cross_actor_hop_sites="',
        '";non_sendable_capture_sites="',
        '";sendable_transfer_sites="',
        '";isolation_boundary_sites="',
        '";guard_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3ActorIsolationSendabilityLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_actor_isolation_sendability_replay_key;",
        "std::size_t actor_isolation_sendability_lowering_sites = 0;",
        "std::size_t actor_isolation_sendability_lowering_sendability_check_sites = 0;",
        "std::size_t actor_isolation_sendability_lowering_cross_actor_hop_sites = 0;",
        "std::size_t actor_isolation_sendability_lowering_non_sendable_capture_sites =",
        "std::size_t actor_isolation_sendability_lowering_sendable_transfer_sites = 0;",
        "std::size_t actor_isolation_sendability_lowering_isolation_boundary_sites = 0;",
        "std::size_t actor_isolation_sendability_lowering_guard_blocked_sites = 0;",
        "std::size_t actor_isolation_sendability_lowering_contract_violation_sites = 0;",
        "bool deterministic_actor_isolation_sendability_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; actor_isolation_sendability_lowering = "',
        "frontend_objc_actor_isolation_sendability_lowering_profile",
        "!objc3.objc_actor_isolation_sendability_lowering = !{!41}",
        "!41 = !{i64 ",
    ):
        assert marker in ir_source
