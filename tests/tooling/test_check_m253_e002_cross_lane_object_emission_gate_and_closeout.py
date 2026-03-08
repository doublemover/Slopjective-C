from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_e002_cross_lane_object_emission_gate_and_closeout.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_e002_cross_lane_object_emission_gate_and_closeout",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_e002_cross_lane_object_emission_gate_and_closeout.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-e002-cross-lane-object-emission-gate-and-closeout-v1"
    assert payload["contract_id"] == "objc3c-runtime-cross-lane-object-emission-closeout/m253-e002-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 70
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False
    assert payload["closeout_ready_for_startup_registration"] is False
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_e002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("C:/Users/sneak/Development/Slopjective-C/tmp/reports/m253/M253-E002/")


def test_contract_fails_closed_when_expectations_drop_integrated_case(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m253_e002_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`fanin-distinct-linker-discovery-closeout`",
            "`fanin-broken-closeout`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--skip-dynamic-probes",
            "--expectations-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M253-E002-DOC-EXP-04" for failure in payload["failures"]
    )


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m253-e002-lane-e-readiness": "python scripts/run_m253_e002_lane_e_readiness.py"',
            '"check:objc3c:m253-e002-lane-e-readiness": "python scripts/check_m253_e002_cross_lane_object_emission_gate_and_closeout.py"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--skip-dynamic-probes",
            "--package-json",
            str(drift_package),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M253-E002-PKG-03" for failure in payload["failures"]
    )


def test_contract_fails_closed_when_e001_summary_is_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_summary = tmp_path / "missing_e001.json"
    exit_code = contract.run(
        [
            "--skip-dynamic-probes",
            "--e001-summary",
            str(missing_summary),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M253-E001-SUM-EXISTS" for failure in payload["failures"]
    )
