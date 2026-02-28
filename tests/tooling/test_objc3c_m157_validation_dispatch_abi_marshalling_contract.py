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
    / "m157_validation_dispatch_abi_marshalling_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m157-dispatch-abi-marshalling-v1"
LOWERING_PREFIX = "; dispatch_abi_marshalling_lowering = "
PROFILE_PREFIX = "; frontend_objc_dispatch_abi_marshalling_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_dispatch_abi_marshalling_profile = "
    r"message_send_sites=(\d+), receiver_slots_marshaled=(\d+), selector_slots_marshaled=(\d+), "
    r"argument_value_slots_marshaled=(\d+), argument_padding_slots_marshaled=(\d+), "
    r"argument_total_slots_marshaled=(\d+), total_marshaled_slots=(\d+), runtime_dispatch_arg_slots=(\d+), "
    r"deterministic_dispatch_abi_marshalling_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!10 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    message_send_sites: int,
    receiver_slots_marshaled: int,
    selector_slots_marshaled: int,
    argument_value_slots_marshaled: int,
    argument_padding_slots_marshaled: int,
    argument_total_slots_marshaled: int,
    total_marshaled_slots: int,
    runtime_dispatch_arg_slots: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"message_send_sites={message_send_sites};"
        f"receiver_slots_marshaled={receiver_slots_marshaled};"
        f"selector_slots_marshaled={selector_slots_marshaled};"
        f"argument_value_slots_marshaled={argument_value_slots_marshaled};"
        f"argument_padding_slots_marshaled={argument_padding_slots_marshaled};"
        f"argument_total_slots_marshaled={argument_total_slots_marshaled};"
        f"total_marshaled_slots={total_marshaled_slots};"
        f"runtime_dispatch_arg_slots={runtime_dispatch_arg_slots};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["dispatch_abi_marshalling_message_send_sites"],
        sema["dispatch_abi_marshalling_receiver_slots_marshaled"],
        sema["dispatch_abi_marshalling_selector_slots_marshaled"],
        sema["dispatch_abi_marshalling_argument_value_slots_marshaled"],
        sema["dispatch_abi_marshalling_argument_padding_slots_marshaled"],
        sema["dispatch_abi_marshalling_argument_total_slots_marshaled"],
        sema["dispatch_abi_marshalling_total_marshaled_slots"],
        sema["dispatch_abi_marshalling_runtime_dispatch_arg_slots"],
    )


def _extract_profile_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, ...], bool]:
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


def _extract_metadata_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, ...], bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:8])
            return counts, match.group(9) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !10 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_dispatch_abi_marshalling_surface"]
    lowering = manifest["lowering_dispatch_abi_marshalling"]

    message_send_sites = sema["dispatch_abi_marshalling_message_send_sites"]
    receiver_slots_marshaled = sema["dispatch_abi_marshalling_receiver_slots_marshaled"]
    selector_slots_marshaled = sema["dispatch_abi_marshalling_selector_slots_marshaled"]
    argument_value_slots_marshaled = sema["dispatch_abi_marshalling_argument_value_slots_marshaled"]
    argument_padding_slots_marshaled = sema["dispatch_abi_marshalling_argument_padding_slots_marshaled"]
    argument_total_slots_marshaled = sema["dispatch_abi_marshalling_argument_total_slots_marshaled"]
    total_marshaled_slots = sema["dispatch_abi_marshalling_total_marshaled_slots"]
    runtime_dispatch_arg_slots = sema["dispatch_abi_marshalling_runtime_dispatch_arg_slots"]
    deterministic_handoff = sema["deterministic_dispatch_abi_marshalling_handoff"]
    expected_replay_key = _expected_replay_key(
        message_send_sites=message_send_sites,
        receiver_slots_marshaled=receiver_slots_marshaled,
        selector_slots_marshaled=selector_slots_marshaled,
        argument_value_slots_marshaled=argument_value_slots_marshaled,
        argument_padding_slots_marshaled=argument_padding_slots_marshaled,
        argument_total_slots_marshaled=argument_total_slots_marshaled,
        total_marshaled_slots=total_marshaled_slots,
        runtime_dispatch_arg_slots=runtime_dispatch_arg_slots,
        deterministic=deterministic_handoff,
    )

    expected_argument_total = message_send_sites * runtime_dispatch_arg_slots
    expected_total_marshaled_slots = (
        receiver_slots_marshaled + selector_slots_marshaled + argument_total_slots_marshaled
    )

    assert receiver_slots_marshaled == message_send_sites
    assert selector_slots_marshaled == message_send_sites
    assert argument_total_slots_marshaled == expected_argument_total
    assert argument_value_slots_marshaled <= argument_total_slots_marshaled
    assert argument_padding_slots_marshaled + argument_value_slots_marshaled == argument_total_slots_marshaled
    assert total_marshaled_slots == expected_total_marshaled_slots

    assert deterministic_handoff is True
    assert sema["lowering_dispatch_abi_marshalling_replay_key"] == expected_replay_key

    assert surface["message_send_sites"] == message_send_sites
    assert surface["receiver_slots_marshaled"] == receiver_slots_marshaled
    assert surface["selector_slots_marshaled"] == selector_slots_marshaled
    assert surface["argument_value_slots_marshaled"] == argument_value_slots_marshaled
    assert surface["argument_padding_slots_marshaled"] == argument_padding_slots_marshaled
    assert surface["argument_total_slots_marshaled"] == argument_total_slots_marshaled
    assert surface["total_marshaled_slots"] == total_marshaled_slots
    assert surface["runtime_dispatch_arg_slots"] == runtime_dispatch_arg_slots
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m157_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M157 validation/conformance/perf dispatch ABI marshalling runbook",
        "python -m pytest tests/tooling/test_objc3c_m157_frontend_dispatch_abi_marshalling_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m157_lowering_dispatch_abi_marshalling_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m157_validation_dispatch_abi_marshalling_contract.py -q",
        "lowering_dispatch_abi_marshalling.replay_key",
        "deterministic_dispatch_abi_marshalling_handoff",
        "dispatch_abi_marshalling_lowering",
        "frontend_objc_dispatch_abi_marshalling_profile",
        "!objc3.objc_dispatch_abi_marshalling = !{!10}",
    ):
        assert text in fragment


def test_m157_validation_manifest_packets_hold_deterministic_dispatch_abi_marshalling_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m157_validation_ir_markers_match_manifest_dispatch_abi_marshalling_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_dispatch_abi_marshalling = !{!10}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)
        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_dispatch_abi_marshalling"]["replay_key"]


def test_m157_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_dispatch_abi_marshalling"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_dispatch_abi_marshalling"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
