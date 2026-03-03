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
    / "check_m248_e004_ci_governance_gate_and_closeout_policy_core_feature_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m248_e004_ci_governance_gate_and_closeout_policy_core_feature_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_e004_ci_governance_gate_and_closeout_policy_core_feature_expansion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m248-e004-lane-e-ci-governance-gate-closeout-policy-core-feature-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m248_e004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-E004/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m248_e004_contract.py"),
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
        tmp_path / "m248_lane_e_ci_governance_gate_and_closeout_policy_core_feature_expansion_e004_expectations.md"
    )
    original = contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8")
    anchor = "Dependencies: `M248-E003`, `M248-A004`, `M248-B004`, `M248-C004`, `M248-D004`"
    assert anchor in original
    drift_text = original.replace(anchor, "Dependencies: `M248-E003`, `M248-A004`, `M248-B004`, `M248-C099`, `M248-D004`")
    assert anchor not in drift_text
    drift_doc.write_text(drift_text, encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-E004-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m248-e004-lane-e-readiness": '
            '"npm run check:objc3c:m248-e003-lane-e-readiness '
            '&& npm run check:objc3c:m248-a004-lane-a-readiness '
            '&& npm run check:objc3c:m248-b004-lane-b-readiness '
            '&& npm run check:objc3c:m248-c004-lane-c-readiness '
            '&& npm run check:objc3c:m248-d004-lane-d-readiness '
            '&& npm run check:objc3c:m248-e004-ci-governance-gate-closeout-policy-core-feature-expansion-contract '
            '&& npm run test:tooling:m248-e004-ci-governance-gate-closeout-policy-core-feature-expansion-contract"',
            '"check:objc3c:m248-e004-lane-e-readiness": '
            '"npm run check:objc3c:m248-e004-ci-governance-gate-closeout-policy-core-feature-expansion-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-E004-PKG-03" for failure in payload["failures"])


def test_contract_drift_findings_are_deterministic_across_runs(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path / "m248_lane_e_ci_governance_gate_and_closeout_policy_core_feature_expansion_e004_expectations.md"
    )
    drift_package = tmp_path / "package.json"

    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-core-feature-expansion/m248-e004-v1`",
            "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-core-feature-expansion/m248-e004-drift`",
            1,
        ),
        encoding="utf-8",
    )
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"proof:objc3c": ',
            '"proof:objc3c-disabled": ',
            1,
        ),
        encoding="utf-8",
    )

    summary_one = tmp_path / "summary_one.json"
    summary_two = tmp_path / "summary_two.json"

    exit_one = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--package-json",
            str(drift_package),
            "--summary-out",
            str(summary_one),
        ]
    )
    exit_two = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--package-json",
            str(drift_package),
            "--summary-out",
            str(summary_two),
        ]
    )

    assert exit_one == 1
    assert exit_two == 1

    payload_one = json.loads(summary_one.read_text(encoding="utf-8"))
    payload_two = json.loads(summary_two.read_text(encoding="utf-8"))
    assert payload_one["ok"] is False
    assert payload_two["ok"] is False
    assert payload_one["checks_total"] == payload_two["checks_total"]
    assert payload_one["checks_passed"] == payload_two["checks_passed"]
    assert payload_one["failures"] == payload_two["failures"]
