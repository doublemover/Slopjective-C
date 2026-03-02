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
    / "check_m249_e003_lane_e_release_gate_docs_and_runbooks_core_feature_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m249_e003_lane_e_release_gate_docs_and_runbooks_core_feature_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m249_e003_lane_e_release_gate_docs_and_runbooks_core_feature_implementation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m249-e003-lane-e-release-gate-docs-runbooks-core-feature-implementation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_e003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-E003/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m249_e003_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_dependency_token(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_lane_e_release_gate_docs_and_runbooks_core_feature_implementation_e003_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace("`M249-C003`", "`M249-C099`"),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E003-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m249-e003-lane-e-readiness": '
            '"npm run check:objc3c:m249-e002-lane-e-readiness '
            '&& npm run check:objc3c:m249-a003-lane-a-readiness '
            '&& npm run check:objc3c:m249-b003-lane-b-readiness '
            '&& npm run check:objc3c:m249-c003-lane-c-readiness '
            '&& npm run check:objc3c:m249-d003-lane-d-readiness '
            "&& npm run check:objc3c:m249-e003-lane-e-release-gate-docs-runbooks-core-feature-implementation-contract "
            '&& npm run test:tooling:m249-e003-lane-e-release-gate-docs-runbooks-core-feature-implementation-contract"',
            '"check:objc3c:m249-e003-lane-e-readiness": '
            '"npm run check:objc3c:m249-e003-lane-e-release-gate-docs-runbooks-core-feature-implementation-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E003-PKG-03" for failure in payload["failures"])
