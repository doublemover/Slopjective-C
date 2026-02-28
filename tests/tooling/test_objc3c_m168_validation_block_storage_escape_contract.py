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
    / "m168_validation_block_storage_escape_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m168-block-storage-escape-lowering-v1"
LOWERING_PREFIX = "; block_storage_escape_lowering = "
PROFILE_PREFIX = "; frontend_objc_block_storage_escape_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_block_storage_escape_lowering_profile = "
    r"block_literal_sites=(\d+), mutable_capture_count_total=(\d+), byref_slot_count_total=(\d+), "
    r"parameter_entries_total=(\d+), capture_entries_total=(\d+), body_statement_entries_total=(\d+), "
    r"requires_byref_cells_sites=(\d+), escape_analysis_enabled_sites=(\d+), escape_to_heap_sites=(\d+), "
    r"escape_profile_normalized_sites=(\d+), byref_layout_symbolized_sites=(\d+), "
    r"contract_violation_sites=(\d+), deterministic_block_storage_escape_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r"^!21 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    block_literal_sites: int,
    mutable_capture_count_total: int,
    byref_slot_count_total: int,
    parameter_entries_total: int,
    capture_entries_total: int,
    body_statement_entries_total: int,
    requires_byref_cells_sites: int,
    escape_analysis_enabled_sites: int,
    escape_to_heap_sites: int,
    escape_profile_normalized_sites: int,
    byref_layout_symbolized_sites: int,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"block_literal_sites={block_literal_sites};"
        f"mutable_capture_count_total={mutable_capture_count_total};"
        f"byref_slot_count_total={byref_slot_count_total};"
        f"parameter_entries_total={parameter_entries_total};"
        f"capture_entries_total={capture_entries_total};"
        f"body_statement_entries_total={body_statement_entries_total};"
        f"requires_byref_cells_sites={requires_byref_cells_sites};"
        f"escape_analysis_enabled_sites={escape_analysis_enabled_sites};"
        f"escape_to_heap_sites={escape_to_heap_sites};"
        f"escape_profile_normalized_sites={escape_profile_normalized_sites};"
        f"byref_layout_symbolized_sites={byref_layout_symbolized_sites};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int, int, int, int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["block_storage_escape_lowering_sites"],
        sema["block_storage_escape_lowering_mutable_capture_count"],
        sema["block_storage_escape_lowering_byref_slot_count"],
        sema["block_storage_escape_lowering_parameter_entries"],
        sema["block_storage_escape_lowering_capture_entries"],
        sema["block_storage_escape_lowering_body_statement_entries"],
        sema["block_storage_escape_lowering_requires_byref_cells_sites"],
        sema["block_storage_escape_lowering_escape_analysis_enabled_sites"],
        sema["block_storage_escape_lowering_escape_to_heap_sites"],
        sema["block_storage_escape_lowering_escape_profile_normalized_sites"],
        sema["block_storage_escape_lowering_byref_layout_symbolized_sites"],
        sema["block_storage_escape_lowering_contract_violation_sites"],
    )


def _extract_profile_counts_and_deterministic(
    ir_text: str,
) -> tuple[tuple[int, int, int, int, int, int, int, int, int, int, int, int], bool]:
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


def _extract_metadata_counts_and_deterministic(
    ir_text: str,
) -> tuple[tuple[int, int, int, int, int, int, int, int, int, int, int, int], bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:12])
            return counts, match.group(13) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !21 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_block_storage_escape_lowering_surface"]
    lowering = manifest["lowering_block_storage_escape"]

    block_literal_sites = sema["block_storage_escape_lowering_sites"]
    mutable_capture_count_total = sema["block_storage_escape_lowering_mutable_capture_count"]
    byref_slot_count_total = sema["block_storage_escape_lowering_byref_slot_count"]
    parameter_entries_total = sema["block_storage_escape_lowering_parameter_entries"]
    capture_entries_total = sema["block_storage_escape_lowering_capture_entries"]
    body_statement_entries_total = sema["block_storage_escape_lowering_body_statement_entries"]
    requires_byref_cells_sites = sema["block_storage_escape_lowering_requires_byref_cells_sites"]
    escape_analysis_enabled_sites = sema["block_storage_escape_lowering_escape_analysis_enabled_sites"]
    escape_to_heap_sites = sema["block_storage_escape_lowering_escape_to_heap_sites"]
    escape_profile_normalized_sites = sema["block_storage_escape_lowering_escape_profile_normalized_sites"]
    byref_layout_symbolized_sites = sema["block_storage_escape_lowering_byref_layout_symbolized_sites"]
    contract_violation_sites = sema["block_storage_escape_lowering_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_block_storage_escape_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        block_literal_sites=block_literal_sites,
        mutable_capture_count_total=mutable_capture_count_total,
        byref_slot_count_total=byref_slot_count_total,
        parameter_entries_total=parameter_entries_total,
        capture_entries_total=capture_entries_total,
        body_statement_entries_total=body_statement_entries_total,
        requires_byref_cells_sites=requires_byref_cells_sites,
        escape_analysis_enabled_sites=escape_analysis_enabled_sites,
        escape_to_heap_sites=escape_to_heap_sites,
        escape_profile_normalized_sites=escape_profile_normalized_sites,
        byref_layout_symbolized_sites=byref_layout_symbolized_sites,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert requires_byref_cells_sites <= block_literal_sites
    assert escape_analysis_enabled_sites <= block_literal_sites
    assert escape_to_heap_sites <= block_literal_sites
    assert escape_profile_normalized_sites <= block_literal_sites
    assert byref_layout_symbolized_sites <= block_literal_sites
    assert contract_violation_sites <= block_literal_sites
    assert mutable_capture_count_total == capture_entries_total
    assert byref_slot_count_total == capture_entries_total
    assert escape_analysis_enabled_sites == block_literal_sites
    assert requires_byref_cells_sites == escape_to_heap_sites

    assert deterministic_handoff is True
    assert sema["lowering_block_storage_escape_replay_key"] == expected_replay_key

    assert surface["block_literal_sites"] == block_literal_sites
    assert surface["mutable_capture_count_total"] == mutable_capture_count_total
    assert surface["byref_slot_count_total"] == byref_slot_count_total
    assert surface["parameter_entries_total"] == parameter_entries_total
    assert surface["capture_entries_total"] == capture_entries_total
    assert surface["body_statement_entries_total"] == body_statement_entries_total
    assert surface["requires_byref_cells_sites"] == requires_byref_cells_sites
    assert surface["escape_analysis_enabled_sites"] == escape_analysis_enabled_sites
    assert surface["escape_to_heap_sites"] == escape_to_heap_sites
    assert surface["escape_profile_normalized_sites"] == escape_profile_normalized_sites
    assert surface["byref_layout_symbolized_sites"] == byref_layout_symbolized_sites
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m168_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M168 validation/conformance/perf block storage escape runbook",
        "python -m pytest tests/tooling/test_objc3c_m168_frontend_block_storage_escape_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m168_sema_block_storage_escape_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m168_lowering_block_storage_escape_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m168_validation_block_storage_escape_contract.py -q",
        "lowering_block_storage_escape.replay_key",
        "deterministic_block_storage_escape_lowering_handoff",
        "block_storage_escape_lowering",
        "frontend_objc_block_storage_escape_lowering_profile",
        "!objc3.objc_block_storage_escape_lowering = !{!21}",
    ):
        assert text in fragment


def test_m168_validation_manifest_packets_hold_deterministic_block_storage_escape_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m168_validation_ir_markers_match_manifest_block_storage_escape_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_block_storage_escape_lowering = !{!21}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_block_storage_escape"]["replay_key"]


def test_m168_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_block_storage_escape"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_block_storage_escape"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
