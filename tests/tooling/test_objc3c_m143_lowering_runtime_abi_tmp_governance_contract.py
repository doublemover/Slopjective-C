from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LOWERING_REGRESSION_SCRIPT = ROOT / "scripts" / "run_objc3c_lowering_regression_suite.ps1"
TYPED_ABI_REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_typed_abi_replay_proof.ps1"
LOWERING_REPLAY_SCRIPT = ROOT / "scripts" / "check_objc3c_lowering_replay_proof.ps1"
SEMANTICS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
ARTIFACTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
TESTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
CONTRACT_DOC = ROOT / "docs" / "contracts" / "artifact_tmp_governance_expectations.md"
PACKAGE_JSON = ROOT / "package.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_lane_c_scripts_default_run_ids_are_deterministic_and_tmp_governed() -> None:
    lowering_script_text = _read(LOWERING_REGRESSION_SCRIPT)
    typed_abi_script_text = _read(TYPED_ABI_REPLAY_SCRIPT)
    lowering_replay_script_text = _read(LOWERING_REPLAY_SCRIPT)

    assert '$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lowering-regression"' in lowering_script_text
    assert '$defaultRunId = "m143-lane-c-lowering-regression-default"' in lowering_script_text
    assert "$env:OBJC3C_NATIVE_LOWERING_RUN_ID" in lowering_script_text
    assert "Get-Date -Format" not in lowering_script_text

    assert '$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/typed-abi-replay-proof"' in typed_abi_script_text
    assert '$defaultRunId = "m143-lane-c-typed-abi-default"' in typed_abi_script_text
    assert "$env:OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID" in typed_abi_script_text
    assert "Get-Date -Format" not in typed_abi_script_text

    assert '$proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lowering-replay-proof"' in lowering_replay_script_text
    assert '$defaultProofRunId = "m143-lane-c-lowering-replay-proof-default"' in lowering_replay_script_text
    assert "$env:OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID" in lowering_replay_script_text
    assert "Get-Date -Format" not in lowering_replay_script_text


def test_lane_c_docs_and_contract_publish_deterministic_tmp_path_rules() -> None:
    assert "Lowering/LLVM IR/runtime-ABI lane-C closeout coverage" in _read(SEMANTICS_FRAGMENT)
    assert "m143-lane-c-lowering-regression-default" in _read(SEMANTICS_FRAGMENT)
    assert "m143-lane-c-typed-abi-default" in _read(SEMANTICS_FRAGMENT)
    assert "m143-lane-c-lowering-replay-proof-default" in _read(SEMANTICS_FRAGMENT)

    assert "tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json" in _read(
        ARTIFACTS_FRAGMENT
    )
    assert "tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json" in _read(
        ARTIFACTS_FRAGMENT
    )
    assert "tmp/artifacts/objc3c-native/lowering-replay-proof/<proof_run_id>/summary.json" in _read(
        ARTIFACTS_FRAGMENT
    )
    assert "OBJC3C_NATIVE_LOWERING_RUN_ID" in _read(TESTS_FRAGMENT)
    assert "OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID" in _read(TESTS_FRAGMENT)
    assert "OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID" in _read(TESTS_FRAGMENT)
    assert "| `M143-C001` |" in _read(CONTRACT_DOC)
    assert "m143-lane-c-lowering-regression-default" in _read(CONTRACT_DOC)
    assert "m143-lane-c-typed-abi-default" in _read(CONTRACT_DOC)
    assert "m143-lane-c-lowering-replay-proof-default" in _read(CONTRACT_DOC)


def test_m143_governance_test_script_includes_lane_c_contract_tests() -> None:
    package_payload = json.loads(_read(PACKAGE_JSON))
    test_script = package_payload["scripts"]["test:objc3c:m143-artifact-governance"]

    assert "tests/tooling/test_objc3c_lowering_contract.py" in test_script
    assert "tests/tooling/test_objc3c_ir_emitter_extraction.py" in test_script
    assert "tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py" in test_script
