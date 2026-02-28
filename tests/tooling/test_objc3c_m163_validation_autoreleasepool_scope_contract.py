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
    / "m163_validation_autoreleasepool_scope_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m163-autoreleasepool-scope-lowering-v1"
LOWERING_PREFIX = "; autoreleasepool_scope_lowering = "
PROFILE_PREFIX = "; frontend_objc_autoreleasepool_scope_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_autoreleasepool_scope_lowering_profile = "
    r"scope_sites=(\d+), scope_symbolized_sites=(\d+), max_scope_depth=(\d+), "
    r"scope_entry_transition_sites=(\d+), scope_exit_transition_sites=(\d+), "
    r"contract_violation_sites=(\d+), deterministic_autoreleasepool_scope_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!16 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    scope_sites: int,
    scope_symbolized_sites: int,
    max_scope_depth: int,
    scope_entry_transition_sites: int,
    scope_exit_transition_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"scope_sites={scope_sites};"
        f"scope_symbolized_sites={scope_symbolized_sites};"
        f"max_scope_depth={max_scope_depth};"
        f"scope_entry_transition_sites={scope_entry_transition_sites};"
        f"scope_exit_transition_sites={scope_exit_transition_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["autoreleasepool_scope_lowering_scope_sites"],
        sema["autoreleasepool_scope_lowering_scope_symbolized_sites"],
        sema["autoreleasepool_scope_lowering_max_scope_depth"],
        sema["autoreleasepool_scope_lowering_scope_entry_transition_sites"],
        sema["autoreleasepool_scope_lowering_scope_exit_transition_sites"],
        sema["autoreleasepool_scope_lowering_contract_violation_sites"],
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
    raise AssertionError("missing LLVM metadata tuple marker: !16 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_autoreleasepool_scope_lowering_surface"]
    lowering = manifest["lowering_autoreleasepool_scope"]

    scope_sites = sema["autoreleasepool_scope_lowering_scope_sites"]
    scope_symbolized_sites = sema["autoreleasepool_scope_lowering_scope_symbolized_sites"]
    max_scope_depth = sema["autoreleasepool_scope_lowering_max_scope_depth"]
    scope_entry_transition_sites = sema["autoreleasepool_scope_lowering_scope_entry_transition_sites"]
    scope_exit_transition_sites = sema["autoreleasepool_scope_lowering_scope_exit_transition_sites"]
    contract_violation_sites = sema["autoreleasepool_scope_lowering_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_autoreleasepool_scope_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        scope_sites=scope_sites,
        scope_symbolized_sites=scope_symbolized_sites,
        max_scope_depth=max_scope_depth,
        scope_entry_transition_sites=scope_entry_transition_sites,
        scope_exit_transition_sites=scope_exit_transition_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert scope_symbolized_sites <= scope_sites
    assert contract_violation_sites <= scope_sites
    assert scope_entry_transition_sites == scope_sites
    assert scope_exit_transition_sites == scope_sites
    assert (scope_sites > 0) or (max_scope_depth == 0)
    assert max_scope_depth <= scope_sites

    assert deterministic_handoff is True
    assert sema["lowering_autoreleasepool_scope_replay_key"] == expected_replay_key

    assert surface["scope_sites"] == scope_sites
    assert surface["scope_symbolized_sites"] == scope_symbolized_sites
    assert surface["max_scope_depth"] == max_scope_depth
    assert surface["scope_entry_transition_sites"] == scope_entry_transition_sites
    assert surface["scope_exit_transition_sites"] == scope_exit_transition_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m163_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M163 validation/conformance/perf autoreleasepool scope runbook",
        "python -m pytest tests/tooling/test_objc3c_m163_frontend_autorelease_pool_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m163_sema_autorelease_pool_scope_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m163_lowering_autoreleasepool_scope_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m163_validation_autoreleasepool_scope_contract.py -q",
        "lowering_autoreleasepool_scope.replay_key",
        "deterministic_autoreleasepool_scope_lowering_handoff",
        "autoreleasepool_scope_lowering",
        "frontend_objc_autoreleasepool_scope_lowering_profile",
        "!objc3.objc_autoreleasepool_scope_lowering = !{!16}",
    ):
        assert text in fragment


def test_m163_validation_manifest_packets_hold_deterministic_autoreleasepool_scope_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m163_validation_ir_markers_match_manifest_autoreleasepool_scope_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_autoreleasepool_scope_lowering = !{!16}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_autoreleasepool_scope"]["replay_key"]


def test_m163_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_autoreleasepool_scope"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_autoreleasepool_scope"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
