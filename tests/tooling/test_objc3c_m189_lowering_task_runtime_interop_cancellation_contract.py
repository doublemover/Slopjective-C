from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_METADATA_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m189_lowering_task_runtime_interop_cancellation_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Task runtime interop and cancellation lowering artifact contract (M189-C001)",
        "kObjc3TaskRuntimeInteropCancellationLoweringLaneContract",
        "Objc3TaskRuntimeInteropCancellationLoweringContract",
        "IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(...)",
        "Objc3TaskRuntimeInteropCancellationLoweringReplayKey(...)",
        "task_runtime_interop_cancellation_lowering = task_runtime_sites=<N>",
        "frontend_objc_task_runtime_interop_cancellation_lowering_profile",
        "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}",
        "python -m pytest tests/tooling/test_objc3c_m189_lowering_task_runtime_interop_cancellation_contract.py -q",
    ):
        assert text in fragment


def test_m189_lowering_task_runtime_interop_cancellation_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)
    ir_header = _read(IR_METADATA_HEADER)
    ir_source = _read(IR_EMITTER_SOURCE)

    for marker in (
        "kObjc3TaskRuntimeInteropCancellationLoweringLaneContract",
        "struct Objc3TaskRuntimeInteropCancellationLoweringContract",
        "std::size_t task_runtime_sites = 0;",
        "std::size_t task_runtime_interop_sites = 0;",
        "std::size_t cancellation_probe_sites = 0;",
        "std::size_t cancellation_handler_sites = 0;",
        "std::size_t runtime_resume_sites = 0;",
        "std::size_t runtime_cancel_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t guard_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(",
        "std::string Objc3TaskRuntimeInteropCancellationLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(",
        "Objc3TaskRuntimeInteropCancellationLoweringReplayKey(",
        "contract.runtime_resume_sites + contract.runtime_cancel_sites >",
        "contract.normalized_sites + contract.guard_blocked_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"task_runtime_sites="',
        '";task_runtime_interop_sites="',
        '";cancellation_probe_sites="',
        '";cancellation_handler_sites="',
        '";runtime_resume_sites="',
        '";runtime_cancel_sites="',
        '";normalized_sites="',
        '";guard_blocked_sites="',
        '";contract_violation_sites="',
        "kObjc3TaskRuntimeInteropCancellationLoweringLaneContract",
    ):
        assert marker in source

    for marker in (
        "std::string lowering_task_runtime_interop_cancellation_replay_key;",
        "std::size_t task_runtime_interop_cancellation_lowering_sites = 0;",
        "std::size_t task_runtime_interop_cancellation_lowering_runtime_interop_sites =",
        "task_runtime_interop_cancellation_lowering_cancellation_probe_sites = 0;",
        "task_runtime_interop_cancellation_lowering_cancellation_handler_sites = 0;",
        "std::size_t task_runtime_interop_cancellation_lowering_runtime_resume_sites =",
        "std::size_t task_runtime_interop_cancellation_lowering_runtime_cancel_sites =",
        "std::size_t task_runtime_interop_cancellation_lowering_normalized_sites = 0;",
        "std::size_t task_runtime_interop_cancellation_lowering_guard_blocked_sites =",
        "task_runtime_interop_cancellation_lowering_contract_violation_sites = 0;",
        "bool deterministic_task_runtime_interop_cancellation_lowering_handoff =",
    ):
        assert marker in ir_header

    for marker in (
        'out << "; task_runtime_interop_cancellation_lowering = "',
        "frontend_objc_task_runtime_interop_cancellation_lowering_profile",
        "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}",
        "!40 = !{i64 ",
    ):
        assert marker in ir_source
