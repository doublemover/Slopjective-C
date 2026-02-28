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
    / "m161_validation_ownership_qualifier_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m161-ownership-qualifier-lowering-v1"
LOWERING_PREFIX = "; ownership_qualifier_lowering = "
PROFILE_PREFIX = "; frontend_objc_ownership_qualifier_lowering_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_ownership_qualifier_lowering_profile = "
    r"ownership_qualifier_sites=(\d+), invalid_ownership_qualifier_sites=(\d+), "
    r"object_pointer_type_annotation_sites=(\d+), deterministic_ownership_qualifier_lowering_handoff=(true|false)$"
)
METADATA_RE = re.compile(r"^!14 = !{i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$")


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    ownership_qualifier_sites: int,
    invalid_ownership_qualifier_sites: int,
    object_pointer_type_annotation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    return (
        f"ownership_qualifier_sites={ownership_qualifier_sites};"
        f"invalid_ownership_qualifier_sites={invalid_ownership_qualifier_sites};"
        f"object_pointer_type_annotation_sites={object_pointer_type_annotation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites"],
        sema["ownership_qualifier_lowering_type_annotation_invalid_ownership_qualifier_sites"],
        sema["ownership_qualifier_lowering_type_annotation_object_pointer_type_sites"],
    )


def _extract_profile_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, int, int], bool]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:3])
            return counts, match.group(4) == "true"
    raise AssertionError(f"missing IR profile marker: {PROFILE_PREFIX}")


def _extract_lowering_replay_key(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(LOWERING_PREFIX):
            return line.removeprefix(LOWERING_PREFIX)
    raise AssertionError(f"missing IR replay-key marker: {LOWERING_PREFIX}")


def _extract_metadata_counts_and_deterministic(ir_text: str) -> tuple[tuple[int, int, int], bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:3])
            return counts, match.group(4) == "1"
    raise AssertionError("missing LLVM metadata tuple marker: !14 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_ownership_qualifier_lowering_surface"]
    lowering = manifest["lowering_ownership_qualifier"]

    ownership_qualifier_sites = sema[
        "ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites"
    ]
    invalid_ownership_qualifier_sites = sema[
        "ownership_qualifier_lowering_type_annotation_invalid_ownership_qualifier_sites"
    ]
    object_pointer_type_annotation_sites = sema[
        "ownership_qualifier_lowering_type_annotation_object_pointer_type_sites"
    ]
    deterministic_handoff = sema["deterministic_ownership_qualifier_lowering_handoff"]

    expected_replay_key = _expected_replay_key(
        ownership_qualifier_sites=ownership_qualifier_sites,
        invalid_ownership_qualifier_sites=invalid_ownership_qualifier_sites,
        object_pointer_type_annotation_sites=object_pointer_type_annotation_sites,
        deterministic=deterministic_handoff,
    )

    assert invalid_ownership_qualifier_sites <= ownership_qualifier_sites
    assert ownership_qualifier_sites <= object_pointer_type_annotation_sites

    assert deterministic_handoff is True
    assert sema["lowering_ownership_qualifier_replay_key"] == expected_replay_key

    assert surface["ownership_qualifier_sites"] == ownership_qualifier_sites
    assert surface["invalid_ownership_qualifier_sites"] == invalid_ownership_qualifier_sites
    assert (
        surface["object_pointer_type_annotation_sites"]
        == object_pointer_type_annotation_sites
    )
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m161_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M161 validation/conformance/perf ownership-qualifier runbook",
        "python -m pytest tests/tooling/test_objc3c_m161_frontend_ownership_qualifier_parser_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m161_sema_ownership_qualifier_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m161_lowering_ownership_qualifier_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m161_validation_ownership_qualifier_contract.py -q",
        "lowering_ownership_qualifier.replay_key",
        "deterministic_ownership_qualifier_lowering_handoff",
        "ownership_qualifier_lowering",
        "frontend_objc_ownership_qualifier_lowering_profile",
        "!objc3.objc_ownership_qualifier_lowering = !{!14}",
    ):
        assert text in fragment


def test_m161_validation_manifest_packets_hold_deterministic_ownership_qualifier_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m161_validation_ir_markers_match_manifest_ownership_qualifier_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_ownership_qualifier_lowering = !{!14}" in ir_text
        profile_counts, profile_deterministic = _extract_profile_counts_and_deterministic(ir_text)
        metadata_counts, metadata_deterministic = _extract_metadata_counts_and_deterministic(ir_text)

        assert profile_counts == manifest_counts
        assert metadata_counts == manifest_counts
        assert profile_deterministic is True
        assert metadata_deterministic is True
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_ownership_qualifier"]["replay_key"]


def test_m161_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_ownership_qualifier"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_ownership_qualifier"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
