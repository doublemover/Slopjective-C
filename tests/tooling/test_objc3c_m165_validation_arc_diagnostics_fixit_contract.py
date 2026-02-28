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
    / "m165_validation_arc_diagnostics_fixit_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m165-arc-diagnostics-fixit-lowering-v1"
LOWERING_PREFIX = "; arc_diagnostics_fixit_lowering = "
PROFILE_PREFIX = "; frontend_objc_arc_diagnostics_fixit_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_arc_diagnostics_fixit_lowering_profile = "
    r"ownership_arc_diagnostic_candidate_sites=(\d+), ownership_arc_fixit_available_sites=(\d+), "
    r"ownership_arc_profiled_sites=(\d+), ownership_arc_weak_unowned_conflict_diagnostic_sites=(\d+), "
    r"ownership_arc_empty_fixit_hint_sites=(\d+), contract_violation_sites=(\d+), "
    r"deterministic_arc_diagnostics_fixit_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!18 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    ownership_arc_diagnostic_candidate_sites: int,
    ownership_arc_fixit_available_sites: int,
    ownership_arc_profiled_sites: int,
    ownership_arc_weak_unowned_conflict_diagnostic_sites: int,
    ownership_arc_empty_fixit_hint_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"ownership_arc_diagnostic_candidate_sites={ownership_arc_diagnostic_candidate_sites};"
        f"ownership_arc_fixit_available_sites={ownership_arc_fixit_available_sites};"
        f"ownership_arc_profiled_sites={ownership_arc_profiled_sites};"
        f"ownership_arc_weak_unowned_conflict_diagnostic_sites={ownership_arc_weak_unowned_conflict_diagnostic_sites};"
        f"ownership_arc_empty_fixit_hint_sites={ownership_arc_empty_fixit_hint_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites"],
        sema["arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites"],
        sema["arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites"],
        sema["arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites"],
        sema["arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites"],
        sema["arc_diagnostics_fixit_lowering_contract_violation_sites"],
    )


def _extract_profile_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:6])
            return counts, match.group(7) == "true"
    raise AssertionError(f"missing IR profile marker: {PROFILE_PREFIX}")


def _extract_lowering_replay_key(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(LOWERING_PREFIX):
            return line.removeprefix(LOWERING_PREFIX)
    raise AssertionError(f"missing IR replay-key marker: {LOWERING_PREFIX}")


def _extract_metadata_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:6])
            return counts, match.group(7) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !18 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_arc_diagnostics_fixit_lowering_surface"]
    lowering = manifest["lowering_arc_diagnostics_fixit"]

    ownership_arc_diagnostic_candidate_sites = sema[
        "arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites"
    ]
    ownership_arc_fixit_available_sites = sema[
        "arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites"
    ]
    ownership_arc_profiled_sites = sema["arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites"]
    ownership_arc_weak_unowned_conflict_diagnostic_sites = sema[
        "arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites"
    ]
    ownership_arc_empty_fixit_hint_sites = sema[
        "arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites"
    ]
    contract_violation_sites = sema["arc_diagnostics_fixit_lowering_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_arc_diagnostics_fixit_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        ownership_arc_diagnostic_candidate_sites=ownership_arc_diagnostic_candidate_sites,
        ownership_arc_fixit_available_sites=ownership_arc_fixit_available_sites,
        ownership_arc_profiled_sites=ownership_arc_profiled_sites,
        ownership_arc_weak_unowned_conflict_diagnostic_sites=ownership_arc_weak_unowned_conflict_diagnostic_sites,
        ownership_arc_empty_fixit_hint_sites=ownership_arc_empty_fixit_hint_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert ownership_arc_fixit_available_sites <= ownership_arc_diagnostic_candidate_sites + contract_violation_sites
    assert ownership_arc_profiled_sites <= ownership_arc_diagnostic_candidate_sites + contract_violation_sites
    assert (
        ownership_arc_weak_unowned_conflict_diagnostic_sites
        <= ownership_arc_diagnostic_candidate_sites + contract_violation_sites
    )
    assert ownership_arc_empty_fixit_hint_sites <= ownership_arc_fixit_available_sites + contract_violation_sites

    assert deterministic_handoff is True
    assert sema["lowering_arc_diagnostics_fixit_replay_key"] == expected_replay_key

    assert surface["ownership_arc_diagnostic_candidate_sites"] == ownership_arc_diagnostic_candidate_sites
    assert surface["ownership_arc_fixit_available_sites"] == ownership_arc_fixit_available_sites
    assert surface["ownership_arc_profiled_sites"] == ownership_arc_profiled_sites
    assert (
        surface["ownership_arc_weak_unowned_conflict_diagnostic_sites"]
        == ownership_arc_weak_unowned_conflict_diagnostic_sites
    )
    assert surface["ownership_arc_empty_fixit_hint_sites"] == ownership_arc_empty_fixit_hint_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m165_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M165 validation/conformance/perf ARC diagnostics/fix-it runbook",
        "python -m pytest tests/tooling/test_objc3c_m165_frontend_arc_diagnostics_fixit_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m165_sema_arc_diagnostics_fixit_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m165_lowering_arc_diagnostics_fixit_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m165_validation_arc_diagnostics_fixit_contract.py -q",
        "lowering_arc_diagnostics_fixit.replay_key",
        "deterministic_arc_diagnostics_fixit_lowering_handoff",
        "arc_diagnostics_fixit_lowering",
        "frontend_objc_arc_diagnostics_fixit_lowering_profile",
        "!objc3.objc_arc_diagnostics_fixit_lowering = !{!18}",
    ):
        assert text in fragment


def test_m165_validation_manifest_packets_hold_deterministic_arc_diagnostics_fixit_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m165_validation_ir_markers_match_manifest_arc_diagnostics_fixit_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_arc_diagnostics_fixit_lowering = !{!18}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_arc_diagnostics_fixit"]["replay_key"]


def test_m165_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_arc_diagnostics_fixit"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_arc_diagnostics_fixit"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
