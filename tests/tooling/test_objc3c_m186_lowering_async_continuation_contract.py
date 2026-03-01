from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m186_lowering_async_continuation_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Async grammar and continuation lowering artifact contract (M186-C001)",
        "kObjc3AsyncContinuationLoweringLaneContract",
        "Objc3AsyncContinuationLoweringContract",
        "IsValidObjc3AsyncContinuationLoweringContract(...)",
        "Objc3AsyncContinuationLoweringReplayKey(...)",
        "async_continuation_lowering = async_continuation_sites=<N>",
        "frontend_objc_async_continuation_lowering_profile",
        "!objc3.objc_async_continuation_lowering = !{!43}",
        "python -m pytest tests/tooling/test_objc3c_m186_lowering_async_continuation_contract.py -q",
    ):
        assert text in fragment


def test_m186_lowering_async_continuation_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3AsyncContinuationLoweringLaneContract",
        "struct Objc3AsyncContinuationLoweringContract",
        "std::size_t async_continuation_sites = 0;",
        "std::size_t async_keyword_sites = 0;",
        "std::size_t async_function_sites = 0;",
        "std::size_t continuation_allocation_sites = 0;",
        "std::size_t continuation_resume_sites = 0;",
        "std::size_t continuation_suspend_sites = 0;",
        "std::size_t async_state_machine_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t gate_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3AsyncContinuationLoweringContract(",
        "std::string Objc3AsyncContinuationLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3AsyncContinuationLoweringContract(",
        "Objc3AsyncContinuationLoweringReplayKey(",
        "contract.normalized_sites + contract.gate_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"async_continuation_sites="',
        '";async_keyword_sites="',
        '";async_function_sites="',
        '";continuation_allocation_sites="',
        '";continuation_resume_sites="',
        '";continuation_suspend_sites="',
        '";async_state_machine_sites="',
        '";normalized_sites="',
        '";gate_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3AsyncContinuationLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_async_continuation_replay_key;",
        "std::size_t async_continuation_lowering_sites = 0;",
        "std::size_t async_continuation_lowering_async_keyword_sites = 0;",
        "std::size_t async_continuation_lowering_async_function_sites = 0;",
        "std::size_t async_continuation_lowering_continuation_allocation_sites = 0;",
        "std::size_t async_continuation_lowering_continuation_resume_sites = 0;",
        "std::size_t async_continuation_lowering_continuation_suspend_sites = 0;",
        "std::size_t async_continuation_lowering_async_state_machine_sites = 0;",
        "std::size_t async_continuation_lowering_normalized_sites = 0;",
        "std::size_t async_continuation_lowering_gate_blocked_sites = 0;",
        "std::size_t async_continuation_lowering_contract_violation_sites = 0;",
        "bool deterministic_async_continuation_lowering_handoff = false;",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; async_continuation_lowering = "',
        "frontend_objc_async_continuation_lowering_profile",
        "!objc3.objc_async_continuation_lowering = !{!43}",
        "!43 = !{i64 ",
    ):
        assert marker in ir_source
