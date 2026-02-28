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
    / "m155_validation_id_class_sel_object_pointer_typecheck_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m155-id-class-sel-object-pointer-typecheck-v1"
PROFILE_PREFIX = "; frontend_objc_id_class_sel_object_pointer_typecheck_profile = "
LOWERING_PREFIX = "; id_class_sel_object_pointer_typecheck_lowering = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_id_class_sel_object_pointer_typecheck_profile = "
    r"id_typecheck_sites=(\d+), class_typecheck_sites=(\d+), sel_typecheck_sites=(\d+), "
    r"object_pointer_typecheck_sites=(\d+), total_typecheck_sites=(\d+), "
    r"deterministic_id_class_sel_object_pointer_typecheck_handoff=(true|false)$"
)
METADATA_RE = re.compile(r"^!8 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i1 ([01])}$")


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    id_sites: int, class_sites: int, sel_sites: int, object_pointer_sites: int, total_sites: int
) -> str:
    return (
        f"id_typecheck_sites={id_sites};"
        f"class_typecheck_sites={class_sites};"
        f"sel_typecheck_sites={sel_sites};"
        f"object_pointer_typecheck_sites={object_pointer_sites};"
        f"total_typecheck_sites={total_sites};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_counts(manifest: dict) -> tuple[int, int, int, int, int]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        sema["id_typecheck_sites"],
        sema["class_typecheck_sites"],
        sema["sel_typecheck_sites"],
        sema["object_pointer_typecheck_sites"],
        sema["id_class_sel_object_pointer_typecheck_sites_total"],
    )


def _extract_profile_counts(ir_text: str) -> tuple[int, int, int, int, int]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            return tuple(int(group) for group in match.groups()[:5])
    raise AssertionError(f"missing IR profile marker: {PROFILE_PREFIX}")


def _extract_lowering_replay_key(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(LOWERING_PREFIX):
            return line.removeprefix(LOWERING_PREFIX)
    raise AssertionError(f"missing IR replay-key marker: {LOWERING_PREFIX}")


def _extract_metadata_counts(ir_text: str) -> tuple[int, int, int, int, int]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            return tuple(int(group) for group in match.groups()[:5])
    raise AssertionError("missing LLVM metadata tuple marker: !8 = !{i64 ..., i1 1}")


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"][
        "objc_id_class_sel_object_pointer_typecheck_surface"
    ]
    lowering = manifest["lowering_id_class_sel_object_pointer_typecheck"]

    id_sites = sema["id_typecheck_sites"]
    class_sites = sema["class_typecheck_sites"]
    sel_sites = sema["sel_typecheck_sites"]
    object_pointer_sites = sema["object_pointer_typecheck_sites"]
    total_sites = sema["id_class_sel_object_pointer_typecheck_sites_total"]
    expected_total = id_sites + class_sites + sel_sites + object_pointer_sites
    expected_replay_key = _expected_replay_key(
        id_sites, class_sites, sel_sites, object_pointer_sites, total_sites
    )

    assert total_sites == expected_total
    assert sema["deterministic_id_class_sel_object_pointer_typecheck_handoff"] is True
    assert sema["lowering_id_class_sel_object_pointer_typecheck_replay_key"] == expected_replay_key

    assert surface["id_typecheck_sites"] == id_sites
    assert surface["class_typecheck_sites"] == class_sites
    assert surface["sel_typecheck_sites"] == sel_sites
    assert surface["object_pointer_typecheck_sites"] == object_pointer_sites
    assert surface["total_typecheck_sites"] == total_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m155_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M155 validation/conformance/perf id/Class/SEL/object-pointer typecheck runbook",
        "python -m pytest tests/tooling/test_objc3c_m155_frontend_id_class_sel_object_pointer_typecheck_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m155_sema_id_class_sel_object_pointer_typecheck_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m155_lowering_id_class_sel_object_pointer_typecheck_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m155_validation_id_class_sel_object_pointer_typecheck_contract.py -q",
        "id_class_sel_object_pointer_typecheck_lowering",
        "frontend_objc_id_class_sel_object_pointer_typecheck_profile",
        "lowering_id_class_sel_object_pointer_typecheck.replay_key",
        "deterministic_id_class_sel_object_pointer_typecheck_handoff",
    ):
        assert text in fragment


def test_m155_validation_manifest_packets_hold_deterministic_typecheck_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m155_validation_ir_markers_match_manifest_typecheck_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts = _manifest_counts(manifest)

        assert "!objc3.objc_id_class_sel_object_pointer_typecheck = !{!8}" in ir_text
        assert _extract_profile_counts(ir_text) == manifest_counts
        assert _extract_metadata_counts(ir_text) == manifest_counts
        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_id_class_sel_object_pointer_typecheck"][
            "replay_key"
        ]


def test_m155_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_id_class_sel_object_pointer_typecheck"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_id_class_sel_object_pointer_typecheck"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
