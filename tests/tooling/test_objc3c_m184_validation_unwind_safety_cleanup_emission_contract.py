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
    / "m184_validation_unwind_safety_cleanup_emission_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m184-unwind-safety-cleanup-emission-lowering-v1"
LOWERING_PREFIX = "; unwind_cleanup_lowering = "
PROFILE_PREFIX = "; frontend_objc_unwind_cleanup_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_unwind_cleanup_lowering_profile = "
    r"unwind_cleanup_sites=(\d+), exceptional_exit_sites=(\d+), "
    r"cleanup_action_sites=(\d+), cleanup_scope_sites=(\d+), "
    r"cleanup_resume_sites=(\d+), normalized_sites=(\d+), "
    r"fail_closed_sites=(\d+), contract_violation_sites=(\d+), "
    r"deterministic_unwind_cleanup_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!44 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    unwind_cleanup_sites: int,
    exceptional_exit_sites: int,
    cleanup_action_sites: int,
    cleanup_scope_sites: int,
    cleanup_resume_sites: int,
    normalized_sites: int,
    fail_closed_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"unwind_cleanup_sites={unwind_cleanup_sites};"
        f"exceptional_exit_sites={exceptional_exit_sites};"
        f"cleanup_action_sites={cleanup_action_sites};"
        f"cleanup_scope_sites={cleanup_scope_sites};"
        f"cleanup_resume_sites={cleanup_resume_sites};"
        f"normalized_sites={normalized_sites};"
        f"fail_closed_sites={fail_closed_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["unwind_cleanup_lowering_sites"],
        sema["unwind_cleanup_lowering_exceptional_exit_sites"],
        sema["unwind_cleanup_lowering_cleanup_action_sites"],
        sema["unwind_cleanup_lowering_cleanup_scope_sites"],
        sema["unwind_cleanup_lowering_cleanup_resume_sites"],
        sema["unwind_cleanup_lowering_normalized_sites"],
        sema["unwind_cleanup_lowering_fail_closed_sites"],
        sema["unwind_cleanup_lowering_contract_violation_sites"],
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
    raise AssertionError("missing LLVM metadata tuple marker: !44 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"][
        "objc_unwind_cleanup_lowering_surface"
    ]
    lowering = manifest["lowering_unwind_cleanup"]

    unwind_cleanup_sites = sema["unwind_cleanup_lowering_sites"]
    exceptional_exit_sites = sema["unwind_cleanup_lowering_exceptional_exit_sites"]
    cleanup_action_sites = sema["unwind_cleanup_lowering_cleanup_action_sites"]
    cleanup_scope_sites = sema["unwind_cleanup_lowering_cleanup_scope_sites"]
    cleanup_resume_sites = sema["unwind_cleanup_lowering_cleanup_resume_sites"]
    normalized_sites = sema["unwind_cleanup_lowering_normalized_sites"]
    fail_closed_sites = sema["unwind_cleanup_lowering_fail_closed_sites"]
    contract_violation_sites = sema["unwind_cleanup_lowering_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_unwind_cleanup_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        unwind_cleanup_sites=unwind_cleanup_sites,
        exceptional_exit_sites=exceptional_exit_sites,
        cleanup_action_sites=cleanup_action_sites,
        cleanup_scope_sites=cleanup_scope_sites,
        cleanup_resume_sites=cleanup_resume_sites,
        normalized_sites=normalized_sites,
        fail_closed_sites=fail_closed_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert exceptional_exit_sites <= unwind_cleanup_sites
    assert cleanup_action_sites <= unwind_cleanup_sites
    assert cleanup_scope_sites <= unwind_cleanup_sites
    assert cleanup_resume_sites <= unwind_cleanup_sites
    assert normalized_sites <= unwind_cleanup_sites
    assert fail_closed_sites <= unwind_cleanup_sites
    assert contract_violation_sites <= unwind_cleanup_sites
    assert normalized_sites + fail_closed_sites == unwind_cleanup_sites

    assert deterministic_handoff is True
    assert sema["lowering_unwind_cleanup_replay_key"] == expected_replay_key

    assert surface["unwind_cleanup_sites"] == unwind_cleanup_sites
    assert surface["exceptional_exit_sites"] == exceptional_exit_sites
    assert surface["cleanup_action_sites"] == cleanup_action_sites
    assert surface["cleanup_scope_sites"] == cleanup_scope_sites
    assert surface["cleanup_resume_sites"] == cleanup_resume_sites
    assert surface["normalized_sites"] == normalized_sites
    assert surface["fail_closed_sites"] == fail_closed_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m184_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M184 validation/conformance/perf unwind safety and cleanup emission runbook (M184-D001)",
        "python -m pytest tests/tooling/test_objc3c_m184_validation_unwind_safety_cleanup_emission_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m184_conformance_unwind_safety_cleanup_emission_contract.py -q",
        "lowering_unwind_cleanup.replay_key",
        "deterministic_unwind_cleanup_lowering_handoff",
        "unwind_cleanup_lowering",
        "frontend_objc_unwind_cleanup_lowering_profile",
        "!objc3.objc_unwind_cleanup_lowering = !{!44}",
        "normalized_sites + fail_closed_sites == unwind_cleanup_sites",
    ):
        assert text in fragment


def test_m184_validation_manifest_packets_hold_deterministic_unwind_cleanup_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m184_validation_ir_markers_match_manifest_unwind_cleanup_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_unwind_cleanup_lowering = !{!44}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_unwind_cleanup"]["replay_key"]


def test_m184_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_unwind_cleanup"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_unwind_cleanup"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
