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
    / "m159_validation_super_dispatch_method_family_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m159-super-dispatch-method-family-v1"
LOWERING_PREFIX = "; super_dispatch_method_family_lowering = "
PROFILE_PREFIX = "; frontend_objc_super_dispatch_method_family_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_super_dispatch_method_family_profile = "
    r"message_send_sites=(\d+), receiver_super_identifier_sites=(\d+), super_dispatch_enabled_sites=(\d+), "
    r"super_dispatch_requires_class_context_sites=(\d+), method_family_init_sites=(\d+), "
    r"method_family_copy_sites=(\d+), method_family_mutable_copy_sites=(\d+), method_family_new_sites=(\d+), "
    r"method_family_none_sites=(\d+), method_family_returns_retained_result_sites=(\d+), "
    r"method_family_returns_related_result_sites=(\d+), contract_violation_sites=(\d+), "
    r"deterministic_super_dispatch_method_family_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!12 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), "
    r"i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    message_send_sites: int,
    receiver_super_identifier_sites: int,
    super_dispatch_enabled_sites: int,
    super_dispatch_requires_class_context_sites: int,
    method_family_init_sites: int,
    method_family_copy_sites: int,
    method_family_mutable_copy_sites: int,
    method_family_new_sites: int,
    method_family_none_sites: int,
    method_family_returns_retained_result_sites: int,
    method_family_returns_related_result_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"message_send_sites={message_send_sites};"
        f"receiver_super_identifier_sites={receiver_super_identifier_sites};"
        f"super_dispatch_enabled_sites={super_dispatch_enabled_sites};"
        f"super_dispatch_requires_class_context_sites={super_dispatch_requires_class_context_sites};"
        f"method_family_init_sites={method_family_init_sites};"
        f"method_family_copy_sites={method_family_copy_sites};"
        f"method_family_mutable_copy_sites={method_family_mutable_copy_sites};"
        f"method_family_new_sites={method_family_new_sites};"
        f"method_family_none_sites={method_family_none_sites};"
        f"method_family_returns_retained_result_sites={method_family_returns_retained_result_sites};"
        f"method_family_returns_related_result_sites={method_family_returns_related_result_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, ...]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["super_dispatch_method_family_message_send_sites"],
        sema["super_dispatch_method_family_receiver_super_identifier_sites"],
        sema["super_dispatch_method_family_enabled_sites"],
        sema["super_dispatch_method_family_requires_class_context_sites"],
        sema["super_dispatch_method_family_init_sites"],
        sema["super_dispatch_method_family_copy_sites"],
        sema["super_dispatch_method_family_mutable_copy_sites"],
        sema["super_dispatch_method_family_new_sites"],
        sema["super_dispatch_method_family_none_sites"],
        sema["super_dispatch_method_family_returns_retained_result_sites"],
        sema["super_dispatch_method_family_returns_related_result_sites"],
        sema["super_dispatch_method_family_contract_violation_sites"],
    )


def _extract_profile_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, ...], bool]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:12])
            return counts, match.group(13) == "true"
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
            counts = tuple(int(group) for group in match.groups()[:12])
            return counts, match.group(13) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !12 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_super_dispatch_method_family_surface"]
    lowering = manifest["lowering_super_dispatch_method_family"]

    message_send_sites = sema["super_dispatch_method_family_message_send_sites"]
    receiver_super_identifier_sites = sema["super_dispatch_method_family_receiver_super_identifier_sites"]
    super_dispatch_enabled_sites = sema["super_dispatch_method_family_enabled_sites"]
    super_dispatch_requires_class_context_sites = sema[
        "super_dispatch_method_family_requires_class_context_sites"
    ]
    method_family_init_sites = sema["super_dispatch_method_family_init_sites"]
    method_family_copy_sites = sema["super_dispatch_method_family_copy_sites"]
    method_family_mutable_copy_sites = sema["super_dispatch_method_family_mutable_copy_sites"]
    method_family_new_sites = sema["super_dispatch_method_family_new_sites"]
    method_family_none_sites = sema["super_dispatch_method_family_none_sites"]
    method_family_returns_retained_result_sites = sema[
        "super_dispatch_method_family_returns_retained_result_sites"
    ]
    method_family_returns_related_result_sites = sema[
        "super_dispatch_method_family_returns_related_result_sites"
    ]
    contract_violation_sites = sema["super_dispatch_method_family_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_super_dispatch_method_family_handoff"]
    expected_replay_key = _expected_replay_key(
        message_send_sites=message_send_sites,
        receiver_super_identifier_sites=receiver_super_identifier_sites,
        super_dispatch_enabled_sites=super_dispatch_enabled_sites,
        super_dispatch_requires_class_context_sites=super_dispatch_requires_class_context_sites,
        method_family_init_sites=method_family_init_sites,
        method_family_copy_sites=method_family_copy_sites,
        method_family_mutable_copy_sites=method_family_mutable_copy_sites,
        method_family_new_sites=method_family_new_sites,
        method_family_none_sites=method_family_none_sites,
        method_family_returns_retained_result_sites=method_family_returns_retained_result_sites,
        method_family_returns_related_result_sites=method_family_returns_related_result_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert receiver_super_identifier_sites == super_dispatch_enabled_sites
    assert super_dispatch_requires_class_context_sites == super_dispatch_enabled_sites
    assert (
        method_family_init_sites
        + method_family_copy_sites
        + method_family_mutable_copy_sites
        + method_family_new_sites
        + method_family_none_sites
        == message_send_sites
    )
    assert method_family_returns_related_result_sites <= method_family_init_sites
    assert method_family_returns_retained_result_sites <= message_send_sites
    assert contract_violation_sites <= message_send_sites

    assert deterministic_handoff is True
    assert sema["lowering_super_dispatch_method_family_replay_key"] == expected_replay_key

    assert surface["message_send_sites"] == message_send_sites
    assert surface["receiver_super_identifier_sites"] == receiver_super_identifier_sites
    assert surface["super_dispatch_enabled_sites"] == super_dispatch_enabled_sites
    assert (
        surface["super_dispatch_requires_class_context_sites"]
        == super_dispatch_requires_class_context_sites
    )
    assert surface["method_family_init_sites"] == method_family_init_sites
    assert surface["method_family_copy_sites"] == method_family_copy_sites
    assert surface["method_family_mutable_copy_sites"] == method_family_mutable_copy_sites
    assert surface["method_family_new_sites"] == method_family_new_sites
    assert surface["method_family_none_sites"] == method_family_none_sites
    assert (
        surface["method_family_returns_retained_result_sites"]
        == method_family_returns_retained_result_sites
    )
    assert (
        surface["method_family_returns_related_result_sites"]
        == method_family_returns_related_result_sites
    )
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m159_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M159 validation/conformance/perf super-dispatch and method-family runbook",
        "python -m pytest tests/tooling/test_objc3c_m159_frontend_super_dispatch_method_family_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m159_lowering_super_dispatch_method_family_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m159_validation_super_dispatch_method_family_contract.py -q",
        "lowering_super_dispatch_method_family.replay_key",
        "deterministic_super_dispatch_method_family_handoff",
        "super_dispatch_method_family_lowering",
        "frontend_objc_super_dispatch_method_family_profile",
        "!objc3.objc_super_dispatch_method_family = !{!12}",
    ):
        assert text in fragment


def test_m159_validation_manifest_packets_hold_deterministic_super_dispatch_method_family_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m159_validation_ir_markers_match_manifest_super_dispatch_method_family_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_super_dispatch_method_family = !{!12}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)
        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_super_dispatch_method_family"][
            "replay_key"
        ]


def test_m159_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_super_dispatch_method_family"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_super_dispatch_method_family"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
