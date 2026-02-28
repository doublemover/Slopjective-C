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
    / "m164_validation_weak_unowned_semantics_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m164-weak-unowned-semantics-lowering-v1"
LOWERING_PREFIX = "; weak_unowned_semantics_lowering = "
PROFILE_PREFIX = "; frontend_objc_weak_unowned_semantics_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_weak_unowned_semantics_lowering_profile = "
    r"ownership_candidate_sites=(\d+), weak_reference_sites=(\d+), unowned_reference_sites=(\d+), "
    r"unowned_safe_reference_sites=(\d+), weak_unowned_conflict_sites=(\d+), contract_violation_sites=(\d+), "
    r"deterministic_weak_unowned_semantics_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!17 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    ownership_candidate_sites: int,
    weak_reference_sites: int,
    unowned_reference_sites: int,
    unowned_safe_reference_sites: int,
    weak_unowned_conflict_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"ownership_candidate_sites={ownership_candidate_sites};"
        f"weak_reference_sites={weak_reference_sites};"
        f"unowned_reference_sites={unowned_reference_sites};"
        f"unowned_safe_reference_sites={unowned_safe_reference_sites};"
        f"weak_unowned_conflict_sites={weak_unowned_conflict_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["weak_unowned_semantics_lowering_ownership_candidate_sites"],
        sema["weak_unowned_semantics_lowering_weak_reference_sites"],
        sema["weak_unowned_semantics_lowering_unowned_reference_sites"],
        sema["weak_unowned_semantics_lowering_unowned_safe_reference_sites"],
        sema["weak_unowned_semantics_lowering_conflict_sites"],
        sema["weak_unowned_semantics_lowering_contract_violation_sites"],
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
    raise AssertionError("missing LLVM metadata tuple marker: !17 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_weak_unowned_semantics_lowering_surface"]
    lowering = manifest["lowering_weak_unowned_semantics"]

    ownership_candidate_sites = sema["weak_unowned_semantics_lowering_ownership_candidate_sites"]
    weak_reference_sites = sema["weak_unowned_semantics_lowering_weak_reference_sites"]
    unowned_reference_sites = sema["weak_unowned_semantics_lowering_unowned_reference_sites"]
    unowned_safe_reference_sites = sema["weak_unowned_semantics_lowering_unowned_safe_reference_sites"]
    weak_unowned_conflict_sites = sema["weak_unowned_semantics_lowering_conflict_sites"]
    contract_violation_sites = sema["weak_unowned_semantics_lowering_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_weak_unowned_semantics_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        ownership_candidate_sites=ownership_candidate_sites,
        weak_reference_sites=weak_reference_sites,
        unowned_reference_sites=unowned_reference_sites,
        unowned_safe_reference_sites=unowned_safe_reference_sites,
        weak_unowned_conflict_sites=weak_unowned_conflict_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert weak_reference_sites <= ownership_candidate_sites
    assert unowned_reference_sites <= ownership_candidate_sites
    assert unowned_safe_reference_sites <= unowned_reference_sites
    assert weak_unowned_conflict_sites <= ownership_candidate_sites
    assert contract_violation_sites <= ownership_candidate_sites + weak_unowned_conflict_sites

    assert deterministic_handoff is True
    assert sema["lowering_weak_unowned_semantics_replay_key"] == expected_replay_key

    assert surface["ownership_candidate_sites"] == ownership_candidate_sites
    assert surface["weak_reference_sites"] == weak_reference_sites
    assert surface["unowned_reference_sites"] == unowned_reference_sites
    assert surface["unowned_safe_reference_sites"] == unowned_safe_reference_sites
    assert surface["weak_unowned_conflict_sites"] == weak_unowned_conflict_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m164_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M164 validation/conformance/perf weak/unowned semantics runbook",
        "python -m pytest tests/tooling/test_objc3c_m164_frontend_weak_unowned_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m164_sema_weak_unowned_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m164_lowering_weak_unowned_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m164_validation_weak_unowned_semantics_contract.py -q",
        "lowering_weak_unowned_semantics.replay_key",
        "deterministic_weak_unowned_semantics_lowering_handoff",
        "weak_unowned_semantics_lowering",
        "frontend_objc_weak_unowned_semantics_lowering_profile",
        "!objc3.objc_weak_unowned_semantics_lowering = !{!17}",
    ):
        assert text in fragment


def test_m164_validation_manifest_packets_hold_deterministic_weak_unowned_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m164_validation_ir_markers_match_manifest_weak_unowned_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_weak_unowned_semantics_lowering = !{!17}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_weak_unowned_semantics"]["replay_key"]


def test_m164_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_weak_unowned_semantics"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_weak_unowned_semantics"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
