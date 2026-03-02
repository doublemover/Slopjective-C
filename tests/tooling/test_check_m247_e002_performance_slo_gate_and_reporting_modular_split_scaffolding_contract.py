from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m247_e002_performance_slo_gate_and_reporting_modular_split_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_e002_performance_slo_gate_and_reporting_modular_split_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_e002_performance_slo_gate_and_reporting_modular_split_scaffolding_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m247-e002-performance-slo-gate-reporting-modular-split-scaffolding-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_e002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-E002/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m247_e002_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_dependency_token(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_lane_e_performance_slo_gate_and_reporting_modular_split_scaffolding_e002_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace("`M247-C002`", "`M247-C099`"),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-E002-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m247-e002-lane-e-readiness": '
            '"npm run check:objc3c:m247-e001-lane-e-readiness '
            '&& npm run check:objc3c:m247-e002-performance-slo-gate-reporting-modular-split-scaffolding-contract '
            '&& npm run test:tooling:m247-e002-performance-slo-gate-reporting-modular-split-scaffolding-contract"',
            '"check:objc3c:m247-e002-lane-e-readiness": '
            '"npm run check:objc3c:m247-e002-performance-slo-gate-reporting-modular-split-scaffolding-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-E002-PKG-03" for failure in payload["failures"])
