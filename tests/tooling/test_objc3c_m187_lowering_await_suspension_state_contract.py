from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m187_lowering_await_suspension_state_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Await lowering and suspension state lowering artifact contract (M187-C001)",
        "kObjc3AwaitLoweringSuspensionStateLoweringLaneContract",
        "Objc3AwaitLoweringSuspensionStateLoweringContract",
        "IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(...)",
        "Objc3AwaitLoweringSuspensionStateLoweringReplayKey(...)",
        "await_lowering_suspension_state_lowering = await_suspension_sites=<N>",
        "frontend_objc_await_lowering_suspension_state_lowering_profile",
        "!objc3.objc_await_lowering_suspension_state_lowering = !{!42}",
        "python -m pytest tests/tooling/test_objc3c_m187_lowering_await_suspension_state_contract.py -q",
    ):
        assert text in fragment


def test_m187_lowering_await_suspension_state_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3AwaitLoweringSuspensionStateLoweringLaneContract",
        "struct Objc3AwaitLoweringSuspensionStateLoweringContract",
        "std::size_t await_suspension_sites = 0;",
        "std::size_t await_keyword_sites = 0;",
        "std::size_t await_suspension_point_sites = 0;",
        "std::size_t await_resume_sites = 0;",
        "std::size_t await_state_machine_sites = 0;",
        "std::size_t await_continuation_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t gate_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(",
        "std::string Objc3AwaitLoweringSuspensionStateLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(",
        "Objc3AwaitLoweringSuspensionStateLoweringReplayKey(",
        "contract.await_resume_sites > contract.await_suspension_point_sites",
        "contract.await_state_machine_sites > contract.await_suspension_point_sites",
        "contract.normalized_sites + contract.gate_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"await_suspension_sites="',
        '";await_keyword_sites="',
        '";await_suspension_point_sites="',
        '";await_resume_sites="',
        '";await_state_machine_sites="',
        '";await_continuation_sites="',
        '";normalized_sites="',
        '";gate_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3AwaitLoweringSuspensionStateLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_await_lowering_suspension_state_replay_key;",
        "std::size_t await_lowering_suspension_state_lowering_sites = 0;",
        "await_lowering_suspension_state_lowering_await_keyword_sites = 0;",
        "await_lowering_suspension_state_lowering_await_suspension_point_sites =",
        "std::size_t await_lowering_suspension_state_lowering_await_resume_sites = 0;",
        "await_lowering_suspension_state_lowering_await_state_machine_sites = 0;",
        "await_lowering_suspension_state_lowering_await_continuation_sites = 0;",
        "std::size_t await_lowering_suspension_state_lowering_normalized_sites = 0;",
        "std::size_t await_lowering_suspension_state_lowering_gate_blocked_sites = 0;",
        "await_lowering_suspension_state_lowering_contract_violation_sites = 0;",
        "bool deterministic_await_lowering_suspension_state_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; await_lowering_suspension_state_lowering = "',
        "frontend_objc_await_lowering_suspension_state_lowering_profile",
        "!objc3.objc_await_lowering_suspension_state_lowering = !{!42}",
        "!42 = !{i64 ",
    ):
        assert marker in ir_source
