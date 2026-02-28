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
    / "m158_validation_nil_receiver_semantics_foldability_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m158-nil-receiver-semantics-foldability-v1"
LOWERING_PREFIX = "; nil_receiver_semantics_foldability_lowering = "
PROFILE_PREFIX = "; frontend_objc_nil_receiver_semantics_foldability_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_nil_receiver_semantics_foldability_profile = "
    r"message_send_sites=(\d+), receiver_nil_literal_sites=(\d+), nil_receiver_semantics_enabled_sites=(\d+), "
    r"nil_receiver_foldable_sites=(\d+), nil_receiver_runtime_dispatch_required_sites=(\d+), "
    r"non_nil_receiver_sites=(\d+), contract_violation_sites=(\d+), "
    r"deterministic_nil_receiver_semantics_foldability_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!11 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    message_send_sites: int,
    receiver_nil_literal_sites: int,
    nil_receiver_semantics_enabled_sites: int,
    nil_receiver_foldable_sites: int,
    nil_receiver_runtime_dispatch_required_sites: int,
    non_nil_receiver_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"message_send_sites={message_send_sites};"
        f"receiver_nil_literal_sites={receiver_nil_literal_sites};"
        f"nil_receiver_semantics_enabled_sites={nil_receiver_semantics_enabled_sites};"
        f"nil_receiver_foldable_sites={nil_receiver_foldable_sites};"
        f"nil_receiver_runtime_dispatch_required_sites={nil_receiver_runtime_dispatch_required_sites};"
        f"non_nil_receiver_sites={non_nil_receiver_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["nil_receiver_semantics_foldability_message_send_sites"],
        sema["nil_receiver_semantics_foldability_receiver_nil_literal_sites"],
        sema["nil_receiver_semantics_foldability_enabled_sites"],
        sema["nil_receiver_semantics_foldability_foldable_sites"],
        sema["nil_receiver_semantics_foldability_runtime_dispatch_required_sites"],
        sema["nil_receiver_semantics_foldability_non_nil_receiver_sites"],
        sema["nil_receiver_semantics_foldability_contract_violation_sites"],
    )


def _extract_profile_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, ...], bool]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:7])
            return counts, match.group(8) == "true"
    raise AssertionError(f"missing IR profile marker: {PROFILE_PREFIX}")


def _extract_lowering_replay_key(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(LOWERING_PREFIX):
            return line.removeprefix(LOWERING_PREFIX)
    raise AssertionError(f"missing IR replay-key marker: {LOWERING_PREFIX}")


def _extract_metadata_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, ...], bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:7])
            return counts, match.group(8) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !11 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_nil_receiver_semantics_foldability_surface"]
    lowering = manifest["lowering_nil_receiver_semantics_foldability"]

    message_send_sites = sema["nil_receiver_semantics_foldability_message_send_sites"]
    receiver_nil_literal_sites = sema["nil_receiver_semantics_foldability_receiver_nil_literal_sites"]
    nil_receiver_semantics_enabled_sites = sema["nil_receiver_semantics_foldability_enabled_sites"]
    nil_receiver_foldable_sites = sema["nil_receiver_semantics_foldability_foldable_sites"]
    nil_receiver_runtime_dispatch_required_sites = sema[
        "nil_receiver_semantics_foldability_runtime_dispatch_required_sites"
    ]
    non_nil_receiver_sites = sema["nil_receiver_semantics_foldability_non_nil_receiver_sites"]
    contract_violation_sites = sema["nil_receiver_semantics_foldability_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_nil_receiver_semantics_foldability_handoff"]
    expected_replay_key = _expected_replay_key(
        message_send_sites=message_send_sites,
        receiver_nil_literal_sites=receiver_nil_literal_sites,
        nil_receiver_semantics_enabled_sites=nil_receiver_semantics_enabled_sites,
        nil_receiver_foldable_sites=nil_receiver_foldable_sites,
        nil_receiver_runtime_dispatch_required_sites=nil_receiver_runtime_dispatch_required_sites,
        non_nil_receiver_sites=non_nil_receiver_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert receiver_nil_literal_sites == nil_receiver_semantics_enabled_sites
    assert nil_receiver_foldable_sites <= nil_receiver_semantics_enabled_sites
    assert nil_receiver_runtime_dispatch_required_sites + nil_receiver_foldable_sites == message_send_sites
    assert nil_receiver_semantics_enabled_sites + non_nil_receiver_sites == message_send_sites
    assert contract_violation_sites <= message_send_sites

    assert deterministic_handoff is True
    assert sema["lowering_nil_receiver_semantics_foldability_replay_key"] == expected_replay_key

    assert surface["message_send_sites"] == message_send_sites
    assert surface["receiver_nil_literal_sites"] == receiver_nil_literal_sites
    assert surface["nil_receiver_semantics_enabled_sites"] == nil_receiver_semantics_enabled_sites
    assert surface["nil_receiver_foldable_sites"] == nil_receiver_foldable_sites
    assert surface["nil_receiver_runtime_dispatch_required_sites"] == nil_receiver_runtime_dispatch_required_sites
    assert surface["non_nil_receiver_sites"] == non_nil_receiver_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m158_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M158 validation/conformance/perf nil-receiver semantics/foldability runbook",
        "python -m pytest tests/tooling/test_objc3c_m158_frontend_nil_receiver_semantics_foldability_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m158_lowering_nil_receiver_semantics_foldability_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m158_validation_nil_receiver_semantics_foldability_contract.py -q",
        "lowering_nil_receiver_semantics_foldability.replay_key",
        "deterministic_nil_receiver_semantics_foldability_handoff",
        "nil_receiver_semantics_foldability_lowering",
        "frontend_objc_nil_receiver_semantics_foldability_profile",
        "!objc3.objc_nil_receiver_semantics_foldability = !{!11}",
    ):
        assert text in fragment


def test_m158_validation_manifest_packets_hold_deterministic_nil_receiver_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m158_validation_ir_markers_match_manifest_nil_receiver_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_nil_receiver_semantics_foldability = !{!11}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)
        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_nil_receiver_semantics_foldability"][
            "replay_key"
        ]


def test_m158_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_nil_receiver_semantics_foldability"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_nil_receiver_semantics_foldability"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
