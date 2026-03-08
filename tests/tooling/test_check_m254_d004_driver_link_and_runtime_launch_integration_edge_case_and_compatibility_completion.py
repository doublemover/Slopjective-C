from __future__ import annotations

import importlib.util
import json
import shutil
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m254_d004_driver_link_and_runtime_launch_integration_edge_case_and_compatibility_completion.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m254_d004_driver_link_and_runtime_launch_integration_edge_case_and_compatibility_completion",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT_PATH}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m254_d004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-D004/")


def test_contract_fails_closed_when_launch_helper_contract_id_drifts(tmp_path: Path) -> None:
    drift_helper = tmp_path / contract.DEFAULT_LAUNCH_CONTRACT_SCRIPT.name
    drift_helper.write_text(
        contract.DEFAULT_LAUNCH_CONTRACT_SCRIPT.read_text(encoding="utf-8").replace(
            contract.CONTRACT_ID,
            "objc3c-runtime-launch-integration/m254-d004-drifted",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--launch-contract-script",
            str(drift_helper),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M254-D004-HELP-01"
        for failure in payload["failures"]
    )


def test_dynamic_probe_validates_compile_proof_and_smoke(tmp_path: Path) -> None:
    if contract.resolve_pwsh() is None:
        pytest.skip("PowerShell is not available")
    if shutil.which("clang") is None and shutil.which("clang.exe") is None:
        pytest.skip("clang is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert payload["compile_case"]["registration_manifest"].endswith(contract.REGISTRATION_MANIFEST_ARTIFACT)
    assert payload["proof_case"]["digest"].endswith("digest.json")
    assert payload["smoke_case"]["summary"].endswith("summary.json")
