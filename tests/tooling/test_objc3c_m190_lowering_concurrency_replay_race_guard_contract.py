from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m190_lowering_concurrency_replay_race_guard_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Concurrency replay-proof and race-guard lowering artifact contract (M190-C001)",
        "kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract",
        "Objc3ConcurrencyReplayRaceGuardLoweringContract",
        "IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(...)",
        "Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(...)",
        "concurrency_replay_race_guard_lowering = concurrency_replay_sites=<N>",
        "frontend_objc_concurrency_replay_race_guard_lowering_profile",
        "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}",
        "python -m pytest tests/tooling/test_objc3c_m190_lowering_concurrency_replay_race_guard_contract.py -q",
    ):
        assert text in fragment


def test_m190_lowering_concurrency_replay_race_guard_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract",
        "struct Objc3ConcurrencyReplayRaceGuardLoweringContract",
        "std::size_t concurrency_replay_sites = 0;",
        "std::size_t replay_proof_sites = 0;",
        "std::size_t race_guard_sites = 0;",
        "std::size_t task_handoff_sites = 0;",
        "std::size_t actor_isolation_sites = 0;",
        "std::size_t deterministic_schedule_sites = 0;",
        "std::size_t guard_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(",
        "std::string Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(",
        "Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(",
        "contract.deterministic_schedule_sites + contract.guard_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"concurrency_replay_sites="',
        '";replay_proof_sites="',
        '";race_guard_sites="',
        '";task_handoff_sites="',
        '";actor_isolation_sites="',
        '";deterministic_schedule_sites="',
        '";guard_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_concurrency_replay_race_guard_replay_key;",
        "std::size_t concurrency_replay_race_guard_lowering_sites = 0;",
        "std::size_t concurrency_replay_race_guard_lowering_replay_proof_sites = 0;",
        "std::size_t concurrency_replay_race_guard_lowering_race_guard_sites = 0;",
        "std::size_t concurrency_replay_race_guard_lowering_task_handoff_sites = 0;",
        "std::size_t concurrency_replay_race_guard_lowering_actor_isolation_sites = 0;",
        "std::size_t concurrency_replay_race_guard_lowering_deterministic_schedule_sites = 0;",
        "std::size_t concurrency_replay_race_guard_lowering_guard_blocked_sites = 0;",
        "std::size_t concurrency_replay_race_guard_lowering_contract_violation_sites = 0;",
        "bool deterministic_concurrency_replay_race_guard_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; concurrency_replay_race_guard_lowering = "',
        "frontend_objc_concurrency_replay_race_guard_lowering_profile",
        "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}",
        "!39 = !{i64 ",
    ):
        assert marker in ir_source
