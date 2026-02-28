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
    / "m167_validation_block_abi_invoke_trampoline_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m167-block-abi-invoke-trampoline-lowering-v1"
LOWERING_PREFIX = "; block_abi_invoke_trampoline_lowering = "
PROFILE_PREFIX = "; frontend_objc_block_abi_invoke_trampoline_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_block_abi_invoke_trampoline_lowering_profile = "
    r"block_literal_sites=(\d+), invoke_argument_slots_total=(\d+), capture_word_count_total=(\d+), "
    r"parameter_entries_total=(\d+), capture_entries_total=(\d+), body_statement_entries_total=(\d+), "
    r"descriptor_symbolized_sites=(\d+), invoke_trampoline_symbolized_sites=(\d+), "
    r"missing_invoke_trampoline_sites=(\d+), non_normalized_layout_sites=(\d+), "
    r"contract_violation_sites=(\d+), deterministic_block_abi_invoke_trampoline_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!20 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    block_literal_sites: int,
    invoke_argument_slots_total: int,
    capture_word_count_total: int,
    parameter_entries_total: int,
    capture_entries_total: int,
    body_statement_entries_total: int,
    descriptor_symbolized_sites: int,
    invoke_trampoline_symbolized_sites: int,
    missing_invoke_trampoline_sites: int,
    non_normalized_layout_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"block_literal_sites={block_literal_sites};"
        f"invoke_argument_slots_total={invoke_argument_slots_total};"
        f"capture_word_count_total={capture_word_count_total};"
        f"parameter_entries_total={parameter_entries_total};"
        f"capture_entries_total={capture_entries_total};"
        f"body_statement_entries_total={body_statement_entries_total};"
        f"descriptor_symbolized_sites={descriptor_symbolized_sites};"
        f"invoke_trampoline_symbolized_sites={invoke_trampoline_symbolized_sites};"
        f"missing_invoke_trampoline_sites={missing_invoke_trampoline_sites};"
        f"non_normalized_layout_sites={non_normalized_layout_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["block_abi_invoke_trampoline_lowering_sites"],
        sema["block_abi_invoke_trampoline_lowering_invoke_argument_slots"],
        sema["block_abi_invoke_trampoline_lowering_capture_word_count"],
        sema["block_abi_invoke_trampoline_lowering_parameter_entries"],
        sema["block_abi_invoke_trampoline_lowering_capture_entries"],
        sema["block_abi_invoke_trampoline_lowering_body_statement_entries"],
        sema["block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites"],
        sema["block_abi_invoke_trampoline_lowering_invoke_symbolized_sites"],
        sema["block_abi_invoke_trampoline_lowering_missing_invoke_sites"],
        sema["block_abi_invoke_trampoline_lowering_non_normalized_layout_sites"],
        sema["block_abi_invoke_trampoline_lowering_contract_violation_sites"],
    )


def _extract_profile_counts_and_deterministic(
    ir_text: str,
) -> tuple[tuple[int, int, int, int, int, int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:11])
            return counts, match.group(12) == "true"
    raise AssertionError(f"missing IR profile marker: {PROFILE_PREFIX}")


def _extract_lowering_replay_key(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(LOWERING_PREFIX):
            return line.removeprefix(LOWERING_PREFIX)
    raise AssertionError(f"missing IR replay-key marker: {LOWERING_PREFIX}")


def _extract_metadata_counts_and_deterministic(
    ir_text: str,
) -> tuple[tuple[int, int, int, int, int, int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:11])
            return counts, match.group(12) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !20 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"][
        "objc_block_abi_invoke_trampoline_lowering_surface"
    ]
    lowering = manifest["lowering_block_abi_invoke_trampoline"]

    block_literal_sites = sema["block_abi_invoke_trampoline_lowering_sites"]
    invoke_argument_slots_total = sema["block_abi_invoke_trampoline_lowering_invoke_argument_slots"]
    capture_word_count_total = sema["block_abi_invoke_trampoline_lowering_capture_word_count"]
    parameter_entries_total = sema["block_abi_invoke_trampoline_lowering_parameter_entries"]
    capture_entries_total = sema["block_abi_invoke_trampoline_lowering_capture_entries"]
    body_statement_entries_total = sema["block_abi_invoke_trampoline_lowering_body_statement_entries"]
    descriptor_symbolized_sites = sema["block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites"]
    invoke_trampoline_symbolized_sites = sema["block_abi_invoke_trampoline_lowering_invoke_symbolized_sites"]
    missing_invoke_trampoline_sites = sema["block_abi_invoke_trampoline_lowering_missing_invoke_sites"]
    non_normalized_layout_sites = sema["block_abi_invoke_trampoline_lowering_non_normalized_layout_sites"]
    contract_violation_sites = sema["block_abi_invoke_trampoline_lowering_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_block_abi_invoke_trampoline_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        block_literal_sites=block_literal_sites,
        invoke_argument_slots_total=invoke_argument_slots_total,
        capture_word_count_total=capture_word_count_total,
        parameter_entries_total=parameter_entries_total,
        capture_entries_total=capture_entries_total,
        body_statement_entries_total=body_statement_entries_total,
        descriptor_symbolized_sites=descriptor_symbolized_sites,
        invoke_trampoline_symbolized_sites=invoke_trampoline_symbolized_sites,
        missing_invoke_trampoline_sites=missing_invoke_trampoline_sites,
        non_normalized_layout_sites=non_normalized_layout_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert descriptor_symbolized_sites <= block_literal_sites
    assert invoke_trampoline_symbolized_sites <= block_literal_sites
    assert missing_invoke_trampoline_sites <= block_literal_sites
    assert non_normalized_layout_sites <= block_literal_sites
    assert contract_violation_sites <= block_literal_sites
    assert invoke_trampoline_symbolized_sites + missing_invoke_trampoline_sites == block_literal_sites
    assert invoke_argument_slots_total == parameter_entries_total
    assert capture_word_count_total == capture_entries_total

    assert deterministic_handoff is True
    assert sema["lowering_block_abi_invoke_trampoline_replay_key"] == expected_replay_key

    assert surface["block_literal_sites"] == block_literal_sites
    assert surface["invoke_argument_slots_total"] == invoke_argument_slots_total
    assert surface["capture_word_count_total"] == capture_word_count_total
    assert surface["parameter_entries_total"] == parameter_entries_total
    assert surface["capture_entries_total"] == capture_entries_total
    assert surface["body_statement_entries_total"] == body_statement_entries_total
    assert surface["descriptor_symbolized_sites"] == descriptor_symbolized_sites
    assert surface["invoke_trampoline_symbolized_sites"] == invoke_trampoline_symbolized_sites
    assert surface["missing_invoke_trampoline_sites"] == missing_invoke_trampoline_sites
    assert surface["non_normalized_layout_sites"] == non_normalized_layout_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m167_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M167 validation/conformance/perf block ABI invoke-trampoline runbook",
        "python -m pytest tests/tooling/test_objc3c_m167_frontend_block_abi_invoke_trampoline_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m167_sema_block_abi_invoke_trampoline_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m167_lowering_block_abi_invoke_trampoline_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m167_validation_block_abi_invoke_trampoline_contract.py -q",
        "lowering_block_abi_invoke_trampoline.replay_key",
        "deterministic_block_abi_invoke_trampoline_lowering_handoff",
        "block_abi_invoke_trampoline_lowering",
        "frontend_objc_block_abi_invoke_trampoline_lowering_profile",
        "!objc3.objc_block_abi_invoke_trampoline_lowering = !{!20}",
    ):
        assert text in fragment


def test_m167_validation_manifest_packets_hold_deterministic_block_abi_invoke_trampoline_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m167_validation_ir_markers_match_manifest_block_abi_invoke_trampoline_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_block_abi_invoke_trampoline_lowering = !{!20}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert (
            _extract_lowering_replay_key(ir_text)
            == manifest["lowering_block_abi_invoke_trampoline"]["replay_key"]
        )


def test_m167_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_block_abi_invoke_trampoline"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_block_abi_invoke_trampoline"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
