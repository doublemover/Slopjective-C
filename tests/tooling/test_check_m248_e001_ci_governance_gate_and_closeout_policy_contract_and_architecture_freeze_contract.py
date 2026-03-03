from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m248_e001_ci_governance_gate_and_closeout_policy_contract_and_architecture_freeze_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m248_e001_ci_governance_gate_and_closeout_policy_contract_and_architecture_freeze_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_e001_ci_governance_gate_and_closeout_policy_contract_and_architecture_freeze_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m248-e001-lane-e-ci-governance-gate-closeout-policy-contract-architecture-freeze-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m248_e001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-E001/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m248_e001_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_dependency_token(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m248_lane_e_ci_governance_gate_and_closeout_policy_contract_and_architecture_freeze_e001_expectations.md"
    )
    original = contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8")
    anchor = "Dependencies: `M248-A001`, `M248-B001`, `M248-C001`, `M248-D001`"
    assert anchor in original
    drift_text = original.replace(anchor, "Dependencies: `M248-A001`, `M248-B001`, `M248-C099`, `M248-D001`")
    assert anchor not in drift_text
    drift_doc.write_text(drift_text, encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-E001-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m248-e001-lane-e-readiness": '
            '"npm run check:objc3c:m248-a001-lane-a-readiness '
            '&& npm run check:objc3c:m248-b001-lane-b-readiness '
            '&& npm run check:objc3c:m248-c001-lane-c-readiness '
            '&& npm run check:objc3c:m248-d001-lane-d-readiness '
            '&& npm run check:objc3c:m248-e001-ci-governance-gate-closeout-policy-contract-architecture-freeze-contract '
            '&& npm run test:tooling:m248-e001-ci-governance-gate-closeout-policy-contract-architecture-freeze-contract"',
            '"check:objc3c:m248-e001-lane-e-readiness": '
            '"npm run check:objc3c:m248-e001-ci-governance-gate-closeout-policy-contract-architecture-freeze-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-E001-PKG-03" for failure in payload["failures"])
