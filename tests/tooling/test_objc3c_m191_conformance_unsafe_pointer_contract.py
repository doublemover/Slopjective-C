import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

FIXTURE_ROOT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "objc3c"
    / "m191_validation_unsafe_pointer_contract"
)
CONFORMANCE_FIXTURE = FIXTURE_ROOT / "M191-D001.json"
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m191_conformance_fixture_fields_are_canonical() -> None:
    fixture = json.loads(_read(CONFORMANCE_FIXTURE))

    assert fixture["id"] == "M191-D001"
    assert fixture["bucket"] == "lowering_abi"
    assert fixture["profile"] == "core"
    assert fixture["tracking"]["milestone"] == "M191"
    assert fixture["tracking"]["milestone_number"] == 276
    assert fixture["tracking"]["lane"] == "D"
    assert fixture["tracking"]["issue"] == 4547
    assert fixture["tracking"]["task"] == "M191-D001"
    assert fixture["tracking"]["sequence"] == 1


def test_m191_conformance_fixture_references_runbook_and_lane_d_contract_tests() -> None:
    fixture = json.loads(_read(CONFORMANCE_FIXTURE))
    references = fixture["references"]
    source = fixture["source"]
    expect = fixture["expect"]

    assert "docs/objc3c-native/src/60-tests.md" in references
    assert "tests/tooling/test_objc3c_m191_validation_unsafe_pointer_contract.py" in references
    assert "tests/tooling/test_objc3c_m191_conformance_unsafe_pointer_contract.py" in references

    assert "// EXPECT: unsafe_pointer_extension_lowering" in source
    assert "// EXPECT: deterministic_unsafe_pointer_extension_lowering_handoff=true" in source

    assert expect["parse"] == "accept"
    assert expect["diagnostics"] == []


def test_m191_conformance_fixture_replay_anchor_matches_validation_packets() -> None:
    run1_manifest = json.loads(_read(RUN1_MANIFEST))
    run2_manifest = json.loads(_read(RUN2_MANIFEST))
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    run1_replay_key = run1_manifest["lowering_unsafe_pointer_extension"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_unsafe_pointer_extension"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert "lane_contract=m191-unsafe-pointer-extension-gating-lowering-v1" in run1_replay_key

    assert f"; unsafe_pointer_extension_lowering = {run1_replay_key}" in run1_ir
    assert f"; unsafe_pointer_extension_lowering = {run2_replay_key}" in run2_ir
    assert "!objc3.objc_unsafe_pointer_extension_lowering = !{!37}" in run1_ir
    assert "!objc3.objc_unsafe_pointer_extension_lowering = !{!37}" in run2_ir
