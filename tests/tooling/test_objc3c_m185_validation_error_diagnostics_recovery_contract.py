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
    / "m185_validation_error_diagnostics_recovery_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m185-error-diagnostics-ux-recovery-lowering-v1"
LOWERING_PREFIX = "; error_diagnostics_recovery_lowering = "
PROFILE_PREFIX = "; frontend_objc_error_diagnostics_recovery_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_error_diagnostics_recovery_lowering_profile = "
    r"error_diagnostics_recovery_sites=(\d+), diagnostic_emission_sites=(\d+), "
    r"recovery_attempt_sites=(\d+), recovery_success_sites=(\d+), "
    r"fixit_hint_sites=(\d+), diagnostic_rendering_sites=(\d+), "
    r"normalized_sites=(\d+), gate_blocked_sites=(\d+), "
    r"contract_violation_sites=(\d+), "
    r"deterministic_error_diagnostics_recovery_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!45 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    error_diagnostics_recovery_sites: int,
    diagnostic_emission_sites: int,
    recovery_attempt_sites: int,
    recovery_success_sites: int,
    fixit_hint_sites: int,
    diagnostic_rendering_sites: int,
    normalized_sites: int,
    gate_blocked_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"error_diagnostics_recovery_sites={error_diagnostics_recovery_sites};"
        f"diagnostic_emission_sites={diagnostic_emission_sites};"
        f"recovery_attempt_sites={recovery_attempt_sites};"
        f"recovery_success_sites={recovery_success_sites};"
        f"fixit_hint_sites={fixit_hint_sites};"
        f"diagnostic_rendering_sites={diagnostic_rendering_sites};"
        f"normalized_sites={normalized_sites};"
        f"gate_blocked_sites={gate_blocked_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["error_diagnostics_recovery_lowering_sites"],
        sema["error_diagnostics_recovery_lowering_diagnostic_emission_sites"],
        sema["error_diagnostics_recovery_lowering_recovery_attempt_sites"],
        sema["error_diagnostics_recovery_lowering_recovery_success_sites"],
        sema["error_diagnostics_recovery_lowering_fixit_hint_sites"],
        sema["error_diagnostics_recovery_lowering_diagnostic_rendering_sites"],
        sema["error_diagnostics_recovery_lowering_normalized_sites"],
        sema["error_diagnostics_recovery_lowering_gate_blocked_sites"],
        sema["error_diagnostics_recovery_lowering_contract_violation_sites"],
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
    raise AssertionError("missing LLVM metadata tuple marker: !45 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"][
        "objc_error_diagnostics_recovery_lowering_surface"
    ]
    lowering = manifest["lowering_error_diagnostics_recovery"]

    error_diagnostics_recovery_sites = sema["error_diagnostics_recovery_lowering_sites"]
    diagnostic_emission_sites = sema["error_diagnostics_recovery_lowering_diagnostic_emission_sites"]
    recovery_attempt_sites = sema["error_diagnostics_recovery_lowering_recovery_attempt_sites"]
    recovery_success_sites = sema["error_diagnostics_recovery_lowering_recovery_success_sites"]
    fixit_hint_sites = sema["error_diagnostics_recovery_lowering_fixit_hint_sites"]
    diagnostic_rendering_sites = sema["error_diagnostics_recovery_lowering_diagnostic_rendering_sites"]
    normalized_sites = sema["error_diagnostics_recovery_lowering_normalized_sites"]
    gate_blocked_sites = sema["error_diagnostics_recovery_lowering_gate_blocked_sites"]
    contract_violation_sites = sema["error_diagnostics_recovery_lowering_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_error_diagnostics_recovery_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        error_diagnostics_recovery_sites=error_diagnostics_recovery_sites,
        diagnostic_emission_sites=diagnostic_emission_sites,
        recovery_attempt_sites=recovery_attempt_sites,
        recovery_success_sites=recovery_success_sites,
        fixit_hint_sites=fixit_hint_sites,
        diagnostic_rendering_sites=diagnostic_rendering_sites,
        normalized_sites=normalized_sites,
        gate_blocked_sites=gate_blocked_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert diagnostic_emission_sites <= error_diagnostics_recovery_sites
    assert recovery_attempt_sites <= error_diagnostics_recovery_sites
    assert recovery_success_sites <= recovery_attempt_sites
    assert fixit_hint_sites <= diagnostic_emission_sites + contract_violation_sites
    assert diagnostic_rendering_sites <= diagnostic_emission_sites + contract_violation_sites
    assert normalized_sites <= error_diagnostics_recovery_sites
    assert gate_blocked_sites <= error_diagnostics_recovery_sites
    assert contract_violation_sites <= error_diagnostics_recovery_sites
    assert normalized_sites + gate_blocked_sites == error_diagnostics_recovery_sites

    assert deterministic_handoff is True
    assert sema["lowering_error_diagnostics_recovery_replay_key"] == expected_replay_key

    assert surface["error_diagnostics_recovery_sites"] == error_diagnostics_recovery_sites
    assert surface["diagnostic_emission_sites"] == diagnostic_emission_sites
    assert surface["recovery_attempt_sites"] == recovery_attempt_sites
    assert surface["recovery_success_sites"] == recovery_success_sites
    assert surface["fixit_hint_sites"] == fixit_hint_sites
    assert surface["diagnostic_rendering_sites"] == diagnostic_rendering_sites
    assert surface["normalized_sites"] == normalized_sites
    assert surface["gate_blocked_sites"] == gate_blocked_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m185_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M185 validation/conformance/perf error diagnostics UX and recovery runbook (M185-D001)",
        "python -m pytest tests/tooling/test_objc3c_m185_validation_error_diagnostics_recovery_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m185_conformance_error_diagnostics_recovery_contract.py -q",
        "lowering_error_diagnostics_recovery.replay_key",
        "deterministic_error_diagnostics_recovery_lowering_handoff",
        "error_diagnostics_recovery_lowering",
        "frontend_objc_error_diagnostics_recovery_lowering_profile",
        "!objc3.objc_error_diagnostics_recovery_lowering = !{!45}",
        "normalized_sites + gate_blocked_sites == error_diagnostics_recovery_sites",
    ):
        assert text in fragment


def test_m185_validation_manifest_packets_hold_deterministic_error_diagnostics_recovery_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m185_validation_ir_markers_match_manifest_error_diagnostics_recovery_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_error_diagnostics_recovery_lowering = !{!45}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_error_diagnostics_recovery"][
            "replay_key"
        ]


def test_m185_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_error_diagnostics_recovery"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_error_diagnostics_recovery"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
