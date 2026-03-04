from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m247-c001-lowering-codegen-cost-profiling-controls-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_c001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-C001/")


def test_contract_fails_closed_when_expectations_drop_issue_anchor(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_c001_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Issue `#6742` defines canonical lane-C contract and architecture freeze scope.",
            "Issue `#6000` defines canonical lane-C contract and architecture freeze scope.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-C001-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_script_drops_pytest_gate(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m247_c001_lane_c_readiness.py"
    drift_runner.write_text(
        contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8").replace(
            "tests/tooling/test_check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py",
            "tests/tooling/missing_contract_test.py",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-C001-RUN-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_lane_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m247-c001-lane-c-readiness": "python scripts/run_m247_c001_lane_c_readiness.py"',
            '"check:objc3c:m247-c001-lane-c-readiness": '
            '"python scripts/check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-C001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_proof_optimization_input(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"proof:objc3c": ',
            '"proof:objc3c-disabled": ',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-C001-PKG-05" for failure in payload["failures"])

