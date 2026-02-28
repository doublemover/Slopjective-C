import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

FIXTURE_ROOT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "objc3c"
    / "m189_validation_task_runtime_interop_cancellation_contract"
)
CONFORMANCE_FIXTURE = FIXTURE_ROOT / "M189-D001.json"
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m189_conformance_fixture_fields_are_canonical() -> None:
    fixture = json.loads(_read(CONFORMANCE_FIXTURE))

    assert fixture["id"] == "M189-D001"
    assert fixture["bucket"] == "lowering_abi"
    assert fixture["profile"] == "core"
    assert fixture["tracking"]["milestone"] == "M189"
    assert fixture["tracking"]["milestone_number"] == 274
    assert fixture["tracking"]["lane"] == "D"
    assert fixture["tracking"]["issue"] == 4537
    assert fixture["tracking"]["task"] == "M189-D001"
    assert fixture["tracking"]["sequence"] == 1


def test_m189_conformance_fixture_references_runbook_and_lane_d_contract_tests() -> None:
    fixture = json.loads(_read(CONFORMANCE_FIXTURE))
    references = fixture["references"]
    source = fixture["source"]
    expect = fixture["expect"]

    assert "docs/objc3c-native/src/60-tests.md" in references
    assert (
        "tests/tooling/test_objc3c_m189_validation_task_runtime_interop_cancellation_contract.py"
        in references
    )
    assert (
        "tests/tooling/test_objc3c_m189_conformance_task_runtime_interop_cancellation_contract.py"
        in references
    )

    assert "// EXPECT: task_runtime_interop_cancellation_lowering" in source
    assert "// EXPECT: deterministic_task_runtime_interop_cancellation_lowering_handoff=true" in source

    assert expect["parse"] == "accept"
    assert expect["diagnostics"] == []


def test_m189_conformance_fixture_replay_anchor_matches_validation_packets() -> None:
    run1_manifest = json.loads(_read(RUN1_MANIFEST))
    run2_manifest = json.loads(_read(RUN2_MANIFEST))
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    run1_replay_key = run1_manifest["lowering_task_runtime_interop_cancellation"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_task_runtime_interop_cancellation"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert "lane_contract=m189-task-runtime-interop-cancellation-lowering-v1" in run1_replay_key

    assert f"; task_runtime_interop_cancellation_lowering = {run1_replay_key}" in run1_ir
    assert f"; task_runtime_interop_cancellation_lowering = {run2_replay_key}" in run2_ir
    assert "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}" in run1_ir
    assert "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}" in run2_ir
