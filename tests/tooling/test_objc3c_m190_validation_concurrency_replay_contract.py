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
    / "m190_validation_concurrency_replay_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m190-concurrency-replay-race-guard-lowering-v1"
LOWERING_PREFIX = "; concurrency_replay_race_guard_lowering = "
PROFILE_PREFIX = "; frontend_objc_concurrency_replay_race_guard_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_concurrency_replay_race_guard_lowering_profile = "
    r"concurrency_replay_sites=(\d+), replay_proof_sites=(\d+), "
    r"race_guard_sites=(\d+), task_handoff_sites=(\d+), "
    r"actor_isolation_sites=(\d+), deterministic_schedule_sites=(\d+), "
    r"guard_blocked_sites=(\d+), contract_violation_sites=(\d+), "
    r"deterministic_concurrency_replay_race_guard_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!39 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    concurrency_replay_sites: int,
    replay_proof_sites: int,
    race_guard_sites: int,
    task_handoff_sites: int,
    actor_isolation_sites: int,
    deterministic_schedule_sites: int,
    guard_blocked_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"concurrency_replay_sites={concurrency_replay_sites};"
        f"replay_proof_sites={replay_proof_sites};"
        f"race_guard_sites={race_guard_sites};"
        f"task_handoff_sites={task_handoff_sites};"
        f"actor_isolation_sites={actor_isolation_sites};"
        f"deterministic_schedule_sites={deterministic_schedule_sites};"
        f"guard_blocked_sites={guard_blocked_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["concurrency_replay_race_guard_lowering_sites"],
        sema["concurrency_replay_race_guard_lowering_replay_proof_sites"],
        sema["concurrency_replay_race_guard_lowering_race_guard_sites"],
        sema["concurrency_replay_race_guard_lowering_task_handoff_sites"],
        sema["concurrency_replay_race_guard_lowering_actor_isolation_sites"],
        sema["concurrency_replay_race_guard_lowering_deterministic_schedule_sites"],
        sema["concurrency_replay_race_guard_lowering_guard_blocked_sites"],
        sema["concurrency_replay_race_guard_lowering_contract_violation_sites"],
    )


def _extract_profile_counts_and_deterministic(
    ir_text: str,
) -> tuple[tuple[int, int, int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:8])
            return counts, match.group(9) == "true"
    raise AssertionError(f"missing IR profile marker: {PROFILE_PREFIX}")


def _extract_lowering_replay_key(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(LOWERING_PREFIX):
            return line.removeprefix(LOWERING_PREFIX)
    raise AssertionError(f"missing IR replay-key marker: {LOWERING_PREFIX}")


def _extract_metadata_counts_and_deterministic(
    ir_text: str,
) -> tuple[tuple[int, int, int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:8])
            return counts, match.group(9) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !39 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"][
        "objc_concurrency_replay_race_guard_lowering_surface"
    ]
    lowering = manifest["lowering_concurrency_replay_race_guard"]

    concurrency_replay_sites = sema["concurrency_replay_race_guard_lowering_sites"]
    replay_proof_sites = sema["concurrency_replay_race_guard_lowering_replay_proof_sites"]
    race_guard_sites = sema["concurrency_replay_race_guard_lowering_race_guard_sites"]
    task_handoff_sites = sema["concurrency_replay_race_guard_lowering_task_handoff_sites"]
    actor_isolation_sites = sema["concurrency_replay_race_guard_lowering_actor_isolation_sites"]
    deterministic_schedule_sites = sema[
        "concurrency_replay_race_guard_lowering_deterministic_schedule_sites"
    ]
    guard_blocked_sites = sema["concurrency_replay_race_guard_lowering_guard_blocked_sites"]
    contract_violation_sites = sema[
        "concurrency_replay_race_guard_lowering_contract_violation_sites"
    ]
    deterministic_handoff = sema["deterministic_concurrency_replay_race_guard_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        concurrency_replay_sites=concurrency_replay_sites,
        replay_proof_sites=replay_proof_sites,
        race_guard_sites=race_guard_sites,
        task_handoff_sites=task_handoff_sites,
        actor_isolation_sites=actor_isolation_sites,
        deterministic_schedule_sites=deterministic_schedule_sites,
        guard_blocked_sites=guard_blocked_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert replay_proof_sites <= concurrency_replay_sites
    assert race_guard_sites <= concurrency_replay_sites
    assert task_handoff_sites <= concurrency_replay_sites
    assert actor_isolation_sites <= concurrency_replay_sites
    assert deterministic_schedule_sites <= concurrency_replay_sites
    assert guard_blocked_sites <= concurrency_replay_sites
    assert contract_violation_sites <= concurrency_replay_sites
    assert deterministic_schedule_sites + guard_blocked_sites == concurrency_replay_sites

    assert deterministic_handoff is True
    assert sema["lowering_concurrency_replay_race_guard_replay_key"] == expected_replay_key

    assert surface["concurrency_replay_sites"] == concurrency_replay_sites
    assert surface["replay_proof_sites"] == replay_proof_sites
    assert surface["race_guard_sites"] == race_guard_sites
    assert surface["task_handoff_sites"] == task_handoff_sites
    assert surface["actor_isolation_sites"] == actor_isolation_sites
    assert surface["deterministic_schedule_sites"] == deterministic_schedule_sites
    assert surface["guard_blocked_sites"] == guard_blocked_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m190_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M190 validation/conformance/perf concurrency replay-proof and race-guard runbook (M190-D001)",
        "python -m pytest tests/tooling/test_objc3c_m190_validation_concurrency_replay_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m190_conformance_concurrency_replay_contract.py -q",
        "lowering_concurrency_replay_race_guard.replay_key",
        "deterministic_concurrency_replay_race_guard_lowering_handoff",
        "concurrency_replay_race_guard_lowering",
        "frontend_objc_concurrency_replay_race_guard_lowering_profile",
        "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}",
        "deterministic_schedule_sites + guard_blocked_sites == concurrency_replay_sites",
    ):
        assert text in fragment


def test_m190_validation_manifest_packets_hold_deterministic_concurrency_replay_race_guard_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m190_validation_ir_markers_match_manifest_concurrency_replay_race_guard_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_concurrency_replay_race_guard"][
            "replay_key"
        ]


def test_m190_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_concurrency_replay_race_guard"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_concurrency_replay_race_guard"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
