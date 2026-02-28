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
    / "m156_validation_message_send_selector_lowering_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m156-message-send-selector-lowering-v1"
LOWERING_PREFIX = "; message_send_selector_lowering = "
PROFILE_PREFIX = "; frontend_objc_message_send_selector_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_message_send_selector_lowering_profile = "
    r"message_send_sites=(\d+), unary_selector_sites=(\d+), keyword_selector_sites=(\d+), "
    r"selector_piece_sites=(\d+), argument_expression_sites=(\d+), receiver_expression_sites=(\d+), "
    r"selector_literal_entries=(\d+), selector_literal_characters=(\d+), "
    r"deterministic_message_send_selector_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!9 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    message_send_sites: int,
    unary_selector_sites: int,
    keyword_selector_sites: int,
    selector_piece_sites: int,
    argument_expression_sites: int,
    receiver_expression_sites: int,
    selector_literal_entries: int,
    selector_literal_characters: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"message_send_sites={message_send_sites};"
        f"unary_selector_sites={unary_selector_sites};"
        f"keyword_selector_sites={keyword_selector_sites};"
        f"selector_piece_sites={selector_piece_sites};"
        f"argument_expression_sites={argument_expression_sites};"
        f"receiver_expression_sites={receiver_expression_sites};"
        f"selector_literal_entries={selector_literal_entries};"
        f"selector_literal_characters={selector_literal_characters};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["message_send_selector_lowering_sites"],
        sema["message_send_selector_lowering_unary_sites"],
        sema["message_send_selector_lowering_keyword_sites"],
        sema["message_send_selector_lowering_selector_piece_sites"],
        sema["message_send_selector_lowering_argument_expression_sites"],
        sema["message_send_selector_lowering_receiver_sites"],
        sema["message_send_selector_lowering_selector_literal_entries"],
        sema["message_send_selector_lowering_selector_literal_characters"],
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
    raise AssertionError("missing LLVM metadata tuple marker: !9 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_message_send_selector_lowering_surface"]
    lowering = manifest["lowering_message_send_selector_lowering"]

    message_send_sites = sema["message_send_selector_lowering_sites"]
    unary_selector_sites = sema["message_send_selector_lowering_unary_sites"]
    keyword_selector_sites = sema["message_send_selector_lowering_keyword_sites"]
    selector_piece_sites = sema["message_send_selector_lowering_selector_piece_sites"]
    argument_expression_sites = sema["message_send_selector_lowering_argument_expression_sites"]
    receiver_expression_sites = sema["message_send_selector_lowering_receiver_sites"]
    selector_literal_entries = sema["message_send_selector_lowering_selector_literal_entries"]
    selector_literal_characters = sema["message_send_selector_lowering_selector_literal_characters"]
    deterministic_handoff = sema["deterministic_message_send_selector_lowering_handoff"]
    expected_replay_key = _expected_replay_key(
        message_send_sites=message_send_sites,
        unary_selector_sites=unary_selector_sites,
        keyword_selector_sites=keyword_selector_sites,
        selector_piece_sites=selector_piece_sites,
        argument_expression_sites=argument_expression_sites,
        receiver_expression_sites=receiver_expression_sites,
        selector_literal_entries=selector_literal_entries,
        selector_literal_characters=selector_literal_characters,
        deterministic=deterministic_handoff,
    )

    assert unary_selector_sites + keyword_selector_sites == message_send_sites
    assert receiver_expression_sites == message_send_sites
    assert selector_piece_sites >= message_send_sites
    assert argument_expression_sites >= keyword_selector_sites
    assert selector_literal_entries <= message_send_sites
    if selector_literal_entries == 0:
        assert selector_literal_characters == 0

    assert deterministic_handoff is True
    assert sema["lowering_message_send_selector_lowering_replay_key"] == expected_replay_key

    assert surface["message_send_sites"] == message_send_sites
    assert surface["unary_selector_sites"] == unary_selector_sites
    assert surface["keyword_selector_sites"] == keyword_selector_sites
    assert surface["selector_piece_sites"] == selector_piece_sites
    assert surface["argument_expression_sites"] == argument_expression_sites
    assert surface["receiver_expression_sites"] == receiver_expression_sites
    assert surface["selector_literal_entries"] == selector_literal_entries
    assert surface["selector_literal_characters"] == selector_literal_characters
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m156_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M156 validation/conformance/perf message-send selector lowering runbook",
        "python -m pytest tests/tooling/test_objc3c_m156_sema_message_send_selector_lowering_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m156_lowering_message_send_selector_lowering_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m156_validation_message_send_selector_lowering_contract.py -q",
        "lowering_message_send_selector_lowering.replay_key",
        "deterministic_message_send_selector_lowering_handoff",
        "message_send_selector_lowering",
        "frontend_objc_message_send_selector_lowering_profile",
        "!objc3.objc_message_send_selector_lowering = !{!9}",
    ):
        assert text in fragment


def test_m156_validation_manifest_packets_hold_deterministic_selector_lowering_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m156_validation_ir_markers_match_manifest_selector_lowering_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_message_send_selector_lowering = !{!9}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)
        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_message_send_selector_lowering"][
            "replay_key"
        ]


def test_m156_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_message_send_selector_lowering"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_message_send_selector_lowering"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
