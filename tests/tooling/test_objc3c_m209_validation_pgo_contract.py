import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
PACKAGE_JSON = ROOT / "package.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m209_validation_pgo_runbook_section_present() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M209 validation/perf profile-guided optimization runbook",
        "npm run test:objc3c:m145-direct-llvm-matrix",
        "npm run test:objc3c:m145-direct-llvm-matrix:lane-d",
        "npm run test:objc3c:execution-smoke",
        "npm run test:objc3c:execution-replay-proof",
        "npm run test:objc3c:perf-budget",
        "tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json",
        "tmp/artifacts/conformance-suite/<target>/summary.json",
        "tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json",
        "tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json",
        "cache_proof.run1.cache_hit",
        "cache_proof.run2.cache_hit",
        "run1_sha256",
        "run2_sha256",
        "python -m pytest tests/tooling/test_objc3c_m209_validation_pgo_contract.py -q",
    ):
        assert text in fragment


def test_m209_validation_pgo_commands_and_scripts_available() -> None:
    package_payload = json.loads(_read(PACKAGE_JSON))
    scripts = package_payload["scripts"]

    for script_name in (
        "test:objc3c:m145-direct-llvm-matrix",
        "test:objc3c:m145-direct-llvm-matrix:lane-d",
        "test:objc3c:execution-smoke",
        "test:objc3c:execution-replay-proof",
        "test:objc3c:perf-budget",
    ):
        assert script_name in scripts

    assert "scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1" in scripts[
        "test:objc3c:m145-direct-llvm-matrix"
    ]
    assert "scripts/check_conformance_suite.ps1" in scripts["test:objc3c:m145-direct-llvm-matrix:lane-d"]
    assert "npm run test:objc3c:perf-budget" in scripts["test:objc3c:m145-direct-llvm-matrix:lane-d"]
    assert "scripts/check_objc3c_native_execution_smoke.ps1" in scripts["test:objc3c:execution-smoke"]
    assert "scripts/check_objc3c_execution_replay_proof.ps1" in scripts["test:objc3c:execution-replay-proof"]
    assert "scripts/check_objc3c_native_perf_budget.ps1" in scripts["test:objc3c:perf-budget"]

    for relative_path in (
        "scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1",
        "scripts/check_conformance_suite.ps1",
        "scripts/check_objc3c_native_execution_smoke.ps1",
        "scripts/check_objc3c_execution_replay_proof.ps1",
        "scripts/check_objc3c_native_perf_budget.ps1",
    ):
        assert (ROOT / relative_path).is_file()
