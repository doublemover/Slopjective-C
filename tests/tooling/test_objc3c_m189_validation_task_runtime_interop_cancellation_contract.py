import hashlib
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
FIXTURE_ROOT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "objc3c"
    / "m189_validation_task_runtime_interop_cancellation_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m189-task-runtime-interop-cancellation-lowering-v1"
LOWERING_PREFIX = "; task_runtime_interop_cancellation_lowering = "
PROFILE_PREFIX = "; frontend_objc_task_runtime_interop_cancellation_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_task_runtime_interop_cancellation_lowering_profile = "
    r"task_runtime_sites=(\d+), task_runtime_interop_sites=(\d+), "
    r"cancellation_probe_sites=(\d+), cancellation_handler_sites=(\d+), "
    r"runtime_resume_sites=(\d+), runtime_cancel_sites=(\d+), "
    r"normalized_sites=(\d+), guard_blocked_sites=(\d+), "
    r"contract_violation_sites=(\d+), "
    r"deterministic_task_runtime_interop_cancellation_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!40 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    task_runtime_sites: int,
    task_runtime_interop_sites: int,
    cancellation_probe_sites: int,
    cancellation_handler_sites: int,
    runtime_resume_sites: int,
    runtime_cancel_sites: int,
    normalized_sites: int,
    guard_blocked_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"task_runtime_sites={task_runtime_sites};"
        f"task_runtime_interop_sites={task_runtime_interop_sites};"
        f"cancellation_probe_sites={cancellation_probe_sites};"
        f"cancellation_handler_sites={cancellation_handler_sites};"
        f"runtime_resume_sites={runtime_resume_sites};"
        f"runtime_cancel_sites={runtime_cancel_sites};"
        f"normalized_sites={normalized_sites};"
        f"guard_blocked_sites={guard_blocked_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["task_runtime_interop_cancellation_lowering_sites"],
        sema["task_runtime_interop_cancellation_lowering_runtime_interop_sites"],
        sema["task_runtime_interop_cancellation_lowering_cancellation_probe_sites"],
        sema["task_runtime_interop_cancellation_lowering_cancellation_handler_sites"],
        sema["task_runtime_interop_cancellation_lowering_runtime_resume_sites"],
        sema["task_runtime_interop_cancellation_lowering_runtime_cancel_sites"],
        sema["task_runtime_interop_cancellation_lowering_normalized_sites"],
        sema["task_runtime_interop_cancellation_lowering_guard_blocked_sites"],
        sema["task_runtime_interop_cancellation_lowering_contract_violation_sites"],
    )


def _extract_profile_counts_and_deterministic(
    ir_text: str,
) -> tuple[tuple[int, int, int, int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:9])
            return counts, match.group(10) == "true"
    raise AssertionError(f"missing IR profile marker: {PROFILE_PREFIX}")


def _extract_lowering_replay_key(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(LOWERING_PREFIX):
            return line.removeprefix(LOWERING_PREFIX)
    raise AssertionError(f"missing IR replay-key marker: {LOWERING_PREFIX}")


def _extract_metadata_counts_and_deterministic(
    ir_text: str,
) -> tuple[tuple[int, int, int, int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:9])
            return counts, match.group(10) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !40 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"][
        "objc_task_runtime_interop_cancellation_lowering_surface"
    ]
    lowering = manifest["lowering_task_runtime_interop_cancellation"]

    task_runtime_sites = sema["task_runtime_interop_cancellation_lowering_sites"]
    task_runtime_interop_sites = sema["task_runtime_interop_cancellation_lowering_runtime_interop_sites"]
    cancellation_probe_sites = sema["task_runtime_interop_cancellation_lowering_cancellation_probe_sites"]
    cancellation_handler_sites = sema[
        "task_runtime_interop_cancellation_lowering_cancellation_handler_sites"
    ]
    runtime_resume_sites = sema["task_runtime_interop_cancellation_lowering_runtime_resume_sites"]
    runtime_cancel_sites = sema["task_runtime_interop_cancellation_lowering_runtime_cancel_sites"]
    normalized_sites = sema["task_runtime_interop_cancellation_lowering_normalized_sites"]
    guard_blocked_sites = sema["task_runtime_interop_cancellation_lowering_guard_blocked_sites"]
    contract_violation_sites = sema[
        "task_runtime_interop_cancellation_lowering_contract_violation_sites"
    ]
    deterministic_handoff = sema["deterministic_task_runtime_interop_cancellation_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        task_runtime_sites=task_runtime_sites,
        task_runtime_interop_sites=task_runtime_interop_sites,
        cancellation_probe_sites=cancellation_probe_sites,
        cancellation_handler_sites=cancellation_handler_sites,
        runtime_resume_sites=runtime_resume_sites,
        runtime_cancel_sites=runtime_cancel_sites,
        normalized_sites=normalized_sites,
        guard_blocked_sites=guard_blocked_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert task_runtime_interop_sites <= task_runtime_sites
    assert cancellation_probe_sites <= task_runtime_sites
    assert cancellation_handler_sites <= task_runtime_sites
    assert runtime_resume_sites <= task_runtime_sites
    assert runtime_cancel_sites <= task_runtime_sites
    assert normalized_sites <= task_runtime_sites
    assert guard_blocked_sites <= task_runtime_sites
    assert contract_violation_sites <= task_runtime_sites
    assert runtime_resume_sites + runtime_cancel_sites <= task_runtime_sites
    assert normalized_sites + guard_blocked_sites == task_runtime_sites

    assert deterministic_handoff is True
    assert sema["lowering_task_runtime_interop_cancellation_replay_key"] == expected_replay_key

    assert surface["task_runtime_sites"] == task_runtime_sites
    assert surface["task_runtime_interop_sites"] == task_runtime_interop_sites
    assert surface["cancellation_probe_sites"] == cancellation_probe_sites
    assert surface["cancellation_handler_sites"] == cancellation_handler_sites
    assert surface["runtime_resume_sites"] == runtime_resume_sites
    assert surface["runtime_cancel_sites"] == runtime_cancel_sites
    assert surface["normalized_sites"] == normalized_sites
    assert surface["guard_blocked_sites"] == guard_blocked_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m189_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M189 validation/conformance/perf task runtime interop and cancellation runbook (M189-D001)",
        "python -m pytest tests/tooling/test_objc3c_m189_validation_task_runtime_interop_cancellation_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m189_conformance_task_runtime_interop_cancellation_contract.py -q",
        "lowering_task_runtime_interop_cancellation.replay_key",
        "deterministic_task_runtime_interop_cancellation_lowering_handoff",
        "task_runtime_interop_cancellation_lowering",
        "frontend_objc_task_runtime_interop_cancellation_lowering_profile",
        "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}",
        "normalized_sites + guard_blocked_sites == task_runtime_sites",
    ):
        assert text in fragment


def test_m189_validation_manifest_packets_hold_deterministic_task_runtime_interop_cancellation_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m189_validation_ir_markers_match_manifest_task_runtime_interop_cancellation_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_task_runtime_interop_cancellation"][
            "replay_key"
        ]


def test_m189_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_task_runtime_interop_cancellation"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_task_runtime_interop_cancellation"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
